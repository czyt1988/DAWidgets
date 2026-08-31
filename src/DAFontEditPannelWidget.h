#ifndef DAFONTEDITPANNELWIDGET_H
#define DAFONTEDITPANNELWIDGET_H

#include <QWidget>
#include <QColor>
#include <QFont>
#include "DAWidgetsGlobals.h"

namespace DA
{
/**
 * @brief 字体编辑面板
 *
 * 字体选择 + 字号 + 粗体/斜体/下划线 + 字体颜色。
 * 通过 currentFontChanged / currentFontColorChanged 信号通知变更。
 */
class DAWIDGETS_EXPORT DAFontEditPannelWidget : public QWidget
{
    Q_OBJECT
public:
    // 布局模式：TwoRowsLayout为默认两排布局，SingleRowLayout为单排布局（适合工具栏）
    enum LayoutMode
    {
        TwoRowsLayout,   ///< 两排布局：第一排字体族，第二排样式按钮+字号（默认）
        SingleRowLayout  ///< 单排布局：所有控件一行排列，适合工具栏等高度受限场景
    };
    Q_ENUM(LayoutMode)
    Q_PROPERTY(LayoutMode layoutMode READ layoutMode WRITE setLayoutMode)

public:
    // 构造字体编辑面板
    explicit DAFontEditPannelWidget(QWidget* parent = nullptr);
    // 析构函数
    ~DAFontEditPannelWidget();

    // 设置布局模式，默认TwoRowsLayout，切换时重建布局
    void setLayoutMode(LayoutMode m);
    // 获取布局模式
    LayoutMode layoutMode() const;

    // 获取当前字体颜色
    QColor getCurrentFontColor() const;
    // 设置当前字体颜色
    void setCurrentFontColor(const QColor& c);

    // 设置当前字体
    void setCurrentFont(const QFont& f);
    // 获取当前字体
    QFont getCurrentFont();

    // 设置字体粗细（Qt5用0-99，Qt6用QFont::Weight枚举，内部做兼容处理）
    void setFontWeight(int w);
    // 获取字体粗细
    int getFontWeight() const;

private Q_SLOTS:
    void onComboBoxFontSizeTextChanged(const QString& t);
    void onFontSizeAdd();
    void onFontSizeSub();
    void signalEmitFontChanged();

Q_SIGNALS:
    /**
     * @brief 字体改变时发射
     * @param f 新字体
     */
    void currentFontChanged(const QFont& f);
    /**
     * @brief 字体颜色改变时发射
     * @param c 新颜色
     */
    void currentFontColorChanged(const QColor& c);

private:
    class PrivateData;
    PrivateData* d;
};
}  // namespace DA

#endif  // DAFONTEDITPANNELWIDGET_H
