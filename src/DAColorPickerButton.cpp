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

DAColorPickerButton::DAColorPickerButton(QWidget* parent)
    : QToolButton(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    connect(this, &QToolButton::clicked, this, &DAColorPickerButton::onButtonClicked);
    setColorToolButtonStyle(WithColorMenu);
}

DAColorPickerButton::DAColorPickerButton(ColorToolButtonStyle style, QWidget* parent)
    : QToolButton(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    connect(this, &QToolButton::clicked, this, &DAColorPickerButton::onButtonClicked);
    setColorToolButtonStyle(style);
}

DAColorPickerButton::~DAColorPickerButton()
{
}

QColor DAColorPickerButton::color() const
{
    DA_DC(d);
    return d->mColor;
}

void DAColorPickerButton::setColor(const QColor& c)
{
    DA_D(d);
    if (d->mColor == c)
        return;
    d->mColor = c;
    update();
    Q_EMIT colorChanged(c);
}

QMargins DAColorPickerButton::margins() const
{
    DA_DC(d);
    return d->mMargins;
}

void DAColorPickerButton::setMargins(const QMargins& mg)
{
    DA_D(d);
    d->mMargins = mg;
    update();
}

DAColorPickerButton::ColorToolButtonStyle DAColorPickerButton::colorToolButtonStyle() const
{
    DA_DC(d);
    return d->mColorButtonStyle;
}

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

DAColorMenu* DAColorPickerButton::colorMenu() const
{
    return qobject_cast<DAColorMenu*>(menu());
}

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

void DAColorPickerButton::onButtonClicked(bool checked)
{
    DA_D(d);
    Q_EMIT colorClicked(d->mColor, checked);
}

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

void DAColorPickerButton::resizeEvent(QResizeEvent* event)
{
    QToolButton::resizeEvent(event);
    update();
}

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

void DAColorPickerButton::paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt)
{
    if (iconRect.isNull() || iconRect.isEmpty())
        return;
    if (!opt.icon.isNull()) {
        opt.icon.paint(p, iconRect);
    }
}

void DAColorPickerButton::paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt)
{
    if (textRect.isNull() || textRect.isEmpty())
        return;
    if (!opt.text.isEmpty()) {
        p->drawText(textRect, Qt::AlignCenter, opt.text);
    }
}

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
