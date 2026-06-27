#include "DASelectComboBox.h"

#include "DAChipsEdit.h"
#include "dawidgets_resources.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <QTimer>
#include <QVBoxLayout>

namespace DA
{
namespace {
// 隐形占位 QLineEdit：QComboBox editable 模式需要一个 lineEdit，这里让它透明、
// 不绘制、不接收鼠标、不参与焦点，编辑区完全交给覆盖在上层的 DAChipsEdit。
class HiddenLineEdit : public QLineEdit
{
public:
    explicit HiddenLineEdit(QWidget *parent = nullptr) : QLineEdit(parent)
    {
        setFrame(false);
        setFocusPolicy(Qt::NoFocus);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
    QSize sizeHint() const override { return QSize(0, 0); }
    QSize minimumSizeHint() const override { return QSize(0, 0); }
protected:
    void paintEvent(QPaintEvent *) override {}
};
}  // namespace

class DASelectComboBox::PrivateData
{
public:
    PrivateData(DASelectComboBox *) {}
    DAChipsEdit *chips { nullptr };
    QWidget *container { nullptr };
    QScrollArea *scroll { nullptr };
    QListWidget *popup { nullptr };
    QStringList options;
    bool allowCreate { true };
    bool filterable { true };
    HeightMode heightMode { HeightMode::GrowWithTags };
    int fixedRowCount { 1 };
    bool popupVisible { false };
};

DASelectComboBox::DASelectComboBox(QWidget *parent) : QComboBox(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    setObjectName(QStringLiteral("DASelectComboBox"));

    // 先创建 chips 和 container,sizeHint() 会访问它们,必须在 setEditable 之前就绪
    d->chips = new DAChipsEdit;
    d->chips->setFrameShape(QFrame::NoFrame);
    d->chips->setAllowCreate(false);
    d->chips->setSeparators(QList<DAChipsEdit::Separator>{});

    d->container = new QWidget(this);
    auto *clay = new QVBoxLayout(d->container);
    clay->setContentsMargins(0, 0, 0, 0);
    clay->setSpacing(0);

    applyHeightMode();  // 初始 GrowWithTags：把 m_chips 放进 container

    // setEditable 会触发 adjustSize → sizeHint,此时 chips/container 必须已就绪
    setEditable(true);
    setLineEdit(new HiddenLineEdit(this));  // 占位，实际编辑由 DAChipsEdit 接管

    d->popup = new QListWidget;
    d->popup->setObjectName(QStringLiteral("DASelectPopup"));
    // 用 ToolTip 而非 Popup：Popup 在 QComboBox editable 模式下鼠标 grab 会失效，
    // 既不自动关闭也拦截外部事件。ToolTip 不抢抓取、不抢焦点，关闭完全由
    // eventFilter 手动管理，行为可控且可靠。
    d->popup->setWindowFlags(Qt::ToolTip);
    d->popup->setFocusPolicy(Qt::NoFocus);
    d->popup->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->popup->setSelectionMode(QAbstractItemView::SingleSelection);
    d->popup->setMouseTracking(true);
    d->popup->setMinimumHeight(40);
    d->popup->hide();

    connect(d->chips, &DAChipsEdit::inputTextChanged, this, &DASelectComboBox::onInputTextChanged);
    connect(d->chips, &DAChipsEdit::inputFocusChanged, this, &DASelectComboBox::onInputFocusChanged);
    connect(d->chips, &DAChipsEdit::inputKeyEvent, this, &DASelectComboBox::onInputKey);
    connect(d->chips, &DAChipsEdit::tagsChanged, this, [this](const QStringList &) {
        DA_D(d);
        if (d->heightMode == HeightMode::GrowWithTags)
            updateGeometry();
    });
    connect(d->chips, &DAChipsEdit::tagsChanged, this, &DASelectComboBox::tagsChanged);
    connect(d->popup, &QListWidget::itemClicked, this, &DASelectComboBox::onPopupItemClicked);

    // 监听全局鼠标按下：popup 可见时点击外部（非 combobox、非 popup 自身）应关闭。
    // Qt::ToolTip 不抢 mouse grab，外部点击由本过滤器兜底关闭。
    qApp->installEventFilter(this);

    // 默认现代化样式：圆角边框 + 无边框 drop-down + 内置 PNG 箭头。
    // 用户可随后自行 setStyleSheet 覆盖。用本对象名作用域，避免污染其它 QComboBox。
    applyDefaultStyle();
}

DASelectComboBox::~DASelectComboBox() = default;

void DASelectComboBox::applyDefaultStyle()
{
    // 把内嵌 PNG 写到缓存目录，qss 用 url(绝对路径) 引用。
    // 不用 Qt RCC：静态库的资源不会自动传递给链接它的可执行文件。
    // 资源由 tools/generate_resources.py 从 tools/*.png 生成到 dawidgets_resources.*。
    static const QString kArrowPath = []() {
        const QString dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        QDir().mkpath(dir);
        const QString path = dir + QStringLiteral("/dawidgets_arrow_down.png");
        const QPixmap pm = dawidgets::resources::load(QStringLiteral("arrow_down"));
        if (!pm.isNull())
            pm.save(path, "PNG");
        return path;
    }();

    static const char *kDefaultQssTpl = R"(
DA--DASelectComboBox {
    background:#ffffff;
    border:1px solid #dcdfe6;
    border-radius:4px;
    padding:1px 24px 1px 6px;
    min-height:22px;
}
DA--DASelectComboBox:on { border:1px solid #409eff; }
DA--DASelectComboBox:focus { border:1px solid #409eff; }
DA--DASelectComboBox::drop-down {
    subcontrol-origin:padding;
    subcontrol-position:top right;
    width:20px;
    border:none;
    border-left:1px solid #dcdfe6;
    border-top-right-radius:4px;
    border-bottom-right-radius:4px;
    background:transparent;
}
DA--DASelectComboBox::drop-down:hover { background:#f5f7fa; }
DA--DASelectComboBox::down-arrow {
    image:url(%1);
    width:10px;
    height:6px;
}
DA--DAChipsEdit { background:transparent; border:none; }
)";
    setStyleSheet(QString::fromLatin1(kDefaultQssTpl).arg(kArrowPath));
}

QStringList DASelectComboBox::options() const
{
    DA_DC(d);
    return d->options;
}

void DASelectComboBox::setOptions(const QStringList &options)
{
    DA_D(d);
    d->options = options;
    if (d->popupVisible)
        refreshPopup(d->chips->inputText());
}

QStringList DASelectComboBox::tags() const
{
    DA_DC(d);
    return d->chips->tags();
}

void DASelectComboBox::setTags(const QStringList &tags)
{
    DA_D(d);
    d->chips->setTags(tags);
}

void DASelectComboBox::clear()
{
    DA_D(d);
    d->chips->clear();
    d->chips->clearInput();
    hideCustomPopup();
}

bool DASelectComboBox::allowCreate() const
{
    DA_DC(d);
    return d->allowCreate;
}

void DASelectComboBox::setAllowCreate(bool on)
{
    DA_D(d);
    d->allowCreate = on;
}

bool DASelectComboBox::filterable() const
{
    DA_DC(d);
    return d->filterable;
}

void DASelectComboBox::setFilterable(bool on)
{
    DA_D(d);
    d->filterable = on;
}

QString DASelectComboBox::placeholder() const
{
    DA_DC(d);
    return d->chips->placeholder();
}

void DASelectComboBox::setPlaceholder(const QString &text)
{
    DA_D(d);
    d->chips->setPlaceholder(text);
}

DASelectComboBox::HeightMode DASelectComboBox::heightMode() const
{
    DA_DC(d);
    return d->heightMode;
}

void DASelectComboBox::setHeightMode(HeightMode mode)
{
    DA_D(d);
    if (d->heightMode == mode)
        return;
    d->heightMode = mode;
    applyHeightMode();
}

int DASelectComboBox::fixedRowCount() const
{
    DA_DC(d);
    return d->fixedRowCount;
}

void DASelectComboBox::setFixedRowCount(int rows)
{
    DA_D(d);
    d->fixedRowCount = qMax(1, rows);
    if (d->heightMode == HeightMode::FixedHeightScrollable)
        updateGeometry();
}

QSize DASelectComboBox::sizeHint() const
{
    DA_DC(d);
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    const int fw = style()->pixelMetric(QStyle::PM_ComboBoxFrameWidth, &opt, this);
    const QRect arrow = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

    int h;
    if (d->heightMode == HeightMode::GrowWithTags) {
        // 用当前宽度算 DAChipsEdit 换行后的高度（FlowLayout 支持 heightForWidth）。
        // 直接调 layout()->heightForWidth 绕过 QWidget::heightForWidth 的 isVisible 检查，
        // 保证 sizeHint 协商阶段（widget 可能尚未可见）也能得到正确换行高度。
        int cw = width();
        if (cw <= 0)
            cw = 200;
        const int chipsW = qMax(cw - fw * 2 - arrow.width(), d->chips->minimumSizeHint().width());
        h = d->chips->layout() ? d->chips->layout()->heightForWidth(chipsW) : -1;
        if (h < 0)
            h = d->chips->sizeHint().height();
    } else {
        h = d->chips->rowHeightHint() * qMax(1, d->fixedRowCount);
    }
    h += fw * 2;

    int w = d->chips->sizeHint().width();
    const QFontMetrics fm = fontMetrics();
    for (const QString &o : d->options) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        w = qMax(w, fm.horizontalAdvance(o));
#else
        w = qMax(w, fm.width(o));
#endif
    }
    w += fw * 2 + arrow.width();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QSize(w, h);
#else
    return QSize(w, h).expandedTo(QApplication::globalStrut());
#endif
}

QSize DASelectComboBox::minimumSizeHint() const
{
    DA_DC(d);
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    const int fw = style()->pixelMetric(QStyle::PM_ComboBoxFrameWidth, &opt, this);
    int h = d->chips->rowHeightHint();
    if (d->heightMode == HeightMode::FixedHeightScrollable)
        h *= qMax(1, d->fixedRowCount);
    h += fw * 2;
    const QRect arrow = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QSize(arrow.width() + 40, h);
#else
    return QSize(arrow.width() + 40, h).expandedTo(QApplication::globalStrut());
#endif
}

void DASelectComboBox::resizeEvent(QResizeEvent *event)
{
    QComboBox::resizeEvent(event);
    DA_D(d);
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    const QRect field = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
    d->container->setGeometry(field);
}

void DASelectComboBox::showPopup()
{
    // 不调用 QComboBox::showPopup：用自己的 QListWidget 弹出，支持过滤
    DA_D(d);
    if (d->popupVisible)
        hideCustomPopup();
    else
        showCustomPopup();
}

bool DASelectComboBox::eventFilter(QObject *watched, QEvent *event)
{
    DA_D(d);
    if (d->popupVisible) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            // 任何鼠标按下都关闭，除非点在 popup 内部（由 itemClicked 处理选中）。
            // 点在 combobox 上时，showPopup 的 toggle 逻辑会接管开关，这里也关闭，
            // 随后 QComboBox 触发的 showPopup 会重新打开——实现"点箭头不闪、点其它关闭"。
            auto *w = qobject_cast< QWidget * >(watched);
            if (w && w != d->popup && !d->popup->isAncestorOf(w))
                hideCustomPopup();
            break;
        }
        case QEvent::KeyPress: {
            // popup 可见时，非输入框来源的 Escape/方向键/回车由 onInputKey 处理；
            // 此处兜底：任何 Tab/焦点切换键关闭下拉。
            auto *ke = static_cast< QKeyEvent * >(event);
            if (ke->key() == Qt::Key_Tab || ke->key() == Qt::Key_Backtab)
                hideCustomPopup();
            break;
        }
        default:
            break;
        }
    }
    return QComboBox::eventFilter(watched, event);
}

void DASelectComboBox::onInputTextChanged(const QString &text)
{
    DA_D(d);
    if (!d->filterable)
        return;
    if (text.isEmpty()) {
        if (d->popupVisible)
            refreshPopup(QString());
    } else {
        showCustomPopup();
        refreshPopup(text);
    }
}

void DASelectComboBox::onInputFocusChanged(bool focused)
{
    DA_D(d);
    if (focused) {
        if (d->filterable && !d->options.isEmpty())
            showCustomPopup();
    }
    // 失焦时不主动关闭：Qt::Popup 在外部点击时自动关闭，选中/Esc 会显式关闭，
    // 避免与 popup itemClicked 的点击事件竞争导致误关。
}

void DASelectComboBox::onInputKey(QKeyEvent *event, bool *handled)
{
    DA_D(d);
    const int key = event->key();
    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
        if (d->popupVisible && d->popup->currentItem()) {
            pickCurrent();
            *handled = true;
        } else if (d->allowCreate && !d->chips->inputText().trimmed().isEmpty()) {
            const QString text = d->chips->inputText().trimmed();
            d->chips->appendTag(text);
            Q_EMIT created(text);
            d->chips->clearInput();
            *handled = true;
        }
        return;
    }
    if (!d->popupVisible)
        return;
    if (key == Qt::Key_Down) {
        const int r = d->popup->currentRow();
        if (r < d->popup->count() - 1)
            d->popup->setCurrentRow(r + 1);
        *handled = true;
    } else if (key == Qt::Key_Up) {
        const int r = d->popup->currentRow();
        if (r > 0)
            d->popup->setCurrentRow(r - 1);
        *handled = true;
    } else if (key == Qt::Key_Escape) {
        hideCustomPopup();
        d->chips->clearInput();
        *handled = true;
    }
}

void DASelectComboBox::onPopupItemClicked(QListWidgetItem *item)
{
    DA_D(d);
    if (!item)
        return;
    d->chips->appendTag(item->text());
    Q_EMIT optionSelected(item->text());
    d->chips->clearInput();
    if (d->popupVisible)
        refreshPopup(QString());
}

void DASelectComboBox::applyHeightMode()
{
    DA_D(d);
    auto *lay = static_cast< QVBoxLayout * >(d->container->layout());
    if (d->heightMode == HeightMode::GrowWithTags) {
        if (d->scroll) {
            d->scroll->takeWidget();  // 归还 m_chips 所有权
            lay->removeWidget(d->scroll);
            d->scroll->deleteLater();
            d->scroll = nullptr;
        }
        d->chips->setParent(d->container);
        lay->addWidget(d->chips);
    } else {
        lay->removeWidget(d->chips);
        d->scroll = new QScrollArea(d->container);
        d->scroll->setFrameShape(QFrame::NoFrame);
        d->scroll->setWidgetResizable(true);
        d->scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        d->scroll->setFocusPolicy(Qt::NoFocus);
        d->scroll->setWidget(d->chips);
        lay->addWidget(d->scroll);
    }
    updateGeometry();
}

void DASelectComboBox::showCustomPopup()
{
    DA_D(d);
    if (d->popupVisible)
        return;
    refreshPopup(d->chips->inputText());
    if (d->popup->count() == 0)
        return;
    d->popup->setFixedWidth(width());
    const int h = qMin(d->popup->sizeHintForRow(0) * d->popup->count() + 6, 220);
    d->popup->setFixedHeight(h);
    d->popup->move(mapToGlobal(QPoint(0, height())));
    d->popup->show();
    d->popup->raise();
    d->popup->setCurrentRow(0);  // default-first-option
    d->popupVisible = true;
}

void DASelectComboBox::hideCustomPopup()
{
    DA_D(d);
    if (!d->popupVisible)
        return;
    d->popup->hide();
    d->popupVisible = false;
}

void DASelectComboBox::refreshPopup(const QString &filter)
{
    DA_D(d);
    d->popup->clear();
    const QStringList selected = d->chips->tags();
    for (const QString &opt : d->options) {
        if (selected.contains(opt))
            continue;
        if (filter.isEmpty() || opt.contains(filter, Qt::CaseInsensitive))
            d->popup->addItem(opt);
    }
    if (d->popup->count() == 0 && d->popupVisible) {
        hideCustomPopup();
        return;
    }
    if (d->popup->count() > 0)
        d->popup->setCurrentRow(0);
}

void DASelectComboBox::pickCurrent()
{
    DA_D(d);
    QListWidgetItem *item = d->popup->currentItem();
    if (!item)
        return;
    d->chips->appendTag(item->text());
    Q_EMIT optionSelected(item->text());
    d->chips->clearInput();
    if (d->popupVisible)
        refreshPopup(QString());
}
}  // namespace DA
