#include "DAShapeEditPannelWidget.h"
#include "ui_DAShapeEditPannelWidget.h"

#include "DABrushEditWidget.h"
#include "DAPenEditWidget.h"

namespace DA
{
class DAShapeEditPannelWidget::PrivateData
{
public:
    Ui::DAShapeEditPannelWidget ui;
};

DAShapeEditPannelWidget::DAShapeEditPannelWidget(QWidget* parent)
    : QWidget(parent), d(new PrivateData)
{
    d->ui.setupUi(this);
    connect(d->ui.widgetPenEdit, &DAPenEditWidget::penChanged, this, &DAShapeEditPannelWidget::borderPenChanged);
    connect(d->ui.widgetBrushEdit, &DABrushEditWidget::brushChanged, this, &DAShapeEditPannelWidget::backgroundBrushChanged);
}

DAShapeEditPannelWidget::~DAShapeEditPannelWidget()
{
    delete d;
}

QBrush DAShapeEditPannelWidget::getBackgroundBrush() const
{
    return d->ui.widgetBrushEdit->getCurrentBrush();
}

QPen DAShapeEditPannelWidget::getBorderPen() const
{
    return d->ui.widgetPenEdit->getCurrentPen();
}

void DAShapeEditPannelWidget::setBackgroundBrush(const QBrush& b)
{
    if (b == getBackgroundBrush()) {
        return;
    }
    d->ui.widgetBrushEdit->setCurrentBrush(b);
}

void DAShapeEditPannelWidget::setBorderPen(const QPen& v)
{
    d->ui.widgetPenEdit->setCurrentPen(v);
}
}  // namespace DA
