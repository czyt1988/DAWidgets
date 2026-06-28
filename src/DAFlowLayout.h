#ifndef DAFLOWLAYOUT_H
#define DAFLOWLAYOUT_H

#include <QLayout>
#include <QList>
#include <QRect>
#include <QStyle>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QWidget;

namespace DA
{
/**
 * @brief 流式布局，子项按行排列、自动换行。
 *
 * 与 Qt 官方 Flow Layout 示例一致，额外支持 #trailingExpanding：开启后
 * 最后一个布局项会占据当前行的剩余宽度（用于在 DAChipsEdit 末尾嵌入自适应
 * 宽度的输入框）；剩余宽度不足时自动换行并占满整行。
 */
class DAWIDGETS_EXPORT DAFlowLayout : public QLayout
{
    DA_DECLARE_PRIVATE(DAFlowLayout)
public:
    // 构造流式布局并挂载到父控件
    explicit DAFlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    // 构造流式布局（无父控件，-1 表示使用 style 默认间距）
    explicit DAFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    // 析构，释放所有布局项
    ~DAFlowLayout() override;

    // 添加布局项到末尾
    void addItem(QLayoutItem *item) override;
    // 在指定位置插入控件
    void insertWidget(int index, QWidget *widget);
    // 获取水平间距（-1 表示使用 style 默认值）
    int horizontalSpacing() const;
    // 获取垂直间距（-1 表示使用 style 默认值）
    int verticalSpacing() const;
    // 返回布局可扩展的方向
    Qt::Orientations expandingDirections() const override;
    // 是否支持 height-for-width 布局（流式布局必须返回 true）
    bool hasHeightForWidth() const override;
    // 根据给定宽度计算所需高度
    int heightForWidth(int width) const override;
    // 获取布局项数量
    int count() const override;
    // 获取指定索引的布局项
    QLayoutItem *itemAt(int index) const override;
    // 获取最小尺寸
    QSize minimumSize() const override;
    // 执行布局，排列所有子项
    void setGeometry(const QRect &rect) override;
    // 获取推荐尺寸
    QSize sizeHint() const override;
    // 移除并返回指定索引的布局项
    QLayoutItem *takeAt(int index) override;

    // 设置末尾项是否占满剩余宽度（用于内嵌自适应输入框）
    void setTrailingExpanding(bool on);
    // 获取末尾项是否占满剩余宽度
    bool trailingExpanding() const;

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;
};
}  // namespace DA

#endif // DAFLOWLAYOUT_H
