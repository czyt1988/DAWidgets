#include "DASelectComboBox.h"

#include "DAChip.h"
#include "DAChipsEdit.h"

#include <QApplication>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

namespace DA
{
namespace {
constexpr int kGroupRole = Qt::UserRole + 1;

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

// 空态面板：叠在弹层 viewport 上的覆盖控件（不插占位 item，popup->clear() 不会波及）
class EmptyPanel : public QWidget
{
public:
    explicit EmptyPanel(QWidget *parent = nullptr) : QWidget(parent)
    {
        setObjectName(QStringLiteral("DASelectEmptyPanel"));
        auto *lay = new QVBoxLayout(this);
        lay->setContentsMargins(8, 8, 8, 8);
        lay->setSpacing(8);
        lay->addStretch(1);

        m_text = new QLabel(this);
        m_text->setObjectName(QStringLiteral("DASelectEmptyText"));
        m_text->setAlignment(Qt::AlignCenter);
        m_text->setWordWrap(true);
        lay->addWidget(m_text);

        m_create = new QPushButton(this);
        m_create->setObjectName(QStringLiteral("DASelectCreateButton"));
        m_create->setCursor(Qt::PointingHandCursor);
        m_create->hide();
        lay->addWidget(m_create, 0, Qt::AlignHCenter);
        lay->addStretch(1);

        setStyleSheet(QStringLiteral(
            "QLabel#DASelectEmptyText { color:#909399; }"
            "QPushButton#DASelectCreateButton { color:#409eff; border:1px solid #d9ecff;"
            " border-radius:4px; padding:2px 10px; background:#ecf5ff; }"
            "QPushButton#DASelectCreateButton:hover { background:#d9ecff; }"));
    }

    void setText(const QString &text) { m_text->setText(text); }
    void setCreateText(const QString &text) { m_create->setText(text); }
    void setCreateVisible(bool on) { m_create->setVisible(on); }
    QPushButton *createButton() const { return m_create; }

private:
    QLabel *m_text { nullptr };
    QPushButton *m_create { nullptr };
};

QPixmap makeClearPixmap(const QColor &color)
{
    QPixmap pm(12, 12);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(3.5, 3.5), QPointF(8.5, 8.5));
    p.drawLine(QPointF(8.5, 3.5), QPointF(3.5, 8.5));
    p.end();
    return pm;
}

bool isGroupHeader(const QListWidgetItem *item)
{
    return item && item->data(kGroupRole).isValid();
}bool isActionable(const QListWidgetItem *item)
{
    return item && (item->flags() & Qt::ItemIsSelectable);
}

int nextActionableRow(QListWidget *popup, int from, int step)
{
    if (!popup || step == 0)
        return -1;
    int i = from;
    if (i < 0) {
        if (step < 0)
            return -1;
        i = -1;
    }
    for (i += step; i >= 0 && i < popup->count(); i += step) {
        if (isActionable(popup->item(i)))
            return i;
    }
    return -1;
}

int firstActionableRow(QListWidget *popup)
{
    if (!popup)
        return -1;
    for (int i = 0; i < popup->count(); ++i) {
        if (isActionable(popup->item(i)))
            return i;
    }
    return -1;
}
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
    QVector< DASelectGroup > groups;
    bool allowCreate { true };
    bool filterable { true };
    HeightMode heightMode { HeightMode::GrowWithTags };
    int fixedRowCount { 1 };
    bool popupVisible { false };

    bool emptyStateEnabled { false };
    QString noMatchText;
    EmptyPanel *emptyPanel { nullptr };
    QPointer< QWidget > emptyWidget;

    bool clearable { false };
    bool collapseTags { false };
    int collapseTagCount { 2 };
    QToolButton *clearButton { nullptr };
    QLabel *collapseLabel { nullptr };
    bool hovered { false };
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
    ensureResourcesInitialized();  // 静态库需显式注册 qrc 资源,否则被链接器丢弃
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

    // 空态面板是弹层 viewport 上的覆盖控件:popup->clear() 不会销毁它
    d->emptyPanel = new EmptyPanel(d->popup);
    d->emptyPanel->hide();
    d->noMatchText = tr("No matching items");  //cn:没有匹配项
    connect(d->emptyPanel->createButton(), &QPushButton::clicked, this, [this] { activateByInput(); });

    connect(d->chips, &DAChipsEdit::inputTextChanged, this, &DASelectComboBox::onInputTextChanged);
    connect(d->chips, &DAChipsEdit::inputFocusChanged, this, &DASelectComboBox::onInputFocusChanged);
    connect(d->chips, &DAChipsEdit::inputKeyEvent, this, &DASelectComboBox::onInputKey);
    connect(d->chips, &DAChipsEdit::tagsChanged, this, [this](const QStringList &) {
        DA_D(d);
        if (d->heightMode == HeightMode::GrowWithTags)
            updateGeometry();
    });
    connect(d->chips, &DAChipsEdit::tagsChanged, this, &DASelectComboBox::tagsChanged);
    connect(d->chips, &DAChipsEdit::tagsChanged, this, [this] { updateEditOverlays(); });
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
 * @details 弹层是无父顶层窗口，必须显式删除；PIMPL 指针由宏自动释放
 */
DASelectComboBox::~DASelectComboBox()
{
    DA_D(d);
    // 先断开子控件的信号与全局过滤器：~QWidget 删除子控件时会触发焦点变化，
    // 此时本对象的 d_ptr 已释放，槽里再访问私有数据就是 use-after-free
    qApp->removeEventFilter(this);
    if (d->chips)
        d->chips->disconnect(this);
    if (d->popup) {
        d->popup->hide();
        delete d->popup;
        d->popup = nullptr;
    }
}

/**
 * @brief 应用默认现代化样式
 * @details qss 直接引用 qrc 中的 SVG 箭头资源（:/DAWidgetsIcons/arrow-down.svg）。
 *          静态库通过 ensureResourcesInitialized() 显式注册资源,确保被链接器保留。
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
    d->groups.clear();
    d->options = options;
    if (d->popupVisible)
        refreshPopup(d->chips->inputText());
}

/**
 * @brief 设置分组候选
 * @param groups 分组列表，title 为空串表示无标题组
 * @details 同时把组内候选按「组顺序 + 组内顺序」扁平化到 options()，弹层可见时立即刷新
 */
void DASelectComboBox::setGroupedOptions(const QVector< DASelectGroup > &groups)
{
    DA_D(d);
    d->groups = groups;
    d->options.clear();
    for (const DASelectGroup &g : groups)
        d->options.append(g.options);
    if (d->popupVisible)
        refreshPopup(d->chips->inputText());
}

/**
 * @brief 获取分组候选
 * @return 分组列表副本
 */
QVector< DASelectGroup > DASelectComboBox::groupedOptions() const
{
    DA_DC(d);
    return d->groups;
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
 * @brief 清空所有已选标签与输入文本
 * @details 不修改候选选项；清空后关闭弹层（保持旧行为）
 */
void DASelectComboBox::clear()
{
    DA_D(d);
    d->chips->clear();
    d->chips->clearInput();
    hideCustomPopup();
    updateEditOverlays();
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
 * @brief 打开/关闭空态面板
 * @param on 是否启用（默认 false）
 * @details 关闭时保持旧行为：无匹配立即关闭弹层
 */
void DASelectComboBox::setEmptyStateEnabled(bool on)
{
    DA_D(d);
    if (d->emptyStateEnabled == on)
        return;
    d->emptyStateEnabled = on;
    if (d->popupVisible) {
        refreshPopup(d->chips->inputText());
        if (!on && firstActionableRow(d->popup) < 0)
            hideCustomPopup();
    }
    updateEmptyOverlay();
}

/** @brief 空态面板是否启用 */
bool DASelectComboBox::isEmptyStateEnabled() const
{
    DA_DC(d);
    return d->emptyStateEnabled;
}

/**
 * @brief 设置空态主文案
 * @param text 文案
 */
void DASelectComboBox::setNoMatchText(const QString &text)
{
    DA_D(d);
    d->noMatchText = text;
    updateEmptyOverlay();
}

/** @brief 获取空态主文案 */
QString DASelectComboBox::noMatchText() const
{
    DA_DC(d);
    return d->noMatchText;
}

/**
 * @brief 设置自定义空态控件
 * @param w 控件（接管所有权，随弹层析构）；nullptr 还原内置面板
 */
void DASelectComboBox::setEmptyWidget(QWidget *w)
{
    DA_D(d);
    if (d->emptyWidget == w)
        return;
    if (d->emptyWidget) {
        d->emptyWidget->hide();
        d->emptyWidget->deleteLater();
    }
    d->emptyWidget = w;
    if (w) {
        w->setParent(d->popup);
        w->hide();
    }
    updateEmptyOverlay();
}

/** @brief 获取自定义空态控件 */
QWidget *DASelectComboBox::emptyWidget() const
{
    DA_DC(d);
    return d->emptyWidget.data();
}

/**
 * @brief 打开/关闭编辑区清空按钮
 * @param on 是否启用（默认 false）
 * @details 关闭时销毁按钮子控件，保持默认视觉与旧行为一致
 */
void DASelectComboBox::setClearable(bool on)
{
    DA_D(d);
    if (d->clearable == on)
        return;
    d->clearable = on;
    if (on && !d->clearButton) {
        d->clearButton = new QToolButton(this);
        d->clearButton->setObjectName(QStringLiteral("DASelectClearButton"));
        // 用绘制的图标而不是文本 ×：不依赖平台字体，跨平台像素断言才稳定
        QIcon icon;
        icon.addPixmap(makeClearPixmap(QColor(0xC0, 0xC4, 0xCC)), QIcon::Normal);
        icon.addPixmap(makeClearPixmap(QColor(0x90, 0x93, 0x99)), QIcon::Active);
        d->clearButton->setIcon(icon);
        d->clearButton->setIconSize(QSize(10, 10));
        d->clearButton->setAccessibleName(tr("Clear all"));  //cn:清空全部
        d->clearButton->setAutoRaise(true);
        d->clearButton->setFocusPolicy(Qt::NoFocus);
        d->clearButton->setCursor(Qt::ArrowCursor);
        d->clearButton->setFixedSize(20, 20);
        d->clearButton->setStyleSheet(QStringLiteral(
            "QToolButton#DASelectClearButton { border:none; background:transparent; padding:0; }"));
        d->clearButton->hide();
        connect(d->clearButton, &QToolButton::clicked, this, [this] {
            DA_D(d);
            d->chips->clear();
            d->chips->clearInput();
            if (d->popupVisible)
                refreshPopup(QString());
            updateEditOverlays();
            Q_EMIT clearClicked();
        });
    }
    if (!on && d->clearButton) {
        d->clearButton->hide();
        d->clearButton->deleteLater();
        d->clearButton = nullptr;
    }
    updateEditOverlays();
}

/** @brief 是否启用清空按钮 */
bool DASelectComboBox::isClearable() const
{
    DA_DC(d);
    return d->clearable;
}

/**
 * @brief 打开/关闭已选标签折叠
 * @param on 是否启用（默认 false）
 */
void DASelectComboBox::setCollapseTags(bool on)
{
    DA_D(d);
    if (d->collapseTags == on)
        return;
    d->collapseTags = on;
    if (on && !d->collapseLabel) {
        d->collapseLabel = new QLabel(this);
        d->collapseLabel->setObjectName(QStringLiteral("DASelectTagSummary"));
        d->collapseLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        d->collapseLabel->setStyleSheet(QStringLiteral(
            "QLabel#DASelectTagSummary { color:#909399; background:#f4f4f5; border-radius:8px;"
            " padding:1px 6px; }"));
        d->collapseLabel->hide();
    }
    if (!on && d->collapseLabel) {
        d->collapseLabel->hide();
        d->collapseLabel->deleteLater();
        d->collapseLabel = nullptr;
    }
    updateEditOverlays();
}

/** @brief 是否启用已选标签折叠 */
bool DASelectComboBox::collapseTags() const
{
    DA_DC(d);
    return d->collapseTags;
}

/**
 * @brief 设置折叠后保留的标签数
 * @param count 保留数，小于 1 时按 1 处理
 */
void DASelectComboBox::setCollapseTagCount(int count)
{
    DA_D(d);
    d->collapseTagCount = qMax(1, count);
    updateEditOverlays();
}

/** @brief 折叠后保留的标签数 */
int DASelectComboBox::collapseTagCount() const
{
    DA_DC(d);
    return d->collapseTagCount;
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
    updateOverlayGeometry();
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
 * @details popup 可见时，外部鼠标按下关闭下拉（点 popup 内部与自身覆盖层除外），
 *          Tab/Backtab 焦点切换也关闭下拉；Enter/Leave 维护清空按钮的悬停显隐
 */
bool DASelectComboBox::eventFilter(QObject *watched, QEvent *event)
{
    DA_D(d);
    if (event->type() == QEvent::Enter) {
        if (watched == this || (d->clearButton && watched == d->clearButton)) {
            d->hovered = true;
            updateEditOverlays();
        }
    } else if (event->type() == QEvent::Leave) {
        if (watched == this || (d->clearButton && watched == d->clearButton)) {
            d->hovered = false;
            updateEditOverlays();
        }
    }
    if (d->popupVisible) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            // 任何鼠标按下都关闭，除非点在 popup 内部（由 itemClicked 处理选中）。
            // × 与 +N 是本控件的覆盖层，点它们时不能把弹层关掉（否则“清空后保持打开”做不到）。
            auto *w = qobject_cast< QWidget * >(watched);
            const bool ownOverlay = (w && (w == d->clearButton || w == d->collapseLabel));
            if (w && w != d->popup && !d->popup->isAncestorOf(w) && !ownOverlay)
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
    // 控件析构时子控件删除会引发焦点变化，此时 d_ptr 可能已释放
    if (!d_func())
        return;
    DA_D(d);
    if (focused) {
        if (d->filterable && !d->options.isEmpty())
            showCustomPopup();
    }
    // 失焦时不主动关闭：Qt::Popup 在外部点击时自动关闭，选中/Esc 会显式关闭，
    // 避免与 popup itemClicked 的点击事件竞争导致误关。
    updateEditOverlays();
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
        QListWidgetItem *current = d->popup->currentItem();
        if (d->popupVisible && current && isActionable(current)) {
            pickCurrent();
            *handled = true;
        } else if (d->allowCreate && !d->chips->inputText().trimmed().isEmpty()) {
            activateByInput();
            *handled = true;
        }
        return;
    }
    if (!d->popupVisible)
        return;
    if (key == Qt::Key_Down || key == Qt::Key_Up) {
        const int step = (key == Qt::Key_Down) ? 1 : -1;
        const int row  = nextActionableRow(d->popup, d->popup->currentRow(), step);
        if (row >= 0)
            d->popup->setCurrentRow(row);
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
 * @details 组标题不可选；候选追加为标签并发射 optionSelected
 */
void DASelectComboBox::onPopupItemClicked(QListWidgetItem *item)
{
    DA_D(d);
    if (!isActionable(item))
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
 * @details 刷新候选列表后定位到 combobox 正下方；default-first-option 取首个可选行；
 *          空态开启时即使没有候选也保持弹层并显示覆盖面板
 */
void DASelectComboBox::showCustomPopup()
{
    DA_D(d);
    if (d->popupVisible)
        return;
    refreshPopup(d->chips->inputText());
    if (d->popup->count() == 0 && !d->emptyStateEnabled)
        return;
    d->popup->setFixedWidth(width());
    int h = 0;
    if (d->popup->count() == 0) {
        // 空态：按面板/自定义控件的高度给一个不低于最小高度的弹层
        QWidget *w = d->emptyWidget ? d->emptyWidget.data() : d->emptyPanel;
        h = qMax(w ? w->sizeHint().height() + 6 : 0, 40);
    } else {
        int contentH = 0;
        for (int i = 0; i < d->popup->count(); ++i)
            contentH += d->popup->sizeHintForRow(i);
        h = qMin(contentH + 6, 220);
    }
    d->popup->setFixedHeight(qMax(h, 40));
    d->popup->move(mapToGlobal(QPoint(0, height())));
    d->popup->show();
    d->popup->raise();
    const int first = firstActionableRow(d->popup);
    if (first >= 0)
        d->popup->setCurrentRow(first);  // default-first-option
    d->popupVisible = true;
    updateEmptyOverlay();
    updateEditOverlays();
}

/** @brief 隐藏自定义候选弹出框并收起空态面板 */
void DASelectComboBox::hideCustomPopup()
{
    DA_D(d);
    if (!d->popupVisible)
        return;
    d->popup->hide();
    d->popupVisible = false;
    updateEmptyOverlay();
    updateEditOverlays();
}

/**
 * @brief 刷新弹出框候选列表
 * @param filter 过滤文本，空字符串表示显示全部
 * @details 无分组时保持原扁平路径；有分组时插入不可选组标题，
 *          组内无命中则整组隐藏。无匹配且未开空态时关闭弹层。
 */
void DASelectComboBox::refreshPopup(const QString &filter)
{
    DA_D(d);
    d->popup->clear();
    const QStringList selected = d->chips->tags();
    if (d->groups.isEmpty()) {
        // 默认路径：与加入分组前逐项一致
        for (const QString &opt : d->options) {
            if (selected.contains(opt))
                continue;
            if (filter.isEmpty() || opt.contains(filter, Qt::CaseInsensitive))
                d->popup->addItem(opt);
        }
    } else {
        for (int gi = 0; gi < d->groups.size(); ++gi) {
            const DASelectGroup &group = d->groups.at(gi);
            QStringList hits;
            for (const QString &opt : group.options) {
                if (selected.contains(opt))
                    continue;
                if (filter.isEmpty() || opt.contains(filter, Qt::CaseInsensitive))
                    hits << opt;
            }
            if (hits.isEmpty())
                continue;
            if (!group.title.isEmpty()) {
                auto *header = new QListWidgetItem(group.title, d->popup);
                header->setFlags(header->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
                header->setData(kGroupRole, gi);
                header->setSizeHint(QSize(0, 22));
            }
            for (const QString &opt : hits)
                d->popup->addItem(opt);
        }
    }

    const int first = firstActionableRow(d->popup);
    if (first < 0) {
        if (d->popupVisible && !d->emptyStateEnabled) {
            hideCustomPopup();
            return;
        }
    } else {
        d->popup->setCurrentRow(first);
    }
    updateEmptyOverlay();
}

/**
 * @brief 选中当前高亮的候选项
 * @details 组标题不可选；候选追加为标签并发射 optionSelected
 */
void DASelectComboBox::pickCurrent()
{
    DA_D(d);
    QListWidgetItem *item = d->popup->currentItem();
    if (!isActionable(item))
        return;
    d->chips->appendTag(item->text());
    Q_EMIT optionSelected(item->text());
    d->chips->clearInput();
    if (d->popupVisible)
        refreshPopup(QString());
}

/** @brief 回车与空态「创建」按钮共用的创建入口 */
void DASelectComboBox::activateByInput()
{
    DA_D(d);
    const QString text = d->chips->inputText().trimmed();
    if (text.isEmpty())
        return;
    d->chips->appendTag(text);
    Q_EMIT created(text);
    d->chips->clearInput();
    if (d->popupVisible)
        refreshPopup(QString());
}

/**
 * @brief 刷新空态覆盖层
 * @details 只在「开关打开 + 弹层可见 + 没有任何可选候选行」时显示；
 *          面板不是 item，popup->clear() 不会销毁它
 */
void DASelectComboBox::updateEmptyOverlay()
{
    DA_D(d);
    if (!d->emptyPanel)
        return;
    const bool show = d->emptyStateEnabled && d->popupVisible && firstActionableRow(d->popup) < 0;
    if (!show) {
        d->emptyPanel->hide();
        if (d->emptyWidget)
            d->emptyWidget->hide();
        return;
    }
    QWidget *w = d->emptyWidget ? d->emptyWidget.data() : d->emptyPanel;
    if (w == d->emptyPanel) {
        d->emptyPanel->setText(d->noMatchText);
        const QString input  = d->chips->inputText().trimmed();
        const bool canCreate = d->allowCreate && !input.isEmpty();
        d->emptyPanel->setCreateVisible(canCreate);
        if (canCreate)
            d->emptyPanel->setCreateText(tr("Create \"%1\"").arg(input));  //cn:创建“%1”
    }
    const QRect viewportRect = d->popup->viewport()->geometry();
    w->setGeometry(viewportRect);
    w->show();
    w->raise();
    if (w != d->emptyPanel)
        d->emptyPanel->hide();
}

/**
 * @brief 刷新 × 与 +N 的几何
 * @details 唯一度量来源是 style()->subControlRect(SC_ComboBoxEditField)
 */
void DASelectComboBox::updateOverlayGeometry()
{
    if (!d_func())
        return;
    DA_D(d);
    if (!d->container)
        return;
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    const QRect field = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);

    QRect containerRect = field;
    if (d->clearable && !tags().isEmpty())
        containerRect.adjust(0, 0, -24, 0);
    d->container->setGeometry(containerRect);

    const int rowH = d->chips->rowHeightHint();
    if (d->clearButton) {
        const int b = 20;
        const int y = field.top() + qMax(0, (rowH - b) / 2);
        d->clearButton->setGeometry(field.right() - b + 1, y, b, b);
    }
    if (d->collapseLabel) {
        const QSize hint = d->collapseLabel->sizeHint();
        int right        = field.right();
        if (d->clearButton && d->clearButton->isVisible())
            right = d->clearButton->geometry().left() - 4;
        const int y = field.top() + qMax(0, (rowH - hint.height()) / 2);
        d->collapseLabel->setGeometry(right - hint.width(), y, hint.width(), hint.height());
    }
}

/**
 * @brief 按折叠状态隐藏/显示 chip 与 +N 摘要
 * @details 只切可见性，不改写 tags()；聚焦或弹层展开时恢复全部
 */
void DASelectComboBox::updateTagCollapse()
{
    DA_D(d);
    QList< DAChip * > chips;
    if (auto *lay = d->chips->layout()) {
        for (int i = 0; i < lay->count(); ++i) {
            if (auto *chip = qobject_cast< DAChip * >(lay->itemAt(i)->widget()))
                chips << chip;
        }
    }
    const int n     = d->chips->tags().size();
    const int keep  = qMax(1, d->collapseTagCount);
    const bool collapse = d->collapseTags && n > keep;
    QWidget *focus      = QApplication::focusWidget();
    const bool focused  = d->chips->hasFocus() || (focus && d->chips->isAncestorOf(focus));
    const bool showAll  = !collapse || focused || d->popupVisible;
    for (int i = 0; i < chips.size(); ++i)
        chips[i]->setVisible(showAll || i < keep);
    if (d->collapseLabel) {
        d->collapseLabel->setText(QStringLiteral("+%1").arg(n - keep));
        d->collapseLabel->setVisible(collapse && !showAll);
    }
    if (auto *lay = d->chips->layout())
        lay->invalidate();
    d->chips->updateGeometry();
    updateGeometry();
}

/** @brief 综合刷新悬停态、× 与折叠摘要 */
void DASelectComboBox::updateEditOverlays()
{
    if (!d_func())
        return;
    DA_D(d);
    if (d->clearButton) {
        const bool show = d->clearable && d->hovered && !tags().isEmpty() && isEnabled();
        d->clearButton->setVisible(show);
    }
    updateTagCollapse();
    updateOverlayGeometry();
}
}  // namespace DA
