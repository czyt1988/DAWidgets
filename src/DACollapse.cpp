#include "DACollapse.h"

#include "DACollapseItem.h"

#include <QSet>
#include <QVBoxLayout>

namespace DA
{
class DACollapse::PrivateData
{
public:
    PrivateData(DACollapse *) {}
    QVBoxLayout *layout { nullptr };
    QList<DACollapseItem *> items;
    bool accordion { false };
};

DACollapse::DACollapse(QWidget *parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    init();
}

DACollapse::~DACollapse() = default;

void DACollapse::init()
{
    DA_D(d);
    setObjectName(QStringLiteral("DACollapse"));
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    // 末尾弹簧,使条目顶部对齐
    d->layout->addStretch(1);
}

bool DACollapse::accordion() const
{
    DA_DC(d);
    return d->accordion;
}

void DACollapse::setAccordion(bool on)
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
                if (auto *it = item(act[ i ]))
                    activate(it, false);
            }
        }
    }
    Q_EMIT accordionChanged(on);
}

QStringList DACollapse::activeNames() const
{
    DA_DC(d);
    QStringList names;
    for (auto *it : d->items) {
        if (it->expanded())
            names << it->name();
    }
    return names;
}

void DACollapse::setActiveNames(const QStringList &names)
{
    DA_D(d);
    const QSet< QString > set(names.begin(), names.end());
    if (d->accordion) {
        // 手风琴:仅展开 names 中第一个实际存在的面板,其余折叠
        DACollapseItem *firstMatch = nullptr;
        for (const auto &n : names) {
            firstMatch = item(n);
            if (firstMatch)
                break;
        }
        for (auto *it : d->items)
            activate(it, it == firstMatch);
    } else {
        for (auto *it : d->items)
            activate(it, set.contains(it->name()));
    }
    Q_EMIT activeNamesChanged(activeNames());
}

void DACollapse::activate(DACollapseItem *item, bool on)
{
    // 禁用面板不可激活(对标 Element UI disabled),折叠则不受限
    if (on && item->disabled())
        return;
    // 临时阻断信号,避免 setActiveNames 过程中逐个触发 onItemExpandChanged 造成抖动
    item->blockSignals(true);
    item->setExpanded(on);
    item->blockSignals(false);
}

void DACollapse::addItem(DACollapseItem *item)
{
    DA_D(d);
    if (!item)
        return;
    item->setParent(this);
    // 在弹簧之前插入,保持条目在上、弹簧在底
    const int idx = d->layout->count() - 1;  // 弹簧索引
    d->layout->insertWidget(idx, item);
    d->items.append(item);
    connect(item, &DACollapseItem::expandChanged, this, [this](bool) { onItemExpandChanged(); });
}

DACollapseItem *DACollapse::addItem(const QString &title, const QString &name)
{
    auto *it = new DACollapseItem(title, this);
    it->setName(name.isEmpty() ? title : name);
    addItem(it);
    return it;
}

DACollapseItem *DACollapse::item(const QString &name) const
{
    DA_DC(d);
    for (auto *it : d->items) {
        if (it->name() == name)
            return it;
    }
    return nullptr;
}

QList<DACollapseItem *> DACollapse::items() const
{
    DA_DC(d);
    return d->items;
}

void DACollapse::removeItem(DACollapseItem *item)
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

void DACollapse::clear()
{
    DA_D(d);
    for (auto *it : d->items) {
        d->layout->removeWidget(it);
        delete it;
    }
    d->items.clear();
    Q_EMIT activeNamesChanged({});
}

void DACollapse::onItemExpandChanged()
{
    DA_D(d);
    auto *src = qobject_cast< DACollapseItem * >(sender());
    if (!src)
        return;

    if (src->expanded() && d->accordion) {
        // 手风琴:折叠其它所有已展开面板
        for (auto *it : d->items) {
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

QSize DACollapse::sizeHint() const
{
    return QWidget::sizeHint();
}
}  // namespace DA
