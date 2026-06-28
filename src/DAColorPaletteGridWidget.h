#ifndef DACOLORPALETTEGRIDWIDGET_H
#define DACOLORPALETTEGRIDWIDGET_H

#include <QWidget>
#include <QList>
#include <QColor>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

namespace DA
{
class DAColorGridWidget;

/**
 * @brief Office 风格色板控件。
 *
 * 上方一行标准主色,下方 5 行(3 行浅色 + 2 行深色)由主色通过 lighter 因子
 * 自动生成的调色板。内部组合两个 DAColorGridWidget。
 *
 * 默认主色列表为 10 色 Office 风格(深红/红/橙/黄/黄绿/绿/浅蓝/蓝/深蓝/紫)。
 */
class DAWIDGETS_EXPORT DAColorPaletteGridWidget : public QWidget
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DAColorPaletteGridWidget)
public:
    // 构造默认Office风格色板
    explicit DAColorPaletteGridWidget(QWidget* par = nullptr);
    // 构造自定义主色色板
    explicit DAColorPaletteGridWidget(const QList<QColor>& cls, QWidget* par = nullptr);
    // 析构函数
    ~DAColorPaletteGridWidget();

    // 设置主色列表，会重新生成调色板
    void setColorList(const QList<QColor>& cls);
    // 获取主色列表
    QList<QColor> colorList() const;

    // 设置浅色/深色变体的 lighter/darker 因子列表
    void setFactor(const QList<int>& factor);
    // 获取浅色/深色变体因子列表
    QList<int> factor() const;

    // 设置色块图标尺寸
    void setColorIconSize(const QSize& s);
    // 获取色块图标尺寸
    QSize colorIconSize() const;

    // 设置色块是否可选中
    void setColorCheckable(bool on = true);
    // 返回色块是否可选中
    bool isColorCheckable() const;

Q_SIGNALS:
    /**
     * @brief 点击任意色块时发射
     * @param c 被点击的颜色
     */
    void colorClicked(const QColor& c);

private:
    void init();
    QList<QColor> makeColorPalette(const QList<QColor>& cls) const;
    void onMainColorClicked(const QColor& c);
    void onPaletteColorClicked(const QColor& c);
};
}  // namespace DA

#endif  // DACOLORPALETTEGRIDWIDGET_H
