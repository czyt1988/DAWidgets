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
    updateGradientBrush();
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

void DAColorScalePreviewWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // 渐变端点跟随控件尺寸，尺寸变化时需重建
    updateGradientBrush();
}

/**
 * @brief 重建缓存渐变画刷
 *
 * 仅在颜色变化或尺寸变化时调用，绘制时直接用 mGradientBrush，避免在 paintEvent 中重复构造。
 * 渐变端点依赖控件尺寸（外框内缩 1px 的区域），因此尺寸变化同样需要重建。
 */
void DAColorScalePreviewWidget::updateGradientBrush()
{
    const bool valid = (mColors.size() == 2 || mColors.size() == 3)
                       && std::all_of(mColors.constBegin(), mColors.constEnd(), [](const QColor& c) { return c.isValid(); });
    mGradientValid = valid;
    if (!valid) {
        mGradientBrush = QBrush();
        return;
    }
    const QRect body = rect().adjusted(1, 1, -1, -1);
    QLinearGradient g(body.topLeft(), body.topRight());
    g.setColorAt(0.0, mColors.at(0));
    if (mColors.size() == 3) {
        g.setColorAt(0.5, mColors.at(1));
        g.setColorAt(1.0, mColors.at(2));
    } else {
        g.setColorAt(1.0, mColors.at(1));
    }
    mGradientBrush = QBrush(g);
}

void DAColorScalePreviewWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    const QRect body = rect().adjusted(1, 1, -1, -1);
    if (mGradientValid) {
        p.fillRect(body, mGradientBrush);
    } else {
        p.fillRect(body, palette().base());
    }
    p.setPen(palette().color(QPalette::Mid));
    p.drawRect(rect().adjusted(0, 0, -1, -1));
}

}  // namespace DA
