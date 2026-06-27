#ifndef DACOLLAPSEITEM_H
#define DACOLLAPSEITEM_H

#include <QFrame>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QLabel;
class QPropertyAnimation;
class QVBoxLayout;
class QColor;

namespace DA
{
/**
 * @brief 折叠面板的单个条目,对标 Element UI el-collapse-item。
 *
 * 由标题栏(header)与内容区(content)两部分构成。点击标题栏在展开/折叠
 * 之间切换,展开时内容区从 0 高度动画过渡到自然高度(可通过 setAnimation 关闭)。
 *
 * 通常不单独使用,而是 add 进 DACollapse 容器由其协调互斥;但也可独立使用,
 * 此时 setExpanded 直接控制自身展开状态。
 *
 * qss 定制示例:
 * ```css
 * DA--DACollapseItem { border:1px solid #ebeef5; border-radius:4px; }
 * DA--DACollapseItem::header { background:#fafafa; }
 * DA--DACollapseItem::header:hover { background:#f5f7fa; }
 * QLabel#DACollapseItemArrow { color:#909399; }
 * QLabel#DACollapseItemTitle { color:#303133; font-weight:bold; }
 * DA--DACollapseItem[disabled="true"]::header { color:#c0c4cc; }
 * ```
 *
 * 对应 Element UI:
 *   name      -> name()      / setName()
 *   title     -> title()     / setTitle()        (title slot -> setTitleWidget)
 *   disabled  -> disabled()  / setDisabled()
 */
class DAWIDGETS_EXPORT DACollapseItem : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool disabled READ disabled WRITE setDisabled NOTIFY disabledChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandChanged)
    Q_PROPERTY(bool animationEnabled READ animationEnabled WRITE setAnimationEnabled NOTIFY animationEnabledChanged)
    DA_DECLARE_PRIVATE(DACollapseItem)
public:
    explicit DACollapseItem(QWidget *parent = nullptr);
    explicit DACollapseItem(const QString &title, QWidget *parent = nullptr);
    ~DACollapseItem();

    QString title() const;
    void setTitle(const QString &title);

    QString name() const;
    void setName(const QString &name);

    bool disabled() const;
    void setDisabled(bool on);

    bool expanded() const;
    void setExpanded(bool on);

    // 动画开关(默认开)
    bool animationEnabled() const;
    void setAnimationEnabled(bool on);

    // 自定义标题内容(对标 title slot);传入后取代默认标题文本
    void setTitleWidget(QWidget *widget);
    QWidget *titleWidget() const;

    // 设置内容控件(取代默认内容布局)
    void setContentWidget(QWidget *widget);
    QWidget *contentWidget() const;

    // 内容区布局,可直接 addWidget 追加多个控件
    QVBoxLayout *contentLayout() const;

    QSize sizeHint() const override;

Q_SIGNALS:
    void titleChanged(const QString &title);
    void nameChanged(const QString &name);
    void disabledChanged(bool disabled);
    void expandChanged(bool expanded);
    void animationEnabledChanged(bool on);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void init();
    void updateArrow();
    void updateHeaderVisualState();
    int contentNaturalHeight() const;
    void doExpand(bool on);
    void onHeaderClicked();
    void onAnimationFinished();
};
}  // namespace DA

#endif // DACOLLAPSEITEM_H
