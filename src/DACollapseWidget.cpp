#include "DACollapseWidget.h"
#include "DACollapseItem.h"
#include "DACollapseLayout.h"
#include <QKeyEvent>
#include <QSet>
#include <QVBoxLayout>

namespace DA
{
namespace
{
/// 变体的 qss 动态属性值
QString variantKey(DACollapseWidget::Variant v)
{
    switch (v) {
    case DACollapseWidget::Variant::Borderless:
        return QStringLiteral("borderless");
    case DACollapseWidget::Variant::Card:
        return QStringLiteral("card");
    case DACollapseWidget::Variant::Default:
        break;
    }
    return QStringLiteral("default");
}
}  // namespace

class DACollapseWidget::PrivateData
{
public:
    PrivateData(DACollapseWidget*)
    {
    }
    DACollapseLayout* layout { nullptr };
    QList< DACollapseItem* > items;
    bool accordion { false };
    bool keyboardNavigation { false };
    Variant variant { Variant::Default };
    // setActiveNames 批量同步期间抑制逐项 activeNamesChanged,但不阻断条目自身信号
    bool activating { false };
};

/**
 * @brief 构造折叠面板容器
 * @param parent 父控件
 * @details 初始化纵向布局并插入末尾弹簧,使条目顶部对齐
 */
DACollapseWidget::DACollapseWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    init();
}

/** @brief 析构,由 PIMPL 智能指针自动释放私有数据 */
DACollapseWidget::~DACollapseWidget() = default;

/** @brief 初始化无间距纵向布局与末尾弹簧 */
void DACollapseWidget::init()
{
    DA_D(d);
    setObjectName(QStringLiteral("DACollapse"));
    d->layout = new DACollapseLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    // 末尾弹簧,使条目顶部对齐
    d->layout->addStretch(1);
}

/**
 * @brief 查询手风琴模式
 * @return true 表示已启用手风琴模式
 */
bool DACollapseWidget::accordion() const
{
    DA_DC(d);
    return d->accordion;
}

/**
 * @brief 设置手风琴模式
 * @param on true 启用, false 禁用
 * @details 启用瞬间若当前多个面板展开,仅保留首个激活面板以维持互斥语义
 */
void DACollapseWidget::setAccordion(bool on)
{
    DA_D(d);
    if (d->accordion == on)
        return;
    d->accordion = on;
    // 切换到手风琴模式时,只保留首个激活面板
    if (on) {
        const QStringList act = activeNames();
        if (act.size() > 1) {
            // 仅保留第一个
            for (int i = 1; i < act.size(); ++i) {
                if (auto* it = item(act[ i ]))
                    activate(it, false);
            }
        }
    }
    Q_EMIT accordionChanged(on);
}

/**
 * @brief 打开/关闭键盘导航
 * @param on 是否开启(默认 false)
 * @details 开启时每个标题变为 StrongFocus;关闭时收回焦点策略并释放焦点
 */
void DACollapseWidget::setKeyboardNavigation(bool on)
{
    DA_D(d);
    if (d->keyboardNavigation == on)
        return;
    d->keyboardNavigation = on;
    for (auto* it : std::as_const(d->items)) {
        QWidget* header = it->headerWidget();
        if (!header)
            continue;
        if (on) {
            header->setFocusPolicy(Qt::StrongFocus);
        } else {
            header->setFocusPolicy(Qt::NoFocus);
            if (header->hasFocus())
                header->clearFocus();
        }
    }
}

/** @brief 键盘导航是否开启 */
bool DACollapseWidget::isKeyboardNavigation() const
{
    DA_DC(d);
    return d->keyboardNavigation;
}

/**
 * @brief 设置外观变体
 * @param variant 变体枚举(默认 Default)
 * @details 只改间距与内留白,不碰 accordion / activeNames
 */
void DACollapseWidget::setVariant(Variant variant)
{
    DA_D(d);
    if (d->variant == variant)
        return;
    d->variant = variant;
    const QString key = variantKey(variant);
    setProperty("daVariant", key);
    d->layout->setSpacing(variant == Variant::Card ? 8 : 0);
    for (auto* it : std::as_const(d->items))
        applyVariantToItem(it);
    d->layout->invalidate();
}

/** @brief 当前外观变体 */
DACollapseWidget::Variant DACollapseWidget::variant() const
{
    DA_DC(d);
    return d->variant;
}

/**
 * @brief 获取当前激活面板的 name 列表
 * @return 遍历所有条目收集展开项的 name
 */
QStringList DACollapseWidget::activeNames() const
{
    DA_DC(d);
    QStringList names;
    for (auto* it : std::as_const(d->items)) {
        if (it->expanded())
            names << it->name();
    }
    return names;
}

/**
 * @brief 设置激活面板集合
 * @param names 需展开面板的 name 列表
 * @details 手风琴模式下仅展开 names 中首个实际存在的面板;非手风琴模式按集合
 *          匹配逐项展开/折叠。完成后发射 activeNamesChanged
 */
void DACollapseWidget::setActiveNames(const QStringList& names)
{
    DA_D(d);
    const QSet< QString > set(names.begin(), names.end());
    if (d->accordion) {
        // 手风琴:仅展开 names 中第一个实际存在的面板,其余折叠
        DACollapseItem* firstMatch = nullptr;
        for (const auto& n : names) {
            firstMatch = item(n);
            if (firstMatch)
                break;
        }
        for (auto* it : std::as_const(d->items)) {
            activate(it, it == firstMatch);
        }
    } else {
        for (auto* it : std::as_const(d->items)) {
            activate(it, set.contains(it->name()));
        }
    }
    Q_EMIT activeNamesChanged(activeNames());
}

/**
 * @brief 单向设置条目展开态
 * @param item 目标条目
 * @param on 是否展开
 * @details 展开禁用面板将被拒绝(对标 Element UI disabled);临时阻断信号避免
 *          setActiveNames 过程中逐个触发 onItemExpandChanged 造成抖动
 */
void DACollapseWidget::activate(DACollapseItem* item, bool on)
{
    DA_D(d);
    if (!item) {
        return;
    }
    // 禁用面板不可激活(对标 Element UI disabled),折叠则不受限
    if (on && item->disabled())
        return;
    // 用标志位抑制 onItemExpandChanged 的逐项发射,而不是 blockSignals():
    // 后者会连 contentRequested 一起吞掉,破坏 setActiveNames 触发的懒加载
    const bool prev = d->activating;
    d->activating  = true;
    item->setExpanded(on);
    d->activating = prev;
}

/**
 * @brief 添加已有条目并接管所有权
 * @param item 待添加条目
 * @details 在末尾弹簧之前插入以保持条目在上,并连接 expandChanged 用于互斥协调
 */
void DACollapseWidget::addItem(DACollapseItem* item)
{
    DA_D(d);
    if (!item)
        return;
    item->setParent(this);
    // 在弹簧之前插入,保持条目在上、弹簧在底
    const int idx = d->layout->count() - 1;  // 弹簧索引
    d->layout->insertWidget(idx, item);
    d->items.append(item);
    // 键盘导航:容器统一监听标题按键
    if (QWidget* header = item->headerWidget()) {
        header->installEventFilter(this);
        if (d->keyboardNavigation)
            header->setFocusPolicy(Qt::StrongFocus);
    }
    connect(item, &DACollapseItem::contentRequested, this,
            [ this, item ] { Q_EMIT expandRequested(item->name()); });
    connect(item, &DACollapseItem::expandChanged, this, [ this ](bool) { onItemExpandChanged(); });
    // 后加入的条目也要吃到当前变体
    applyVariantToItem(item);
}

/**
 * @brief 便捷创建并添加条目
 * @param title 标题文本
 * @param name 面板标识,为空时回退使用 title
 * @return 新建条目实例
 */
DACollapseItem* DACollapseWidget::addItem(const QString& title, const QString& name)
{
    auto* it = new DACollapseItem(title, this);
    it->setName(name.isEmpty() ? title : name);
    addItem(it);
    return it;
}

/**
 * @brief 按 name 查找条目
 * @param name 面板标识
 * @return 匹配条目,未找到返回 nullptr
 */
DACollapseItem* DACollapseWidget::item(const QString& name) const
{
    DA_DC(d);
    for (auto* it : d->items) {
        if (it->name() == name)
            return it;
    }
    return nullptr;
}

/** @brief 获取全部条目列表 */
QList< DACollapseItem* > DACollapseWidget::items() const
{
    DA_DC(d);
    return d->items;
}

/**
 * @brief 移除并销毁条目
 * @param item 待移除条目
 * @details 从布局与列表移除后 delete;移除后发射 activeNamesChanged 同步状态
 */
void DACollapseWidget::removeItem(DACollapseItem* item)
{
    DA_D(d);
    if (!item)
        return;
    const int i = d->items.indexOf(item);
    if (i < 0)
        return;
    if (QWidget* header = item->headerWidget())
        header->removeEventFilter(this);
    d->items.removeAt(i);
    d->layout->removeWidget(item);
    delete item;
    Q_EMIT activeNamesChanged(activeNames());
}

/**
 * @brief 清空并销毁所有条目
 * @details 移除布局中的每个条目并 delete,最后发射空 activeNamesChanged
 */
void DACollapseWidget::clear()
{
    DA_D(d);
    for (auto* it : std::as_const(d->items)) {
        if (QWidget* header = it->headerWidget())
            header->removeEventFilter(this);
        d->layout->removeWidget(it);
        delete it;
    }
    d->items.clear();
    Q_EMIT activeNamesChanged({});
}

/**
 * @brief 子条目展开态变化时的协调槽
 * @details 手风琴模式下新展开面板会折叠其余已展开面板(阻断信号避免递归),
 *          随后发射 activeNamesChanged
 */
void DACollapseWidget::onItemExpandChanged()
{
    DA_D(d);
    if (d->activating)
        return;
    auto* src = qobject_cast< DACollapseItem* >(sender());
    if (!src)
        return;

    if (src->expanded() && d->accordion) {
        // 手风琴:折叠其它所有已展开面板
        for (auto* it : std::as_const(d->items)) {
            if (it != src && it->expanded()) {
                // 阻断信号避免递归
                it->blockSignals(true);
                it->setExpanded(false);
                it->blockSignals(false);
            }
        }
    }
    Q_EMIT activeNamesChanged(activeNames());
}

/**
 * @brief 事件过滤器:标题上的 ↑/↓ 面板间导航
 * @param watched 被监听对象(标题控件)
 * @param event 事件
 * @details 跳过禁用项、到首尾不循环;方向键一律吞掉,避免焦点跑出容器
 */
bool DACollapseWidget::eventFilter(QObject* watched, QEvent* event)
{
    DA_D(d);
    if (!d->keyboardNavigation || event->type() != QEvent::KeyPress || d->items.isEmpty())
        return QWidget::eventFilter(watched, event);

    auto* ke           = static_cast< QKeyEvent* >(event);
    const int key      = ke->key();
    if (key != Qt::Key_Up && key != Qt::Key_Down)
        return QWidget::eventFilter(watched, event);

    DACollapseItem* current = nullptr;
    for (auto* it : std::as_const(d->items)) {
        if (it->headerWidget() == watched) {
            current = it;
            break;
        }
    }
    if (!current)
        return QWidget::eventFilter(watched, event);

    const int dir   = (key == Qt::Key_Down) ? 1 : -1;
    const int start = d->items.indexOf(current);
    for (int i = start + dir; i >= 0 && i < d->items.size(); i += dir) {
        DACollapseItem* candidate = d->items.at(i);
        if (candidate->disabled())
            continue;
        if (QWidget* header = candidate->headerWidget()) {
            header->setFocus(Qt::ShortcutFocusReason);
            break;
        }
    }
    return true;
}

/**
 * @brief 把当前变体下发给单个条目
 * @param item 目标条目
 */
void DACollapseWidget::applyVariantToItem(DACollapseItem* item)
{
    DA_D(d);
    if (!item)
        return;
    item->setProperty("daVariant", variantKey(d->variant));
    item->applyVariantChrome();
}

/** @brief 推荐尺寸,委托基类实现 */
QSize DACollapseWidget::sizeHint() const
{
    return QWidget::sizeHint();
}

/**
 * @brief 在指定索引处插入普通控件
 * @param index 插入位置(相对于布局中所有项,含 DACollapseItem 与弹簧)
 * @param widget 待插入控件,可为任意 QWidget(非 DACollapseItem)
 * @details 直接委托给内部 DACollapseLayout::insertWidget,
 *          控件不进入 items 列表,不参与 activeNames/accordion 协调。
 *          负索引或超出范围时钳制到 [0, count]。
 */
void DACollapseWidget::insertWidget(int index, QWidget* widget)
{
    DA_D(d);
    if (!widget)
        return;
    // 插入到弹簧之前:布局末尾是 stretch,新控件应位于其前
    const int stretchIdx = d->layout->count() - 1;
    const int clamped    = qBound(0, index, stretchIdx);
    d->layout->insertWidget(clamped, widget);
}

/**
 * @brief 末尾(弹簧前)添加普通控件
 * @param widget 待添加控件
 * @details 等价于 insertWidget(count-1, widget),确保控件在末尾弹簧之前。
 */
void DACollapseWidget::addWidget(QWidget* widget)
{
    DA_D(d);
    insertWidget(d->layout->count() - 1, widget);
}

/**
 * @brief 移除普通控件(不销毁)
 * @param widget 待移除控件
 * @details 遍历布局查找匹配的 QWidgetItem 并 takeAt,调用方负责后续所有权。
 *          若 widget 同时是 DACollapseItem,应用 removeItem 以同步 items 列表。
 */
void DACollapseWidget::removeWidget(QWidget* widget)
{
    DA_D(d);
    if (!widget)
        return;
    const int n = d->layout->count();
    for (int i = 0; i < n; ++i) {
        if (QLayoutItem* it = d->layout->itemAt(i)) {
            if (it->widget() == widget) {
                d->layout->takeAt(i);
                return;
            }
        }
    }
}

/**
 * @brief 返回普通控件的索引
 * @param widget 目标控件
 * @return 布局中的索引,不存在返回 -1
 * @details 索引相对于布局中所有项(含 DACollapseItem 与弹簧)。
 */
int DACollapseWidget::indexOf(QWidget* widget) const
{
    DA_DC(d);
    if (!widget)
        return -1;
    const int n = d->layout->count();
    for (int i = 0; i < n; ++i) {
        if (QLayoutItem* it = d->layout->itemAt(i)) {
            if (it->widget() == widget)
                return i;
        }
    }
    return -1;
}
}  // namespace DA
