#include "DACollapseWidget.h"
#include "DACollapseItem.h"
#include <QSet>
#include <QVBoxLayout>

namespace DA
{
class DACollapseWidget::PrivateData
{
public:
    PrivateData(DACollapseWidget*)
    {
    }
    QVBoxLayout* layout { nullptr };
    QList< DACollapseItem* > items;
    bool accordion { false };
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
    d->layout = new QVBoxLayout(this);
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
    if (!item) {
        return;
    }
    // 禁用面板不可激活(对标 Element UI disabled),折叠则不受限
    if (on && item->disabled())
        return;
    // 临时阻断信号,避免 setActiveNames 过程中逐个触发 onItemExpandChanged 造成抖动
    item->blockSignals(true);
    item->setExpanded(on);
    item->blockSignals(false);
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
    connect(item, &DACollapseItem::expandChanged, this, [ this ](bool) { onItemExpandChanged(); });
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

/** @brief 推荐尺寸,委托基类实现 */
QSize DACollapseWidget::sizeHint() const
{
    return QWidget::sizeHint();
}
}  // namespace DA
