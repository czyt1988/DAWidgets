#include "DASelectComboBox.h"

#include "DAChipsEdit.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QResizeEvent>
#include <QScrollArea>
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

/**
 * @brief 构造多标签选择下拉框
 * @param parent 父控件
 * @details 创建 DAChipsEdit 编辑区、隐形占位 QLineEdit、QListWidget 候选弹出框，
 *          安装全局事件过滤器与默认 qss 样式。编辑区组件必须在 setEditable 之前
 *          就绪，否则 sizeHint 协商会访问空指针。
 */
DASelectComboBox::DASelectComboBox(QWidget *parent) : QComboBox(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    Q_INIT_RESOURCE(dawidgetsicons);  // 静态库需显式注册 qrc 资源,否则被链接器丢弃
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

    // 默认现代化样式：圆角边框 + 无边框 drop-down + qrc 内置 SVG 箭头。
    // 用户可随后自行 setStyleSheet 覆盖。用本对象名作用域，避免污染其它 QComboBox。
    applyDefaultStyle();
}

/**
 * @brief 析构
 * @details PIMPL 指针由 DA_PIMPL_CONSTRUCT 宏自动释放
 */
DASelectComboBox::~DASelectComboBox() = default;

/**
 * @brief 应用默认现代化样式
 * @details qss 直接引用 qrc 中的 SVG 箭头资源（:/DAWidgetsIcons/arrow-down.svg）。
 *          静态库通过 Q_INIT_RESOURCE(dawidgetsicons) 显式注册资源,确保被链接器保留。
 */
void DASelectComboBox::applyDefaultStyle()
{
    static const char *kDefaultQss = R"(
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
    image:url(:/DAWidgetsIcons/arrow-down.svg);
    width:10px;
    height:6px;
}
DA--DAChipsEdit { background:transparent; border:none; }
)";
    setStyleSheet(QString::fromLatin1(kDefaultQss));
}

/**
 * @brief 获取候选选项列表
 * @return 当前候选选项的字符串列表
 */
QStringList DASelectComboBox::options() const
{
    DA_DC(d);
    return d->options;
}

/**
 * @brief 设置候选选项列表
 * @param options 候选选项字符串列表
 * @details 替换当前所有候选选项，若弹出框可见则立即刷新过滤结果
 */
void DASelectComboBox::setOptions(const QStringList &options)
{
    DA_D(d);
    d->options = options;
    if (d->popupVisible)
        refreshPopup(d->chips->inputText());
}

/**
 * @brief 获取已选标签列表
 * @return 当前已选标签的字符串列表
 */
QStringList DASelectComboBox::tags() const
{
    DA_DC(d);
    return d->chips->tags();
}

/**
 * @brief 设置已选标签列表
 * @param tags 标签字符串列表
 * @details 委托给 DAChipsEdit 替换当前所有标签
 */
void DASelectComboBox::setTags(const QStringList &tags)
{
    DA_D(d);
    d->chips->setTags(tags);
}

/**
 * @brief 清空所有已选标签与候选选项
 * @details 同时清空 DAChipsEdit 标签与输入框，并关闭弹出框
 */
void DASelectComboBox::clear()
{
    DA_D(d);
    d->chips->clear();
    d->chips->clearInput();
    hideCustomPopup();
}

/**
 * @brief 是否允许在无匹配时创建新标签
 * @return 允许创建返回 true
 */
bool DASelectComboBox::allowCreate() const
{
    DA_DC(d);
    return d->allowCreate;
}

/**
 * @brief 设置是否允许创建新标签
 * @param on 是否允许
 */
void DASelectComboBox::setAllowCreate(bool on)
{
    DA_D(d);
    d->allowCreate = on;
}

/**
 * @brief 下拉列表是否支持输入过滤
 * @return 支持过滤返回 true
 */
bool DASelectComboBox::filterable() const
{
    DA_DC(d);
    return d->filterable;
}

/**
 * @brief 设置下拉列表是否支持输入过滤
 * @param on 是否支持过滤
 */
void DASelectComboBox::setFilterable(bool on)
{
    DA_D(d);
    d->filterable = on;
}

/**
 * @brief 获取输入框占位提示文本
 * @return 占位提示文本
 */
QString DASelectComboBox::placeholder() const
{
    DA_DC(d);
    return d->chips->placeholder();
}

/**
 * @brief 设置输入框占位提示文本
 * @param text 占位提示文本
 */
void DASelectComboBox::setPlaceholder(const QString &text)
{
    DA_D(d);
    d->chips->setPlaceholder(text);
}

/**
 * @brief 获取当前高度策略
 * @return 高度策略枚举值
 */
DASelectComboBox::HeightMode DASelectComboBox::heightMode() const
{
    DA_DC(d);
    return d->heightMode;
}

/**
 * @brief 设置高度策略
 * @param mode 高度策略
 * @details 若策略变化则重新组装编辑区容器布局（GrowWithTags 直接放入，
 *          FixedHeightScrollable 包入 QScrollArea）
 */
void DASelectComboBox::setHeightMode(HeightMode mode)
{
    DA_D(d);
    if (d->heightMode == mode)
        return;
    d->heightMode = mode;
    applyHeightMode();
}

/**
 * @brief 获取固定高度模式下的显示行数
 * @return 显示行数
 */
int DASelectComboBox::fixedRowCount() const
{
    DA_DC(d);
    return d->fixedRowCount;
}

/**
 * @brief 设置固定高度模式下的显示行数
 * @param rows 显示行数，最小为 1
 * @details 若当前处于固定高度模式则触发 updateGeometry 重新协商尺寸
 */
void DASelectComboBox::setFixedRowCount(int rows)
{
    DA_D(d);
    d->fixedRowCount = qMax(1, rows);
    if (d->heightMode == HeightMode::FixedHeightScrollable)
        updateGeometry();
}

/**
 * @brief 计算推荐尺寸
 * @return 推荐的 QSize
 * @details GrowWithTags 模式下用 layout()->heightForWidth 计算换行高度，
 *          绕过 QWidget::heightForWidth 的 isVisible 检查以保证协商阶段正确；
 *          FixedHeightScrollable 模式下用 rowHeightHint * fixedRowCount。
 *          宽度取所有候选选项的最大文本宽度与 DAChipsEdit 宽度的较大值。
 */
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
    return QSize(w, h);
}

/**
 * @brief 计算最小推荐尺寸
 * @return 最小 QSize
 * @details 高度基于单行 rowHeightHint，FixedHeightScrollable 模式乘以行数；
 *          宽度保证箭头按钮可显示
 */
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
    return QSize(arrow.width() + 40, h);
}

/**
 * @brief 处理尺寸变化事件
 * @param event 尺寸变化事件
 * @details 将编辑区容器对齐到 QComboBox 的 EditField subcontrol 区域，
 *          保证 DAChipsEdit 覆盖原生编辑区
 */
void DASelectComboBox::resizeEvent(QResizeEvent *event)
{
    QComboBox::resizeEvent(event);
    DA_D(d);
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    const QRect field = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
    d->container->setGeometry(field);
}

/**
 * @brief 弹出/收起下拉候选列表
 * @details 不调用 QComboBox::showPopup，改用自己的 QListWidget 弹出以支持过滤
 */
void DASelectComboBox::showPopup()
{
    // 不调用 QComboBox::showPopup：用自己的 QListWidget 弹出，支持过滤
    DA_D(d);
    if (d->popupVisible)
        hideCustomPopup();
    else
        showCustomPopup();
}

/**
 * @brief 全局事件过滤器
 * @param watched 被监听的对象
 * @param event 事件
 * @return 是否拦截事件
 * @details popup 可见时，外部鼠标按下关闭下拉（点 popup 内部由 itemClicked 处理），
 *          Tab/Backtab 焦点切换也关闭下拉
 */
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

/**
 * @brief 输入框文本变化时刷新候选列表
 * @param text 当前输入文本
 * @details 可过滤时根据文本显示/隐藏弹出框并刷新过滤结果
 */
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

/**
 * @brief 输入框焦点变化处理
 * @param focused 是否获得焦点
 * @details 获焦时若可过滤且有候选选项则弹出下拉；失焦时不主动关闭，
 *          避免与 popup itemClicked 的点击事件竞争导致误关
 */
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

/**
 * @brief 输入框按键事件拦截
 * @param event 按键事件
 * @param handled 输出参数，置 true 阻止 DAChipsEdit 默认处理
 * @details 回车/Enter 选中当前候选项或创建新标签；方向键导航候选列表；
 *          Escape 关闭下拉并清空输入
 */
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

/**
 * @brief 点击弹出列表项时选中该项
 * @param item 被点击的列表项
 * @details 将项文本追加为标签并发射 optionSelected，清空输入框并刷新候选列表
 */
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

/**
 * @brief 应用高度策略，组装编辑区容器布局
 * @details GrowWithTags 模式将 DAChipsEdit 直接放入容器；
 *          FixedHeightScrollable 模式将其包入 QScrollArea 以支持滚动
 */
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

/**
 * @brief 显示自定义候选弹出框
 * @details 刷新候选列表后定位到 combobox 正下方，设置默认选中第一项（default-first-option）
 */
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

/**
 * @brief 隐藏自定义候选弹出框
 */
void DASelectComboBox::hideCustomPopup()
{
    DA_D(d);
    if (!d->popupVisible)
        return;
    d->popup->hide();
    d->popupVisible = false;
}

/**
 * @brief 刷新弹出框候选列表
 * @param filter 过滤文本，空字符串表示显示全部
 * @details 排除已选标签，按过滤文本模糊匹配；无匹配项时自动关闭弹出框
 */
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

/**
 * @brief 选中当前高亮的候选项
 * @details 将当前项文本追加为标签并发射 optionSelected，清空输入框并刷新候选列表
 */
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
