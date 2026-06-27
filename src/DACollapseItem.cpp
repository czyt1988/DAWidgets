#include "DACollapseItem.h"

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
    static QIcon icon(QStringLiteral(":/DAWidgetsIcons/resources/arrow-right.svg"));
    return icon;
}

/// 展开态箭头(▼)
const QIcon& arrowDownIcon()
{
    static const QIcon icon(QStringLiteral(":/DAWidgetsIcons/resources/arrow-down.svg"));
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

DACollapseItem::DACollapseItem(QWidget* parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    init();
}

DACollapseItem::~DACollapseItem() = default;

DACollapseItem::DACollapseItem(const QString& title, QWidget* parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->title = title;
    init();
    d->titleLabel->setText(title);
}

void DACollapseItem::init()
{
    DA_D(d);
    Q_INIT_RESOURCE(dawidgetsicons);  // 静态库需显式注册 qrc 资源,否则被链接器丢弃
    setObjectName(QStringLiteral("DACollapseItem"));
    setFrameShape(QFrame::NoFrame);

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // ---- 标题栏 ----
    d->header = new QFrame(this);
    d->header->setObjectName(QStringLiteral("header"));
    d->header->setFrameShape(QFrame::NoFrame);
    d->header->setCursor(Qt::PointingHandCursor);
    // 垂直方向固定:展开/折叠内容高度变化时不被布局挤压,避免标题栏跳动
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

void DACollapseItem::updateArrow()
{
    DA_D(d);
    const QIcon& icon = d->expanded ? arrowDownIcon() : arrowRightIcon();
    // Disabled 模式:QIcon 未显式 addPixmap(Disabled) 时,Qt 自动从 Normal 生成灰度版本
    const QIcon::Mode mode = d->disabled ? QIcon::Disabled : QIcon::Normal;
    d->arrow->setPixmap(icon.pixmap(QSize(kArrowSize, kArrowSize), mode));
}

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

int DACollapseItem::contentNaturalHeight() const
{
    DA_DC(d);
    // 直接取布局 sizeHint,不修改 widget 状态,避免触发 QLayoutRequest 导致跳动
    if (auto* lay = d->content->layout())
        return lay->sizeHint().height();
    return d->content->sizeHint().height();
}

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
            const int start = d->content->maximumHeight();
            d->animation->stop();
            d->animation->setStartValue(start > 0 ? start : contentNaturalHeight());
            d->animation->setEndValue(0);
            d->animation->start();
        }
    } else {
        // 无动画:直接显示/隐藏
        if (on) {
            d->content->setMaximumHeight(QWIDGETSIZE_MAX);
            d->content->show();
        } else {
            d->content->setMaximumHeight(0);
            d->content->hide();
        }
    }
    updateArrow();
}

void DACollapseItem::onHeaderClicked()
{
    DA_D(d);
    if (d->disabled)
        return;
    setExpanded(!d->expanded);
}

void DACollapseItem::onAnimationFinished()
{
    DA_D(d);
    if (d->expanded) {
        // 展开完成:解除高度上限,内容按自然高度显示(支持后续动态添加内容)
        d->content->setMaximumHeight(QWIDGETSIZE_MAX);
    } else {
        d->content->hide();
    }
}

bool DACollapseItem::eventFilter(QObject* watched, QEvent* event)
{
    DA_D(d);
    if (watched == d->header && event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast< QMouseEvent* >(event);
        if (me->button() == Qt::LeftButton)
            onHeaderClicked();
        return true;
    }
    return QFrame::eventFilter(watched, event);
}

QString DACollapseItem::title() const
{
    DA_DC(d);
    return d->title;
}

void DACollapseItem::setTitle(const QString& title)
{
    DA_D(d);
    if (d->title == title)
        return;
    d->title = title;
    d->titleLabel->setText(title);
    Q_EMIT titleChanged(title);
}

QString DACollapseItem::name() const
{
    DA_DC(d);
    return d->name;
}

void DACollapseItem::setName(const QString& name)
{
    DA_D(d);
    if (d->name == name)
        return;
    d->name = name;
    Q_EMIT nameChanged(name);
}

bool DACollapseItem::disabled() const
{
    DA_DC(d);
    return d->disabled;
}

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

bool DACollapseItem::expanded() const
{
    DA_DC(d);
    return d->expanded;
}

void DACollapseItem::setExpanded(bool on)
{
    DA_D(d);
    if (d->expanded == on)
        return;
    d->expanded = on;
    doExpand(on);
    Q_EMIT expandChanged(on);
}

bool DACollapseItem::animationEnabled() const
{
    DA_DC(d);
    return d->animationEnabled;
}

void DACollapseItem::setAnimationEnabled(bool on)
{
    DA_D(d);
    if (d->animationEnabled == on)
        return;
    d->animationEnabled = on;
    Q_EMIT animationEnabledChanged(on);
}

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

QWidget* DACollapseItem::titleWidget() const
{
    DA_DC(d);
    return d->titleWidget;
}

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

QWidget* DACollapseItem::contentWidget() const
{
    DA_DC(d);
    if (d->contentLayout->count() > 0) {
        if (auto* it = d->contentLayout->itemAt(0))
            return it->widget();
    }
    return nullptr;
}

QVBoxLayout* DACollapseItem::contentLayout() const
{
    DA_DC(d);
    return d->contentLayout;
}

QSize DACollapseItem::sizeHint() const
{
    return QFrame::sizeHint();
}
}  // namespace DA
