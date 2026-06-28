#ifndef DABRUSHEDITWIDGET_H
#define DABRUSHEDITWIDGET_H

#include <QWidget>
#include <QBrush>
#include <QGradient>
#include <QPixmap>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

namespace DA
{
class DAColorMenu;

/**
 * @brief 画刷编辑窗口
 *
 * 颜色按钮 + 画刷样式下拉框。支持纯色、图案、渐变(Linear/Radial/Conical)、
 * 纹理(QPixmap)四种填充模式。
 *
 * - 纯色/图案:颜色按钮选色,下拉框选 NoBrush/SolidPattern/Dense1-7/Hor/Ver/Cross/Diag 等
 * - 渐变:下拉框选 LinearGradientPattern/RadialGradientPattern/ConicalGradientPattern,
 *   自动弹出渐变编辑对话框(类型+起始色+结束色+预览)
 * - 纹理:下拉框选 TexturePattern,自动弹出文件对话框选择图片
 *
 * 颜色与样式联动:颜色有效时若样式为 NoBrush 自动改为 SolidPattern;
 * 颜色无效(清空)时样式自动改为 NoBrush。
 */
class DAWIDGETS_EXPORT DABrushEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DABrushEditWidget(QWidget* parent = nullptr);
    ~DABrushEditWidget();

    QBrush getCurrentBrush() const;
    void setStyleTextVisible(bool on);
    bool isStyleTextVisible() const;
    Qt::BrushStyle getCurrentBrushStyle() const;
    QColor getCurrentBrushColor() const;

public Q_SLOTS:
    void setCurrentBrush(const QBrush& b);
    void setCurrentBrushStyle(Qt::BrushStyle s);

Q_SIGNALS:
    void brushChanged(const QBrush& b);

private Q_SLOTS:
    void onColorChanged(const QColor& c);
    void onBrushStyleChanged(Qt::BrushStyle s);

private:
    class PrivateData;
    PrivateData* d;
    void editGradient(Qt::BrushStyle style);
    void editTexture();
};
}  // namespace DA

#endif  // DABRUSHEDITWIDGET_H
