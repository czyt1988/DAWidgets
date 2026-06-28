#ifndef DACOLORMENU_H
#define DACOLORMENU_H

#include <QMenu>
#include <QColor>
#include <QList>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QLabel;
class QWidgetAction;
class QColorDialog;

namespace DA
{
class DAColorPaletteGridWidget;
class DAColorGridWidget;

/**
 * @brief 颜色选择菜单。
 *
 * 结构:标题 Label → 主题色板(DAColorPaletteGridWidget)→ (可选)"None" Action →
 * "Custom Color" Action → 自定义颜色记录网格(DAColorGridWidget)。
 *
 * 选色后发射 selectedColor 信号并自动隐藏菜单。
 * "Custom Color" 弹出 QColorDialog,选中的颜色记录到自定义色网格(最多 10 个,FIFO)。
 */
class DAWIDGETS_EXPORT DAColorMenu : public QMenu
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DAColorMenu)
public:
    explicit DAColorMenu(QWidget* parent = nullptr);
    explicit DAColorMenu(const QString& title, QWidget* parent = nullptr);
    ~DAColorMenu();

    QWidgetAction* themeColorsPaletteAction() const;
    QWidgetAction* getCustomColorsWidgetAction() const;
    QAction* customColorAction() const;

    DAColorPaletteGridWidget* colorPaletteGridWidget() const;
    DAColorGridWidget* customColorsWidget() const;

    void enableNoneColorAction(bool on = true);
    QAction* noneColorAction() const;

public Q_SLOTS:
    void emitSelectedColor(const QColor& c);

Q_SIGNALS:
    void selectedColor(const QColor& c);

private:
    void init(const QList<QColor>& themeCls);
    void onCustomColorActionTriggered(bool on = false);
    void onNoneColorActionTriggered(bool on = false);
    QColor getColorByDialog();
    void recordCustomColor(const QColor& c);
    static QIcon createNoneColorIcon();
};
}  // namespace DA

#endif  // DACOLORMENU_H
