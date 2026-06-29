#ifndef DACOLLAPSELAYOUT_H
#define DACOLLAPSELAYOUT_H

#include <QLayout>
#include <QVector>
#include "DAWidgetsGlobals.h"

namespace DA
{
/**
 * @brief 折叠面板专用垂直布局器
 *
 * 从上到下依次排列子项,每个子项取其 sizeHint 的自然高度,
 * 不压缩、不拉伸、不调用 qGeomCalc()。
 * 空间不足时子项溢出底部而非被压缩,从根本上消除展开/折叠动画期间的抖动。
 *
 * 替换 DACollapseWidget 和 DACollapseItem 中的 QVBoxLayout。
 */
class DAWIDGETS_EXPORT DACollapseLayout : public QLayout
{
    Q_OBJECT
public:
    explicit DACollapseLayout(QWidget* parent = nullptr);
    ~DACollapseLayout() override;

    void addItem(QLayoutItem* item) override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;
    int count() const override;
    void setGeometry(const QRect& rect) override;
    QSize sizeHint() const override;
    QSize minimumSize() const override;
    QSize maximumSize() const override;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    void invalidate() override;

    void insertWidget(int index, QWidget* widget);
    void addStretch(int stretch = 0);

private:
    QVector<QLayoutItem*> m_items;
    mutable bool m_dirty;
    mutable QSize m_sizeHint;
    mutable QSize m_minSize;

    void recalc() const;
};
}  // namespace DA

#endif  // DACOLLAPSELAYOUT_H
