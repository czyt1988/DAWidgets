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
    explicit DAFlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    explicit DAFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~DAFlowLayout() override;

    void addItem(QLayoutItem *item) override;
    void insertWidget(int index, QWidget *widget);
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect &rect) override;
    QSize sizeHint() const override;
    QLayoutItem *takeAt(int index) override;

    void setTrailingExpanding(bool on);
    bool trailingExpanding() const;

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;
};
}  // namespace DA

#endif // DAFLOWLAYOUT_H
