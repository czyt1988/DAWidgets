#include "DAFlowLayout.h"

#include <QWidget>
#include <QWidgetItem>

namespace DA
{
class DAFlowLayout::PrivateData
{
public:
    PrivateData(DAFlowLayout *) {}
    QList<QLayoutItem *> items;
    int hSpace { -1 };
    int vSpace { -1 };
    bool trailingExpanding { true };
};

DAFlowLayout::DAFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->hSpace = (hSpacing >= 0 ? hSpacing : -1);
    d->vSpace = (vSpacing >= 0 ? vSpacing : -1);
    setContentsMargins(margin, margin, margin, margin);
}

DAFlowLayout::DAFlowLayout(int margin, int hSpacing, int vSpacing)
    : DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->hSpace = (hSpacing >= 0 ? hSpacing : -1);
    d->vSpace = (vSpacing >= 0 ? vSpacing : -1);
    setContentsMargins(margin, margin, margin, margin);
}

DAFlowLayout::~DAFlowLayout()
{
    while (auto *item = takeAt(0))
        delete item;
}

void DAFlowLayout::addItem(QLayoutItem *item)
{
    DA_D(d);
    d->items.append(item);
}

void DAFlowLayout::insertWidget(int index, QWidget *widget)
{
    DA_D(d);
    addChildWidget(widget);
    if (index < 0)
        index = d->items.size();
    index = qBound(0, index, d->items.size());
    d->items.insert(index, new QWidgetItem(widget));
    invalidate();
}

int DAFlowLayout::horizontalSpacing() const
{
    DA_DC(d);
    return d->hSpace >= 0 ? d->hSpace : smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int DAFlowLayout::verticalSpacing() const
{
    DA_DC(d);
    return d->vSpace >= 0 ? d->vSpace : smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

int DAFlowLayout::count() const
{
    DA_DC(d);
    return d->items.size();
}

QLayoutItem *DAFlowLayout::itemAt(int index) const
{
    DA_DC(d);
    return d->items.value(index);
}

QLayoutItem *DAFlowLayout::takeAt(int index)
{
    DA_D(d);
    return (index >= 0 && index < d->items.size()) ? d->items.takeAt(index) : nullptr;
}

Qt::Orientations DAFlowLayout::expandingDirections() const
{
    return {};
}

bool DAFlowLayout::hasHeightForWidth() const
{
    return true;
}

int DAFlowLayout::heightForWidth(int width) const
{
    return doLayout(QRect(0, 0, width, 0), true);
}

void DAFlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize DAFlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize DAFlowLayout::minimumSize() const
{
    DA_DC(d);
    QSize size;
    for (auto *item : d->items)
        size = size.expandedTo(item->minimumSize());
    const auto m = contentsMargins();
    size += QSize(m.left() + m.right(), m.top() + m.bottom());
    return size;
}

void DAFlowLayout::setTrailingExpanding(bool on)
{
    DA_D(d);
    if (d->trailingExpanding == on)
        return;
    d->trailingExpanding = on;
    invalidate();
}

bool DAFlowLayout::trailingExpanding() const
{
    DA_DC(d);
    return d->trailingExpanding;
}

int DAFlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    DA_DC(d);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    const QRect eff = rect.adjusted(left, top, -right, -bottom);
    int x = eff.x();
    int y = eff.y();
    int lineHeight = 0;

    const int hsp = horizontalSpacing();
    const int vsp = verticalSpacing();
    const int n = d->items.size();

    for (int i = 0; i < n; ++i) {
        QLayoutItem *item = d->items.at(i);
        const bool trailing = d->trailingExpanding && (i == n - 1);

        if (trailing) {
            const int minW = item->minimumSize().width();
            const int hintH = item->sizeHint().height();
            int remaining = eff.right() - x; // 剩余宽度（不含尾部 hsp）
            if (remaining < minW && lineHeight > 0) {
                // 换行：占满整行宽度
                y = y + lineHeight + vsp;
                x = eff.x();
                lineHeight = 0;
                remaining = eff.width();
            }
            const int w = qMax(remaining, minW);
            const int h = qMax(hintH, item->minimumSize().height());
            if (!testOnly)
                item->setGeometry(QRect(x, y, w, h));
            x += w + hsp;
            lineHeight = qMax(lineHeight, h);
            continue;
        }

        const QSize is = item->sizeHint();
        int nextX = x + is.width() + hsp;
        if (nextX - hsp > eff.right() && lineHeight > 0) {
            x = eff.x();
            y = y + lineHeight + vsp;
            nextX = x + is.width() + hsp;
            lineHeight = 0;
        }
        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), is));
        x = nextX;
        lineHeight = qMax(lineHeight, is.height());
    }
    return y + lineHeight - rect.y() + bottom;
}

int DAFlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *p = parent();
    if (!p)
        return -1;
    if (p->isWidgetType()) {
        auto *w = static_cast<QWidget *>(p);
        return w->style()->pixelMetric(pm, nullptr, w);
    }
    return static_cast<QLayout *>(p)->spacing();
}
}  // namespace DA
