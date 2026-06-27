#ifndef DACOLLAPSE_H
#define DACOLLAPSE_H

#include <QStringList>
#include <QWidget>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QVBoxLayout;

namespace DA
{
class DACollapseItem;

/**
 * @brief 折叠面板容器,对标 Element UI el-collapse。
 *
 * 纵向排列多个 DACollapseItem,并协调展开状态:
 *  - 非手风琴模式(默认):多个面板可同时展开,activeNames 为列表。
 *  - 手风琴模式:同时只能展开一个,展开新面板时折叠其余,activeNames 取首个。
 *
 * 对应 Element UI:
 *   value/v-model  -> activeNames() / setActiveNames()
 *   accordion      -> accordion()   / setAccordion()
 *   change(event)  -> activeNamesChanged(QStringList)
 *
 * 用法:
 * ```cpp
 * auto *c = new DA::DACollapse;
 * c->addItem(QStringLiteral("一致性"), QStringLiteral("1"));
 * c->addItem(QStringLiteral("反馈"), QStringLiteral("2"));
 * c->setActiveNames({ QStringLiteral("1") });
 * ```
 */
class DAWIDGETS_EXPORT DACollapse : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool accordion READ accordion WRITE setAccordion NOTIFY accordionChanged)
    Q_PROPERTY(QStringList activeNames READ activeNames WRITE setActiveNames NOTIFY activeNamesChanged)
    DA_DECLARE_PRIVATE(DACollapse)
public:
    explicit DACollapse(QWidget *parent = nullptr);
    ~DACollapse();

    bool accordion() const;
    void setAccordion(bool on);

    QStringList activeNames() const;
    // 设置激活面板(手风琴模式下仅取首个生效);会同步展开/折叠子项
    void setActiveNames(const QStringList &names);

    // 添加一个已有的条目(接管所有权)
    void addItem(DACollapseItem *item);
    // 便捷创建并添加条目,返回新建实例以便设置内容
    DACollapseItem *addItem(const QString &title, const QString &name = QString());

    // 按 name 查找条目
    DACollapseItem *item(const QString &name) const;
    QList<DACollapseItem *> items() const;

    // 移除并销毁条目
    void removeItem(DACollapseItem *item);
    void clear();

    QSize sizeHint() const override;

Q_SIGNALS:
    void accordionChanged(bool on);
    void activeNamesChanged(const QStringList &activeNames);

private:
    void init();
    void activate(DACollapseItem *item, bool on);
    void onItemExpandChanged();
};
}  // namespace DA

#endif // DACOLLAPSE_H
