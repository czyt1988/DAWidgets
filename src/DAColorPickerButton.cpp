#include "DAColorPickerButton.h"

#include "DAColorMenu.h"

#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QStyle>
#include <QResizeEvent>
#include <QPaintEvent>

namespace DA
{
class DAColorPickerButton::PrivateData
{
    DA_DECLARE_PUBLIC(DAColorPickerButton)
public:
    PrivateData(DAColorPickerButton* p);
    QColor mColor { Qt::white };
    int mSpacing { 2 };                              // icon 与 color 之间的间隔
    QMargins mMargins { 3, 3, 3, 3 };                // 内容边距
    ColorToolButtonStyle mColorButtonStyle { WithColorMenu };
};

DAColorPickerButton::PrivateData::PrivateData(DAColorPickerButton* p) : q_ptr(p)
{
}

//===================================================
// DAColorPickerButton
//===================================================

/**
 * @brief 构造默认WithColorMenu模式的颜色拾取按钮
 * @param parent 父控件
 */
DAColorPickerButton::DAColorPickerButton(QWidget* parent)
    : QToolButton(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    connect(this, &QToolButton::clicked, this, &DAColorPickerButton::onButtonClicked);
    setColorToolButtonStyle(WithColorMenu);
}

/**
 * @brief 构造指定样式的颜色拾取按钮
 * @param style 按钮样式模式
 * @param parent 父控件
 */
DAColorPickerButton::DAColorPickerButton(ColorToolButtonStyle style, QWidget* parent)
    : QToolButton(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    connect(this, &QToolButton::clicked, this, &DAColorPickerButton::onButtonClicked);
    setColorToolButtonStyle(style);
}

/**
 * @brief 析构函数
 */
DAColorPickerButton::~DAColorPickerButton()
{
}

/**
 * @brief 获取当前颜色
 * @return 当前颜色
 */
QColor DAColorPickerButton::color() const
{
    DA_DC(d);
    return d->mColor;
}

/**
 * @brief 设置当前颜色
 * @param c 新颜色
 * @details 颜色变化时更新显示并发射colorChanged信号
 */
void DAColorPickerButton::setColor(const QColor& c)
{
    DA_D(d);
    if (d->mColor == c)
        return;
    d->mColor = c;
    update();
    Q_EMIT colorChanged(c);
}

/**
 * @brief 获取绘制边距
 * @return 内容边距
 */
QMargins DAColorPickerButton::margins() const
{
    DA_DC(d);
    return d->mMargins;
}

/**
 * @brief 设置绘制边距
 * @param mg 新边距
 */
void DAColorPickerButton::setMargins(const QMargins& mg)
{
    DA_D(d);
    d->mMargins = mg;
    update();
}

/**
 * @brief 获取按钮样式模式
 * @return 当前样式模式
 */
DAColorPickerButton::ColorToolButtonStyle DAColorPickerButton::colorToolButtonStyle() const
{
    DA_DC(d);
    return d->mColorButtonStyle;
}

/**
 * @brief 设置按钮样式模式
 * @param s 样式模式
 * @details WithColorMenu模式自动创建DAColorMenu，NoColorMenu模式移除已有菜单
 */
void DAColorPickerButton::setColorToolButtonStyle(ColorToolButtonStyle s)
{
    DA_D(d);
    d->mColorButtonStyle = s;
    if (s == WithColorMenu) {
        createColorMenu();
    } else {
        QMenu* m = menu();
        if (m) {
            setMenu(nullptr);
            m->deleteLater();
        }
        setPopupMode(QToolButton::DelayedPopup);
    }
}

/**
 * @brief 获取关联的颜色菜单
 * @return 颜色菜单指针，不存在返回nullptr
 */
DAColorMenu* DAColorPickerButton::colorMenu() const
{
    return qobject_cast<DAColorMenu*>(menu());
}

/**
 * @brief 创建并关联颜色菜单
 * @return 创建的DAColorMenu指针
 * @details 设置InstantPopup模式，启用NoneColorAction，连接选色信号
 */
DAColorMenu* DAColorPickerButton::createColorMenu()
{
    DA_D(d);
    setPopupMode(QToolButton::InstantPopup);
    DAColorMenu* m = new DAColorMenu(this);
    m->enableNoneColorAction(true);
    connect(m, &DAColorMenu::selectedColor, this, &DAColorPickerButton::setColor);
    setMenu(m);
    return m;
}

/**
 * @brief 绘制"无颜色"标记
 * @param p 画笔
 * @param colorRect 绘制区域
 * @details 绘制红色对角线+黑色边框表示无颜色状态
 */
void DAColorPickerButton::paintNoneColor(QPainter* p, const QRect& colorRect)
{
    p->save();
    QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
    p->setPen(pen);
    p->setRenderHint(QPainter::SmoothPixmapTransform, true);
    p->setRenderHint(QPainter::Antialiasing, true);
    int ss = colorRect.width() / 3;
    p->drawLine(QPoint(colorRect.x() + ss, colorRect.bottom()),
                QPoint(colorRect.right() - ss, colorRect.top()));
    pen.setColor(Qt::black);
    p->setPen(pen);
    p->drawRect(colorRect);
    p->restore();
}

/**
 * @brief 按钮点击时发射colorClicked信号
 * @param checked 按钮是否处于选中状态
 */
void DAColorPickerButton::onButtonClicked(bool checked)
{
    DA_D(d);
    Q_EMIT colorClicked(d->mColor, checked);
}

/**
 * @brief 重绘事件，自绘按钮全部内容
 * @details 按顺序绘制：按钮面板→图标→文字→颜色块
 */
void DAColorPickerButton::paintEvent(QPaintEvent*)
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QRect iconRect, textRect, colorRect;
    calcRect(opt, iconRect, textRect, colorRect);

    paintButton(&p, opt);
    paintIcon(&p, iconRect, opt);
    paintText(&p, textRect, opt);
    DA_D(d);
    paintColor(&p, colorRect, d->mColor, opt);
}

/**
 * @brief 尺寸变化事件，触发重绘
 * @param event 尺寸变化事件
 */
void DAColorPickerButton::resizeEvent(QResizeEvent* event)
{
    QToolButton::resizeEvent(event);
    update();
}

/**
 * @brief 推荐尺寸
 * @return 根据toolButtonStyle、图标、文字、边距计算的推荐尺寸
 * @details 不同ToolButtonStyle下图标/文字/颜色块的排列方式不同
 */
QSize DAColorPickerButton::sizeHint() const
{
    DA_DC(d);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    int w = 0, h = 0;
    const QSize is = iconSize();
    const QSize ts = opt.fontMetrics.size(Qt::TextShowMnemonic, text());

    switch (toolButtonStyle()) {
    case Qt::ToolButtonIconOnly:
        w = is.width() + d->mMargins.left() + d->mMargins.right();
        h = is.height() + d->mMargins.top() + d->mMargins.bottom();
        break;
    case Qt::ToolButtonTextOnly:
        w = ts.width() + d->mSpacing + is.width() + d->mMargins.left() + d->mMargins.right();
        h = qMax(ts.height(), is.height()) + d->mMargins.top() + d->mMargins.bottom();
        break;
    case Qt::ToolButtonTextBesideIcon: {
        int extraH = 0;
        if (!opt.icon.isNull())
            extraH = is.height() / 4 + d->mSpacing;
        w = ts.width() + d->mSpacing + is.width() + d->mMargins.left() + d->mMargins.right();
        h = qMax(ts.height(), is.height() + extraH) + d->mMargins.top() + d->mMargins.bottom();
        break;
    }
    case Qt::ToolButtonTextUnderIcon:
        w = qMax(ts.width(), is.width()) + d->mMargins.left() + d->mMargins.right();
        h = ts.height() + is.height() + d->mSpacing + d->mMargins.top() + d->mMargins.bottom();
        break;
    default:
        w = is.width() + d->mMargins.left() + d->mMargins.right();
        h = is.height() + d->mMargins.top() + d->mMargins.bottom();
        break;
    }

    if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
        opt.features & QStyleOptionToolButton::HasMenu) {
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
    }

    return style()->sizeFromContents(QStyle::CT_ToolButton, &opt, QSize(w, h), this).expandedTo(QSize(2, 2));
}

/**
 * @brief 计算图标、文字、颜色块的布局矩形
 * @param opt 样式选项
 * @param iconRect [out] 图标矩形
 * @param textRect [out] 文字矩形
 * @param colorRect [out] 颜色块矩形
 * @details 根据ToolButtonStyle和是否有图标计算各元素的位置和大小
 */
void DAColorPickerButton::calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect)
{
    DA_D(d);
    QRect buttonRect = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton, this);
    buttonRect = buttonRect.marginsRemoved(d->mMargins);

    const QSize is = iconSize();
    const QSize ts = opt.fontMetrics.size(Qt::TextShowMnemonic, text());

    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly: {
        if (opt.icon.isNull()) {
            // 无 icon:颜色占满
            colorRect = buttonRect;
            iconRect = QRect();
            textRect = QRect();
        } else {
            // icon 在上,color 在下(高度为 icon 的 1/4)
            int colorHeight = is.height() / 4;
            int totalHeight = colorHeight + is.height() + d->mSpacing;
            if (totalHeight > buttonRect.height()) {
                // 空间不足:反推
                colorHeight = (buttonRect.height() - d->mSpacing) / 5;
                int iconHeight = colorHeight * 4;
                int iconWidth = is.width() * (float)iconHeight / is.height();
                int yOffset = (buttonRect.height() - totalHeight) / 2;
                if (yOffset < 0)
                    yOffset = 0;
                iconRect = QRect(buttonRect.x() + (buttonRect.width() - iconWidth) / 2,
                                 buttonRect.y() + yOffset, iconWidth, iconHeight);
                colorRect = QRect(iconRect.x(), iconRect.bottom() + d->mSpacing,
                                  iconRect.width(), colorHeight);
            } else {
                int yOffset = (buttonRect.height() - totalHeight) / 2;
                iconRect = QRect(buttonRect.x() + (buttonRect.width() - is.width()) / 2,
                                 buttonRect.y() + yOffset, is.width(), is.height());
                colorRect = QRect(iconRect.x(), iconRect.bottom() + d->mSpacing,
                                  iconRect.width(), colorHeight);
            }
            textRect = QRect();
        }
        break;
    }
    case Qt::ToolButtonTextOnly: {
        iconRect = QRect();
        QSize colorSize = is.isValid() ? is : QSize(16, 16);
        colorRect = QRect(buttonRect.x(),
                          buttonRect.y() + (buttonRect.height() - colorSize.height()) / 2,
                          colorSize.width(), colorSize.height());
        textRect = buttonRect.adjusted(colorRect.right() + d->mSpacing, 0, 0, 0);
        break;
    }
    case Qt::ToolButtonTextBesideIcon: {
        if (opt.icon.isNull()) {
            // 无 icon:同 TextOnly 的左色右文
            iconRect = QRect();
            QSize colorSize = is.isValid() ? is : QSize(16, 16);
            colorRect = QRect(buttonRect.x(),
                              buttonRect.y() + (buttonRect.height() - colorSize.height()) / 2,
                              colorSize.width(), colorSize.height());
            textRect = buttonRect.adjusted(colorRect.right() + d->mSpacing, 0, 0, 0);
        } else {
            // icon+color 在左(垂直叠放),text 在右
            int colorHeight = is.height() / 4;
            int totalHeight = is.height() + colorHeight + d->mSpacing;
            int yOffset = (buttonRect.height() - totalHeight) / 2;
            if (yOffset < 0)
                yOffset = 0;
            iconRect = QRect(buttonRect.x(), buttonRect.y() + yOffset, is.width(), is.height());
            colorRect = QRect(iconRect.x(), iconRect.bottom() + d->mSpacing, iconRect.width(), colorHeight);
            textRect = buttonRect.adjusted(iconRect.right() + d->mSpacing, 0, 0, 0);
        }
        break;
    }
    case Qt::ToolButtonTextUnderIcon: {
        if (opt.icon.isNull()) {
            // 无 icon:color 在上,text 在下
            int colorHeight = is.isValid() ? is.height() : 16;
            int totalHeight = colorHeight + ts.height() + d->mSpacing;
            int yOffset = (buttonRect.height() - totalHeight) / 2;
            if (yOffset < 0)
                yOffset = 0;
            int colorWidth = is.isValid() ? is.width() : 16;
            colorRect = QRect(buttonRect.x() + (buttonRect.width() - colorWidth) / 2,
                              buttonRect.y() + yOffset, colorWidth, colorHeight);
            textRect = QRect(buttonRect.x(), colorRect.bottom() + d->mSpacing,
                             buttonRect.width(), ts.height());
            iconRect = QRect();
        } else {
            // 从上到下:icon → color → text
            int colorHeight = is.height() / 4;
            int totalHeight = ts.height() + is.height() + colorHeight + 2 * d->mSpacing;
            int yOffset = (buttonRect.height() - totalHeight) / 2;
            if (yOffset < 0)
                yOffset = 0;
            iconRect = QRect(buttonRect.x() + (buttonRect.width() - is.width()) / 2,
                             buttonRect.y() + yOffset, is.width(), is.height());
            colorRect = QRect(iconRect.x(), iconRect.bottom() + d->mSpacing,
                              iconRect.width(), colorHeight);
            textRect = QRect(buttonRect.x(), colorRect.bottom() + d->mSpacing,
                             buttonRect.width(), ts.height());
        }
        break;
    }
    default:
        colorRect = buttonRect;
        iconRect = QRect();
        textRect = QRect();
        break;
    }
}

/**
 * @brief 绘制按钮面板（背景、边框、下拉箭头、焦点框）
 * @param p 画笔
 * @param opt 样式选项
 */
void DAColorPickerButton::paintButton(QStylePainter* p, const QStyleOptionToolButton& opt)
{
    if (opt.state & QStyle::State_AutoRaise) {
        p->drawPrimitive(QStyle::PE_PanelButtonTool, opt);
    } else {
        p->drawPrimitive(QStyle::PE_PanelButtonBevel, opt);
    }
    // 下拉箭头
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
        QStyleOptionToolButton arrowOpt = opt;
        QRect mr = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);
        arrowOpt.rect = mr;
        p->drawPrimitive(QStyle::PE_IndicatorArrowDown, arrowOpt);
    }
    // 焦点框
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(3, 3, -3, -3);
        p->drawPrimitive(QStyle::PE_FrameFocusRect, fr);
    }
}

/**
 * @brief 绘制图标
 * @param p 画笔
 * @param iconRect 图标区域
 * @param opt 样式选项
 */
void DAColorPickerButton::paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt)
{
    if (iconRect.isNull() || iconRect.isEmpty())
        return;
    if (!opt.icon.isNull()) {
        opt.icon.paint(p, iconRect);
    }
}

/**
 * @brief 绘制文字
 * @param p 画笔
 * @param textRect 文字区域
 * @param opt 样式选项
 */
void DAColorPickerButton::paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt)
{
    if (textRect.isNull() || textRect.isEmpty())
        return;
    if (!opt.text.isEmpty()) {
        p->drawText(textRect, Qt::AlignCenter, opt.text);
    }
}

/**
 * @brief 绘制颜色块
 * @param p 画笔
 * @param colorRect 颜色块区域
 * @param color 颜色，无效颜色时绘制"无颜色"标记
 * @param opt 样式选项
 */
void DAColorPickerButton::paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt)
{
    if (colorRect.isNull() || colorRect.isEmpty())
        return;
    if (color.isValid()) {
        p->fillRect(colorRect, color);
    } else {
        paintNoneColor(p, colorRect);
    }
}
}  // namespace DA
