#include "DAColorScalePreviewWidget.h"
#include <QPainter>
#include <QLinearGradient>
#include <QPaintEvent>
#include <algorithm>

namespace DA
{

DAColorScalePreviewWidget::DAColorScalePreviewWidget(QWidget* parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void DAColorScalePreviewWidget::setColors(const QList< QColor >& colors)
{
    if (mColors == colors) {
        return;
    }
    mColors = colors;
    update();
    Q_EMIT colorsChanged(mColors);
}

QList< QColor > DAColorScalePreviewWidget::colors() const
{
    return mColors;
}

QSize DAColorScalePreviewWidget::sizeHint() const
{
    return QSize(160, 22);
}

QSize DAColorScalePreviewWidget::minimumSizeHint() const
{
    return QSize(48, 16);
}

void DAColorScalePreviewWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    const QRect body = rect().adjusted(1, 1, -1, -1);
    const bool valid = (mColors.size() == 2 || mColors.size() == 3) && std::all_of(
        mColors.constBegin(), mColors.constEnd(), [](const QColor& c) { return c.isValid(); });
    if (valid) {
        QLinearGradient g(body.topLeft(), body.topRight());
        g.setColorAt(0.0, mColors.at(0));
        if (mColors.size() == 3) {
            g.setColorAt(0.5, mColors.at(1));
            g.setColorAt(1.0, mColors.at(2));
        } else {
            g.setColorAt(1.0, mColors.at(1));
        }
        p.fillRect(body, g);
    } else {
        p.fillRect(body, palette().base());
    }
    p.setPen(palette().color(QPalette::Mid));
    p.drawRect(rect().adjusted(0, 0, -1, -1));
}

}  // namespace DA
