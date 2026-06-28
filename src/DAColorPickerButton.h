#ifndef DACOLORPICKERBUTTON_H
#define DACOLORPICKERBUTTON_H

#include <QToolButton>
#include <QColor>
#include <QMargins>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QStylePainter;
class QStyleOptionToolButton;

namespace DA
{
class DAColorMenu;

/**
 * @brief 颜色拾取按钮。
 *
 * 继承 QToolButton,完全自绘:在按钮区域内绘制颜色块(可选图标+文本+颜色条布局)。
 * 默认 WithColorMenu 模式自动创建 DAColorMenu,点击弹出选色;
 * NoColorMenu 模式下点击发射 colorClicked 信号,由外部处理选色逻辑。
 *
 * 无效颜色(QColor())显示为红色斜杠+黑框的"无色"标记。
 *
 * qss 定制示例:
 * ```css
 * DA--DAColorPickerButton { border:1px solid #dcdfe6; border-radius:3px; }
 * DA--DAColorPickerButton:hover { border:1px solid #409eff; }
 * ```
 */
class DAWIDGETS_EXPORT DAColorPickerButton : public QToolButton
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DAColorPickerButton)
public:
    /// 按钮样式:是否自带颜色菜单
    enum ColorToolButtonStyle {
        WithColorMenu,  ///< 默认:自动构建 DAColorMenu
        NoColorMenu     ///< 不带颜色菜单
    };

    explicit DAColorPickerButton(QWidget* parent = nullptr);
    explicit DAColorPickerButton(ColorToolButtonStyle style, QWidget* parent = nullptr);
    ~DAColorPickerButton();

    QColor color() const;

    QMargins margins() const;
    void setMargins(const QMargins& mg);

    ColorToolButtonStyle colorToolButtonStyle() const;
    void setColorToolButtonStyle(ColorToolButtonStyle s);

    DAColorMenu* colorMenu() const;
    DAColorMenu* createColorMenu();

    /// 绘制"无颜色"标记:红色对角线+黑色边框
    static void paintNoneColor(QPainter* p, const QRect& colorRect);

Q_SIGNALS:
    void colorClicked(const QColor& color, bool checked = false);
    void colorChanged(const QColor& color);

public Q_SLOTS:
    void setColor(const QColor& c);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;

    virtual void calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    virtual void paintButton(QStylePainter* p, const QStyleOptionToolButton& opt);
    virtual void paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt);
    virtual void paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt);
    virtual void paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt);

private:
    void onButtonClicked(bool checked = false);
};
}  // namespace DA

#endif  // DACOLORPICKERBUTTON_H
