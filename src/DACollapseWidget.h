#ifndef DACOLLAPSEWIDGET_H
#define DACOLLAPSEWIDGET_H

#include <QStringList>
#include <QWidget>
#include "DAWidgetsGlobals.h"

namespace DA
{
class DACollapseItem;
class DACollapseLayout;

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
class DAWIDGETS_EXPORT DACollapseWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool accordion READ accordion WRITE setAccordion NOTIFY accordionChanged)
    Q_PROPERTY(QStringList activeNames READ activeNames WRITE setActiveNames NOTIFY activeNamesChanged)
    DA_DECLARE_PRIVATE(DACollapseWidget)
public:
    // 构造折叠面板容器
    explicit DACollapseWidget(QWidget* parent = nullptr);
    // 析构,释放内部条目与布局
    ~DACollapseWidget();

    // 是否启用手风琴模式(同时仅一个面板展开)
    bool accordion() const;
    // 启用/禁用手风琴模式
    void setAccordion(bool on);

    // 获取当前激活(展开)面板的 name 列表
    QStringList activeNames() const;
    // 设置激活面板(手风琴模式下仅取首个生效);会同步展开/折叠子项
    void setActiveNames(const QStringList& names);

    // 添加一个已有的条目(接管所有权)
    void addItem(DACollapseItem* item);
    // 便捷创建并添加条目,返回新建实例以便设置内容
    DACollapseItem* addItem(const QString& title, const QString& name = QString());

    // 按 name 查找条目
    DACollapseItem* item(const QString& name) const;
    // 获取全部条目列表
    QList< DACollapseItem* > items() const;

    // 移除并销毁条目
    void removeItem(DACollapseItem* item);
    // 清空并销毁所有条目
    void clear();

    // 推荐尺寸
    QSize sizeHint() const override;

Q_SIGNALS:
    /**
     * @brief 手风琴模式切换时发射
     * @param on 切换后是否启用手风琴模式
     */
    void accordionChanged(bool on);
    /**
     * @brief 激活面板集合变化时发射
     * @param activeNames 当前激活面板的 name 列表
     */
    void activeNamesChanged(const QStringList& activeNames);

private:
    void init();
    void activate(DACollapseItem* item, bool on);
    void onItemExpandChanged();
};
}  // namespace DA

#endif  // DACOLLAPSEWIDGET_H
