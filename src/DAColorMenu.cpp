#include "DAColorMenu.h"

#include "DAColorPaletteGridWidget.h"
#include "DAColorGridWidget.h"

#include <QAction>
#include <QLabel>
#include <QWidgetAction>
#include <QColorDialog>
#include <QPainter>
#include <QPixmap>
#include <QPointer>

namespace DA
{
class DAColorMenu::PrivateData
{
    DA_DECLARE_PUBLIC(DAColorMenu)
public:
    PrivateData(DAColorMenu* p);
    QLabel* mTitleLabel { nullptr };
    QWidgetAction* mTitleLabelAction { nullptr };
    DAColorPaletteGridWidget* mThemeColorsWidget { nullptr };
    QWidgetAction* mThemeColorPaletteAction { nullptr };
    QAction* mCustomColorAction { nullptr };
    DAColorGridWidget* mCustomColorsWidget { nullptr };
    QWidgetAction* mCustomColorsWidgetAction { nullptr };
    QAction* mNoneColorAction { nullptr };
    QList<QColor> mCustomColors;
    int mMaxCustomColorSize { 10 };
    QPointer<QColorDialog> mColorDlg;
};

DAColorMenu::PrivateData::PrivateData(DAColorMenu* p) : q_ptr(p)
{
}

//===================================================
// DAColorMenu
//===================================================

DAColorMenu::DAColorMenu(QWidget* parent) : QMenu(parent), DA_PIMPL_CONSTRUCT
{
    // 默认主题色由 DAColorPaletteGridWidget 构造函数提供
    init({});
}

DAColorMenu::DAColorMenu(const QString& title, QWidget* parent) : QMenu(title, parent), DA_PIMPL_CONSTRUCT
{
    init({});
}

DAColorMenu::~DAColorMenu()
{
}

void DAColorMenu::init(const QList<QColor>& themeCls)
{
    DA_D(d);
    // 标题
    d->mTitleLabel = new QLabel(QStringLiteral("Theme Colors"), this);
    d->mTitleLabelAction = new QWidgetAction(this);
    d->mTitleLabelAction->setDefaultWidget(d->mTitleLabel);
    addAction(d->mTitleLabelAction);

    // 主题色板
    if (themeCls.isEmpty())
        d->mThemeColorsWidget = new DAColorPaletteGridWidget(this);
    else
        d->mThemeColorsWidget = new DAColorPaletteGridWidget(themeCls, this);
    d->mThemeColorsWidget->setColorCheckable(false);
    d->mThemeColorPaletteAction = new QWidgetAction(this);
    d->mThemeColorPaletteAction->setDefaultWidget(d->mThemeColorsWidget);
    addAction(d->mThemeColorPaletteAction);

    // Custom Color action
    d->mCustomColorAction = new QAction(QStringLiteral("Custom Color"), this);
    addAction(d->mCustomColorAction);

    // 自定义颜色记录网格
    d->mCustomColorsWidget = new DAColorGridWidget(this);
    d->mCustomColorsWidget->setRowMinimumHeight(0, d->mThemeColorsWidget->colorIconSize().height());
    d->mCustomColorsWidget->setHorizontalSpacerToRight(true);
    d->mCustomColorsWidget->setColorIconSize(d->mThemeColorsWidget->colorIconSize());
    d->mCustomColorsWidget->setColumnCount(d->mMaxCustomColorSize);
    d->mCustomColorsWidget->setColorCheckable(false);
    d->mCustomColorsWidgetAction = new QWidgetAction(this);
    d->mCustomColorsWidgetAction->setDefaultWidget(d->mCustomColorsWidget);
    addAction(d->mCustomColorsWidgetAction);

    // 连线
    connect(d->mCustomColorAction, &QAction::triggered, this, &DAColorMenu::onCustomColorActionTriggered);
    connect(d->mThemeColorsWidget, &DAColorPaletteGridWidget::colorClicked, this, &DAColorMenu::emitSelectedColor);
    connect(d->mCustomColorsWidget, &DAColorGridWidget::colorClicked, this, &DAColorMenu::emitSelectedColor);
}

QWidgetAction* DAColorMenu::themeColorsPaletteAction() const
{
    DA_DC(d);
    return d->mThemeColorPaletteAction;
}

QWidgetAction* DAColorMenu::getCustomColorsWidgetAction() const
{
    DA_DC(d);
    return d->mCustomColorsWidgetAction;
}

QAction* DAColorMenu::customColorAction() const
{
    DA_DC(d);
    return d->mCustomColorAction;
}

DAColorPaletteGridWidget* DAColorMenu::colorPaletteGridWidget() const
{
    DA_DC(d);
    return d->mThemeColorsWidget;
}

DAColorGridWidget* DAColorMenu::customColorsWidget() const
{
    DA_DC(d);
    return d->mCustomColorsWidget;
}

void DAColorMenu::enableNoneColorAction(bool on)
{
    DA_D(d);
    if (on) {
        if (!d->mNoneColorAction) {
            d->mNoneColorAction = new QAction(createNoneColorIcon(), QStringLiteral("None"), this);
            connect(d->mNoneColorAction, &QAction::triggered, this, &DAColorMenu::onNoneColorActionTriggered);
            // 插入到 Custom Color action 之前
            insertAction(d->mCustomColorAction, d->mNoneColorAction);
        }
    } else {
        if (d->mNoneColorAction) {
            removeAction(d->mNoneColorAction);
        }
    }
}

QAction* DAColorMenu::noneColorAction() const
{
    DA_DC(d);
    return d->mNoneColorAction;
}

void DAColorMenu::emitSelectedColor(const QColor& c)
{
    Q_EMIT selectedColor(c);
    hide();
}

void DAColorMenu::onCustomColorActionTriggered(bool)
{
    DA_D(d);
    QColor c = getColorByDialog();
    if (c.isValid()) {
        recordCustomColor(c);
        d->mCustomColorsWidget->setColorList(d->mCustomColors);
        d->mCustomColorsWidget->updateGeometry();
        emitSelectedColor(c);
    }
}

void DAColorMenu::onNoneColorActionTriggered(bool)
{
    emitSelectedColor(QColor());  // 无效颜色表示"无颜色"
}

QColor DAColorMenu::getColorByDialog()
{
    DA_D(d);
    if (!d->mColorDlg) {
        d->mColorDlg = new QColorDialog(this);
        d->mColorDlg->setOption(QColorDialog::ShowAlphaChannel, true);
    }
    if (d->mColorDlg->exec() == QDialog::Accepted) {
        return d->mColorDlg->currentColor();
    }
    return QColor();
}

void DAColorMenu::recordCustomColor(const QColor& c)
{
    DA_D(d);
    if (d->mCustomColors.size() < d->mMaxCustomColorSize) {
        d->mCustomColors.append(c);
    } else {
        // FIFO 淘汰最旧
        for (int i = 0; i < d->mCustomColors.size() - 1; ++i) {
            d->mCustomColors[i] = d->mCustomColors[i + 1];
        }
        d->mCustomColors.back() = c;
    }
}

QIcon DAColorMenu::createNoneColorIcon()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QRect r = pixmap.rect().adjusted(1, 1, -1, -1);
    // 红色对角线
    QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(pen);
    int ss = r.width() / 3;
    painter.drawLine(QPoint(r.x() + ss, r.bottom()), QPoint(r.right() - ss, r.top()));
    // 黑色边框
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawRect(r);
    return QIcon(pixmap);
}
}  // namespace DA
