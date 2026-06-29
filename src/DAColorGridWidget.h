#ifndef DACOLORGRIDWIDGET_H
#define DACOLORGRIDWIDGET_H

#include <QWidget>
#include <QList>
#include <QColor>
#include "DAWidgetsGlobals.h"

class QToolButton;
class QGridLayout;
class QButtonGroup;
class QSpacerItem;

namespace DA
{
/**
 * @brief 基础颜色网格控件。
 *
 * 使用 QGridLayout + QButtonGroup(exclusive) 排列一组颜色色块按钮,
 * 每个色块为 QToolButton(IconOnly),颜色通过 QPixmap icon 显示。
 *
 * 是 DAColorPaletteGridWidget 和 DAColorMenu 的基础构件。
 */
class DAWIDGETS_EXPORT DAColorGridWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    DA_DECLARE_PRIVATE(DAColorGridWidget)
public:
    using FunColorBtn = std::function<void(QToolButton*)>;

    // 构造颜色网格控件
    explicit DAColorGridWidget(QWidget* par = nullptr);
    // 析构函数
    ~DAColorGridWidget();

    // 设置网格列数，颜色按钮按行优先排列
    void setColumnCount(int c);
    // 获取网格列数
    int columnCount() const;

    // 设置颜色列表，会触发网格重建
    void setColorList(const QList<QColor>& cls);
    // 获取当前颜色列表
    QList<QColor> getColorList() const;

    // 获取颜色间距（水平和垂直的统一值）
    int spacing() const;
    // 设置颜色间距（水平和垂直统一设置）
    void setSpacing(int v);

    // 获取颜色数量
    int colorCount() const;

    // 设置色块图标尺寸
    void setColorIconSize(const QSize& s);
    // 获取色块图标尺寸
    QSize colorIconSize() const;

    // 设置色块是否可选中（checkable）
    void setColorCheckable(bool on = true);
    // 返回色块是否可选中
    bool isColorCheckable() const;

    // 获取当前被选中的颜色（基于buttonGroup的checked状态）
    QColor currentCheckedColor() const;
    // 根据索引获取对应位置的色块按钮
    QToolButton* colorButton(int index) const;

    // 设置垂直间距
    void setVerticalSpacing(int v);
    // 获取垂直间距
    int verticalSpacing() const;
    // 设置水平间距
    void setHorizontalSpacing(int v);
    // 获取水平间距
    int horizontalSpacing() const;

    // 清除所有色块的选中状态
    void clearCheckedState();

    // 遍历所有色块按钮并执行回调函数
    void iterationColorBtns(FunColorBtn fn);

    // 设置指定行的最小高度
    void setRowMinimumHeight(int row, int minSize);
    // 在右侧添加水平弹簧，使色块左对齐
    void setHorizontalSpacerToRight(bool on = true);

    // 推荐尺寸
    QSize sizeHint() const override;

Q_SIGNALS:
    /**
     * @brief 颜色被点击时发射
     * @param c 被点击的颜色
     */
    void colorClicked(const QColor& c);
    /**
     * @brief 颜色按钮被按下时发射
     * @param c 被按下的颜色
     */
    void colorPressed(const QColor& c);
    /**
     * @brief 颜色按钮被释放时发射
     * @param c 被释放的颜色
     */
    void colorReleased(const QColor& c);
    /**
     * @brief 颜色按钮选中状态切换时发射
     * @param c 切换的颜色
     * @param on 是否选中
     */
    void colorToggled(const QColor& c, bool on);

private:
    void updateGridColor(bool isRemoveSpacer = false);
    void setColorAt(const QColor& clr, int r, int c);
    static QIcon makeColorIcon(const QColor& clr, const QSize& size);
};
}  // namespace DA

#endif  // DACOLORGRIDWIDGET_H
