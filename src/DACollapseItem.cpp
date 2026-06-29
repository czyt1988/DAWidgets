#include "DACollapseItem.h"
#include "DACollapseLayout.h"

#include <QColor>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QStyle>
#include <QSvgRenderer>
#include <QVBoxLayout>

namespace DA
{
namespace
{
/// 箭头逻辑尺寸(像素),与 m_arrow 的固定尺寸保持一致
constexpr int kArrowSize = 14;

/// 折叠态箭头(▶):惰性初始化的全局缓存,整个进程只渲染一次。
/// Disabled 模式交由 QIcon 自动从 Normal 生成灰度版本。
const QIcon& arrowRightIcon()
{
    static QIcon icon(QStringLiteral(":/DAWidgetsIcons/arrow-right.svg"));
    return icon;
}

/// 展开态箭头(▼)
const QIcon& arrowDownIcon()
{
    static const QIcon icon(QStringLiteral(":/DAWidgetsIcons/arrow-down.svg"));
    return icon;
}
}  // namespace

class DACollapseItem::PrivateData
{
public:
    PrivateData(DACollapseItem*)
    {
    }
    QFrame* header { nullptr };
    QLabel* arrow { nullptr };
    QLabel* titleLabel { nullptr };
    QWidget* titleWidget { nullptr };
    QFrame* content { nullptr };
    QVBoxLayout* contentLayout { nullptr };
    QPropertyAnimation* animation { nullptr };

    QString title;
    QString name;
    QColor titleNormalColor;  // 记录正常态标题文本色,用于禁用时还原
    bool disabled { false };
    bool expanded { false };
    bool animationEnabled { true };
};

/**
 * @brief 构造空标题的折叠条目
 * @param parent 父控件
 */
DACollapseItem::DACollapseItem(QWidget* parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    init();
}

/** @brief 析构,由 PIMPL 智能指针自动释放私有数据 */
DACollapseItem::~DACollapseItem() = default;

/**
 * @brief 构造指定标题的折叠条目
 * @param title 标题文本
 * @param parent 父控件
 */
DACollapseItem::DACollapseItem(const QString& title, QWidget* parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->title = title;
    init();
    d->titleLabel->setText(title);
}

/**
 * @brief 初始化标题栏、内容区与动画
 * @details 显式注册 qrc 资源避免静态库被链接器丢弃;标题栏垂直固定提供正确 sizeHint;
 *          内容区初始 maximumHeight=0 折叠;动画绑定 content 的 maximumHeight 属性
 */
void DACollapseItem::init()
{
    DA_D(d);
    Q_INIT_RESOURCE(dawidgetsicons);  // 静态库需显式注册 qrc 资源,否则被链接器丢弃
    setObjectName(QStringLiteral("DACollapseItem"));
    setFrameShape(QFrame::NoFrame);
    installEventFilter(this);

    auto* outer = new DACollapseLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // ---- 标题栏 ----
    d->header = new QFrame(this);
    d->header->setObjectName(QStringLiteral("header"));
    d->header->setFrameShape(QFrame::NoFrame);
    d->header->setCursor(Qt::PointingHandCursor);
    // 垂直方向 Fixed:给 DACollapseLayout 提供稳定的 sizeHint
    d->header->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    d->header->installEventFilter(this);

    auto* headerLay = new QHBoxLayout(d->header);
    headerLay->setContentsMargins(12, 10, 12, 10);
    headerLay->setSpacing(8);

    d->arrow = new QLabel(d->header);
    d->arrow->setObjectName(QStringLiteral("DACollapseItemArrow"));
    d->arrow->setFixedSize(kArrowSize, kArrowSize);
    headerLay->addWidget(d->arrow);

    d->titleLabel = new QLabel(d->title, d->header);
    d->titleLabel->setObjectName(QStringLiteral("DACollapseItemTitle"));
    headerLay->addWidget(d->titleLabel, 1);

    outer->addWidget(d->header);

    // ---- 内容区 ----
    d->content = new QFrame(this);
    d->content->setObjectName(QStringLiteral("DACollapseItemContent"));
    d->content->setFrameShape(QFrame::NoFrame);
    d->content->setMaximumHeight(0);  // 初始折叠
    d->content->hide();
    d->content->installEventFilter(this);  // 监听 LayoutRequest 适配动态内容/宽度变化

    d->contentLayout = new QVBoxLayout(d->content);
    d->contentLayout->setContentsMargins(12, 8, 12, 12);
    d->contentLayout->setSpacing(6);

    outer->addWidget(d->content);

    // ---- 动画 ----
    d->animation = new QPropertyAnimation(d->content, "maximumHeight", this);
    d->animation->setDuration(250);
    d->animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->animation, &QPropertyAnimation::finished, this, &DACollapseItem::onAnimationFinished);

    updateArrow();
    d->titleNormalColor = d->titleLabel->palette().color(QPalette::WindowText);
    updateHeaderVisualState();
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

/** @brief 根据展开态与禁用态刷新箭头图标 */
void DACollapseItem::updateArrow()
{
    DA_D(d);
    const QIcon& icon = d->expanded ? arrowDownIcon() : arrowRightIcon();
    // Disabled 模式:QIcon 未显式 addPixmap(Disabled) 时,Qt 自动从 Normal 生成灰度版本
    const QIcon::Mode mode = d->disabled ? QIcon::Disabled : QIcon::Normal;
    d->arrow->setPixmap(icon.pixmap(QSize(kArrowSize, kArrowSize), mode));
}

/**
 * @brief 刷新标题栏禁用视觉
 * @details qss 的 color 作用在 QFrame 上不会传递给子 QLabel,故在此内置禁用
 *          置灰与光标切换,并维护 disabled 动态属性供 qss 进一步定制
 */
void DACollapseItem::updateHeaderVisualState()
{
    DA_D(d);
    // 内置禁用视觉,不依赖外部 qss(qss 的 color 作用在 QFrame 上不会传给子 QLabel)
    const QColor c = d->disabled ? QColor(QStringLiteral("#c0c4cc")) : d->titleNormalColor;
    QPalette pal   = d->titleLabel->palette();
    pal.setColor(QPalette::Active, QPalette::WindowText, c);
    pal.setColor(QPalette::Inactive, QPalette::WindowText, c);
    d->titleLabel->setPalette(pal);
    // 禁用时光标为"禁止"图标,比箭头更直观
    d->header->setCursor(d->disabled ? Qt::ForbiddenCursor : Qt::PointingHandCursor);
    // 保留 disabled 动态属性供 qss 进一步定制(如 hover 背景抑制)
    setProperty("disabled", d->disabled);
    style()->unpolish(this);
    style()->polish(this);
}

/**
 * @brief 计算内容区在当前宽度下的自然高度
 * @return heightForWidth 结果,退化为 sizeHint
 * @details 用 item 当前宽度调用 heightForWidth,而非 layout->sizeHint().height()
 *          (后者基于 idealWidth,对 wordWrap 内容偏小)。
 *          widget 未显示时 width() 为 0,heightForWidth(0) 因边距为负返回 -1,
 *          此时退化为 sizeHint 作为初始估算,待 Resize 事件获得实际宽度后修正。
 *          outer layout margins=0 且 content 为 NoFrame,故 content 宽度 == item 宽度。
 */
int DACollapseItem::contentNaturalHeight() const
{
    DA_DC(d);
    if (auto* lay = d->content->layout()) {
        const int w = width();
        if (w > 0) {
            const int h = lay->heightForWidth(w);
            if (h > 0)
                return h;
        }
        return lay->sizeHint().height();
    }
    return d->content->sizeHint().height();
}

/**
 * @brief 执行展开/折叠的实际动画或即时切换
 * @param on true 展开, false 折叠
 * @details 通过动画驱动 content 的 maximumHeight 在 0 与自然高度间过渡。
 *          DACollapseLayout 不压缩子项,无需 setMinimumHeight 防护。
 *          折叠时以当前实际高度作为动画起点。
 */
void DACollapseItem::doExpand(bool on)
{
    DA_D(d);
    if (d->animationEnabled) {
        if (on) {
            d->content->setMaximumHeight(0);
            d->content->show();
            const int target = contentNaturalHeight();
            d->animation->stop();
            d->animation->setStartValue(0);
            d->animation->setEndValue(target);
            d->animation->start();
        } else {
            const int start = d->content->height();
            d->content->setMaximumHeight(start);
            d->animation->stop();
            d->animation->setStartValue(start);
            d->animation->setEndValue(0);
            d->animation->start();
        }
    } else {
        // 无动画:直接显示/隐藏
        if (on) {
            d->content->show();
            d->content->setMaximumHeight(contentNaturalHeight());
        } else {
            d->content->setMaximumHeight(0);
            d->content->hide();
        }
    }
    updateArrow();
}

/** @brief 标题栏点击处理,切换展开态(禁用面板忽略) */
void DACollapseItem::onHeaderClicked()
{
    DA_D(d);
    if (d->disabled)
        return;
    setExpanded(!d->expanded);
}

/**
 * @brief 动画结束后收尾
 * @details 展开态:动画已将 maximumHeight 设为目标值,无需额外操作。
 *          折叠态:隐藏 content,避免 maximumHeight=0 的可见残留。
 *          DACollapseLayout 不压缩子项,无需 setMinimumHeight 双向锁定。
 */
void DACollapseItem::onAnimationFinished()
{
    DA_D(d);
    if (!d->expanded) {
        d->content->hide();
    }
}

/**
 * @brief 事件过滤器:转发标题栏点击、适配内容区布局变化与首次显示高度修正
 * @param watched 被监听对象(自身/标题栏/内容区)
 * @param event 事件
 * @details 标题栏 MouseButtonPress 触发展开切换;
 *          内容区 LayoutRequest 在非动画展开态下更新 maximumHeight 适配动态内容;
 *          自身 Resize 在展开态下重新计算高度——修正 widget 首次显示时
 *          width()==0 导致的初始高度估算偏差
 */
bool DACollapseItem::eventFilter(QObject* watched, QEvent* event)
{
    DA_D(d);
    if (watched == d->header && event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast< QMouseEvent* >(event);
        if (me->button() == Qt::LeftButton)
            onHeaderClicked();
        return true;
    }
    if (watched == d->content && event->type() == QEvent::LayoutRequest) {
        // 展开态(非动画中)下内容变化时,更新 maximumHeight 以适应新的自然高度
        if (d->expanded && d->animation->state() != QAbstractAnimation::Running) {
            d->content->setMaximumHeight(contentNaturalHeight());
        }
    }
    if (watched == this && event->type() == QEvent::Resize) {
        // 首次显示时 width() 从 0 变为实际值,重新计算内容高度。
        // 动画进行中则更新 endValue,动画结束后直接修正 maximumHeight。
        if (d->expanded) {
            const int h = contentNaturalHeight();
            if (d->animation->state() == QAbstractAnimation::Running) {
                d->animation->setEndValue(h);
            } else if (d->content->maximumHeight() != h) {
                d->content->setMaximumHeight(h);
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}

/** @brief 获取标题文本 */
QString DACollapseItem::title() const
{
    DA_DC(d);
    return d->title;
}

/**
 * @brief 设置标题文本
 * @param title 新标题文本
 * @details 值未变化时跳过;更新标签后发射 titleChanged
 */
void DACollapseItem::setTitle(const QString& title)
{
    DA_D(d);
    if (d->title == title)
        return;
    d->title = title;
    d->titleLabel->setText(title);
    Q_EMIT titleChanged(title);
}

/** @brief 获取面板标识 */
QString DACollapseItem::name() const
{
    DA_DC(d);
    return d->name;
}

/**
 * @brief 设置面板标识
 * @param name 新面板标识
 * @details 值未变化时跳过;更新后发射 nameChanged
 */
void DACollapseItem::setName(const QString& name)
{
    DA_D(d);
    if (d->name == name)
        return;
    d->name = name;
    Q_EMIT nameChanged(name);
}

/** @brief 是否禁用 */
bool DACollapseItem::disabled() const
{
    DA_DC(d);
    return d->disabled;
}

/**
 * @brief 启用/禁用面板交互
 * @param on true 禁用
 * @details 更新标题置灰、箭头灰度与光标,并维护 qss disabled 属性后发射信号
 */
void DACollapseItem::setDisabled(bool on)
{
    DA_D(d);
    if (d->disabled == on)
        return;
    d->disabled = on;
    updateHeaderVisualState();  // 标题文本置灰 + 光标 + qss 属性
    updateArrow();              // 箭头随 QIcon::Disabled 自动变灰
    Q_EMIT disabledChanged(on);
}

/** @brief 是否处于展开态 */
bool DACollapseItem::expanded() const
{
    DA_DC(d);
    return d->expanded;
}

/**
 * @brief 展开/折叠面板
 * @param on true 展开, false 折叠
 * @details 值未变化时跳过;更新状态后调用 doExpand 执行动画或即时切换,
 *          随后发射 expandChanged 供容器协调互斥
 */
void DACollapseItem::setExpanded(bool on)
{
    DA_D(d);
    if (d->expanded == on)
        return;
    d->expanded = on;
    doExpand(on);
    Q_EMIT expandChanged(on);
}

/** @brief 是否启用展开/折叠动画 */
bool DACollapseItem::animationEnabled() const
{
    DA_DC(d);
    return d->animationEnabled;
}

/**
 * @brief 设置是否启用动画
 * @param on true 启用
 * @details 值未变化时跳过;更新后发射 animationEnabledChanged
 */
void DACollapseItem::setAnimationEnabled(bool on)
{
    DA_D(d);
    if (d->animationEnabled == on)
        return;
    d->animationEnabled = on;
    Q_EMIT animationEnabledChanged(on);
}

/**
 * @brief 设置自定义标题控件
 * @param widget 标题控件,传入 nullptr 还原默认文本标签
 * @details 取代默认标题文本,插入到箭头与标题之间;旧的自定义控件被销毁
 */
void DACollapseItem::setTitleWidget(QWidget* widget)
{
    DA_D(d);
    if (d->titleWidget == widget)
        return;
    if (d->titleWidget) {
        d->titleWidget->setParent(nullptr);
        delete d->titleWidget;
    }
    d->titleWidget = widget;
    if (d->titleWidget) {
        d->titleWidget->setParent(d->header);
        // 标题 widget 取代默认文本标签与箭头之间的位置
        auto* lay = qobject_cast< QHBoxLayout* >(d->header->layout());
        lay->insertWidget(lay->indexOf(d->arrow) + 1, d->titleWidget, 1);
        d->titleLabel->hide();
    } else {
        d->titleLabel->show();
    }
}

/** @brief 获取自定义标题控件,未设置时返回 nullptr */
QWidget* DACollapseItem::titleWidget() const
{
    DA_DC(d);
    return d->titleWidget;
}

/**
 * @brief 设置内容控件
 * @param widget 内容控件,可为 nullptr 仅清空
 * @details 清空旧内容布局项后追加新控件,取代默认内容
 */
void DACollapseItem::setContentWidget(QWidget* widget)
{
    DA_D(d);
    // 清空旧内容
    while (d->contentLayout->count() > 0) {
        QLayoutItem* it = d->contentLayout->takeAt(0);
        if (it->widget())
            delete it->widget();
        delete it;
    }
    if (widget) {
        d->contentLayout->addWidget(widget);
    }
}

/** @brief 获取内容控件,未设置时返回 nullptr */
QWidget* DACollapseItem::contentWidget() const
{
    DA_DC(d);
    if (d->contentLayout->count() > 0) {
        if (auto* it = d->contentLayout->itemAt(0))
            return it->widget();
    }
    return nullptr;
}

/** @brief 获取内容区布局,可直接 addWidget 追加多个控件 */
QVBoxLayout* DACollapseItem::contentLayout() const
{
    DA_DC(d);
    return d->contentLayout;
}

/** @brief 推荐尺寸,委托基类实现 */
QSize DACollapseItem::sizeHint() const
{
    return QFrame::sizeHint();
}
}  // namespace DA
