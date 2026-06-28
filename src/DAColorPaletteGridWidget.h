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
    explicit DAColorPaletteGridWidget(QWidget* par = nullptr);
    explicit DAColorPaletteGridWidget(const QList<QColor>& cls, QWidget* par = nullptr);
    ~DAColorPaletteGridWidget();

    void setColorList(const QList<QColor>& cls);
    QList<QColor> colorList() const;

    void setFactor(const QList<int>& factor);
    QList<int> factor() const;

    void setColorIconSize(const QSize& s);
    QSize colorIconSize() const;

    void setColorCheckable(bool on = true);
    bool isColorCheckable() const;

Q_SIGNALS:
    void colorClicked(const QColor& c);

private:
    void init();
    QList<QColor> makeColorPalette(const QList<QColor>& cls) const;
    void onMainColorClicked(const QColor& c);
    void onPaletteColorClicked(const QColor& c);
};
}  // namespace DA

#endif  // DACOLORPALETTEGRIDWIDGET_H
