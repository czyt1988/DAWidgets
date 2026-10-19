#ifndef DACOLORSCALEPREVIEWWIDGET_H
#define DACOLORSCALEPREVIEWWIDGET_H

#include <QWidget>
#include <QColor>
#include <QList>
#include "DAWidgetsGlobals.h"

namespace DA
{

/**
 * @brief 色阶渐变预览条（2/3 色水平渐变）
 *
 * 条件格式色阶编辑等场景的效果预览：按传入的 2 个（首尾）或 3 个（首/中/尾）
 * 颜色绘制水平线性渐变（三色时中点色停在 0.5 位置），外框 1px 中性描边。
 * 颜色数不为 2/3 或存在无效色时绘制空白态。
 */
class DAWIDGETS_EXPORT DAColorScalePreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DAColorScalePreviewWidget(QWidget* parent = nullptr);

    // 设置渐变色（2 或 3 个有效色生效，其余为空白态）
    void setColors(const QList< QColor >& colors);
    // 获取当前渐变色
    QList< QColor > colors() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    /**
     * @brief 渐变色变化
     * @param colors 新渐变色
     */
    void colorsChanged(const QList< QColor >& colors);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QList< QColor > mColors;  ///< 渐变色（2/3 个有效色，否则空白态）
};
}  // namespace DA

#endif  // DACOLORSCALEPREVIEWWIDGET_H
