#ifndef DACOLLAPSEITEM_H
#define DACOLLAPSEITEM_H

#include <QFrame>
#include "DAWidgetsGlobals.h"

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
 * DA--DACollapseItem #header { background:#fafafa; }
 * DA--DACollapseItem #header:hover { background:#f5f7fa; }
 * QLabel#DACollapseItemArrow { color:#909399; }
 * QLabel#DACollapseItemTitle { color:#303133; font-weight:bold; }
 * DA--DACollapseItem[disabled="true"] #header { color:#c0c4cc; }
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
    // 构造空标题的折叠条目
    explicit DACollapseItem(QWidget *parent = nullptr);
    // 构造指定标题的折叠条目
    explicit DACollapseItem(const QString &title, QWidget *parent = nullptr);
    // 析构,释放标题栏与内容区子控件
    ~DACollapseItem();

    // 获取标题文本
    QString title() const;
    // 设置标题文本
    void setTitle(const QString &title);

    // 获取面板标识(供容器 activeNames 引用)
    QString name() const;
    // 设置面板标识
    void setName(const QString &name);

    // 是否禁用(禁用后不可点击展开)
    bool disabled() const;
    // 启用/禁用面板交互
    void setDisabled(bool on);

    // 是否处于展开态
    bool expanded() const;
    // 展开/折叠面板(可带动画)
    void setExpanded(bool on);

    // 动画开关(默认开)
    bool animationEnabled() const;
    // 设置是否启用展开/折叠动画
    void setAnimationEnabled(bool on);

    // 自定义标题内容(对标 title slot);传入后取代默认标题文本
    void setTitleWidget(QWidget *widget);
    // 获取自定义标题控件(未设置时返回 nullptr)
    QWidget *titleWidget() const;

    // 标题栏控件(键盘导航与自定义样式用;当前实现是内部按钮)
    QWidget *headerWidget() const;

    // 内容高度重算:展开态下更新 maximumHeight(动画中则更新动画终点)。
    // 异步加载内容的宿主在 setContentWidget() 之后调用它。
    void refreshContentHeight();

    // 设置内容控件(取代默认内容布局)
    void setContentWidget(QWidget *widget);
    // 获取内容控件(未设置时返回 nullptr)
    QWidget *contentWidget() const;

    // 内容区布局,可直接 addWidget 追加多个控件
    QVBoxLayout *contentLayout() const;

    // 推荐尺寸
    QSize sizeHint() const override;

Q_SIGNALS:
    /**
     * @brief 标题文本变化时发射
     * @param title 新标题文本
     */
    void titleChanged(const QString &title);
    /**
     * @brief 面板标识变化时发射
     * @param name 新面板标识
     */
    void nameChanged(const QString &name);
    /**
     * @brief 禁用状态变化时发射
     * @param disabled 是否禁用
     */
    void disabledChanged(bool disabled);
    /**
     * @brief 展开/折叠状态变化时发射
     * @param expanded 是否展开
     */
    void expandChanged(bool expanded);
    /**
     * @brief 动画开关变化时发射
     * @param on 是否启用动画
     */
    void animationEnabledChanged(bool on);
    /**
     * @brief 首次展开且内容区为空时、动画开始前发射(每个条目只发一次)
     */
    void contentRequested();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    friend class DACollapseWidget;  // 仅用于下发变体的间距/留白
    void init();
    void updateArrow();
    void updateHeaderVisualState();
    int contentNaturalHeight() const;
    void doExpand(bool on);
    void onHeaderClicked();
    void onAnimationFinished();
    // 按 daVariant 动态属性重设标题/内容留白
    void applyVariantChrome();
};
}  // namespace DA

#endif // DACOLLAPSEITEM_H
