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
    // 构造默认主题色的颜色菜单
    explicit DAColorMenu(QWidget* parent = nullptr);
    // 构造带标题的颜色菜单
    explicit DAColorMenu(const QString& title, QWidget* parent = nullptr);
    // 析构函数
    ~DAColorMenu();

    // 获取主题色板的 QWidgetAction
    QWidgetAction* themeColorsPaletteAction() const;
    // 获取自定义颜色记录网格的 QWidgetAction
    QWidgetAction* getCustomColorsWidgetAction() const;
    // 获取"自定义颜色"Action（点击弹出QColorDialog）
    QAction* customColorAction() const;

    // 获取主题色板网格控件
    DAColorPaletteGridWidget* colorPaletteGridWidget() const;
    // 获取自定义颜色记录网格控件
    DAColorGridWidget* customColorsWidget() const;

    // 启用或禁用"无颜色"Action
    void enableNoneColorAction(bool on = true);
    // 获取"无颜色"Action
    QAction* noneColorAction() const;

public Q_SLOTS:
    // 发射 selectedColor 信号并隐藏菜单
    void emitSelectedColor(const QColor& c);

Q_SIGNALS:
    /**
     * @brief 用户选色后发射
     * @param c 选中的颜色，无效颜色表示"无颜色"
     */
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
