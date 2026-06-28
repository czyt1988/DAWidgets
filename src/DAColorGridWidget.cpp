#include "DAColorGridWidget.h"

#include <QGridLayout>
#include <QButtonGroup>
#include <QToolButton>
#include <QSpacerItem>
#include <QPainter>
#include <QPixmap>
#include <QVariant>

namespace DA
{
class DAColorGridWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAColorGridWidget)
public:
    PrivateData(DAColorGridWidget* p);
    QList<QColor> mColors;
    QGridLayout* mGridLayout { nullptr };
    QButtonGroup* mButtonGroup { nullptr };
    QSize mIconSize { 16, 16 };
    int mColumnCount { 8 };
    bool mColorCheckable { true };
    bool mHorizontalSpacerToRight { false };
};

DAColorGridWidget::PrivateData::PrivateData(DAColorGridWidget* p)
    : q_ptr(p)
{
    mGridLayout = new QGridLayout(p);
    mGridLayout->setSpacing(0);
    mGridLayout->setContentsMargins(1, 1, 1, 1);
    mButtonGroup = new QButtonGroup(p);
    mButtonGroup->setExclusive(true);
}

//===================================================
// DAColorGridWidget
//===================================================

DAColorGridWidget::DAColorGridWidget(QWidget* par) : QWidget(par), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    connect(d->mButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, [this](QAbstractButton* btn) {
        auto* tb = qobject_cast<QToolButton*>(btn);
        if (tb) {
            QColor c = tb->property("color").value<QColor>();
            Q_EMIT colorClicked(c);
        }
    });
    connect(d->mButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonPressed),
            this, [this](QAbstractButton* btn) {
        auto* tb = qobject_cast<QToolButton*>(btn);
        if (tb) {
            QColor c = tb->property("color").value<QColor>();
            Q_EMIT colorPressed(c);
        }
    });
    connect(d->mButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonReleased),
            this, [this](QAbstractButton* btn) {
        auto* tb = qobject_cast<QToolButton*>(btn);
        if (tb) {
            QColor c = tb->property("color").value<QColor>();
            Q_EMIT colorReleased(c);
        }
    });
}

DAColorGridWidget::~DAColorGridWidget()
{
}

void DAColorGridWidget::setColumnCount(int c)
{
    DA_D(d);
    d->mColumnCount = c;
    updateGridColor();
}

int DAColorGridWidget::columnCount() const
{
    DA_DC(d);
    return d->mColumnCount;
}

void DAColorGridWidget::setColorList(const QList<QColor>& cls)
{
    DA_D(d);
    d->mColors = cls;
    updateGridColor();
}

QList<QColor> DAColorGridWidget::getColorList() const
{
    DA_DC(d);
    return d->mColors;
}

int DAColorGridWidget::spacing() const
{
    DA_DC(d);
    return d->mGridLayout->spacing();
}

void DAColorGridWidget::setSpacing(int v)
{
    DA_D(d);
    d->mGridLayout->setSpacing(v);
}

int DAColorGridWidget::colorCount() const
{
    DA_DC(d);
    return d->mColors.size();
}

void DAColorGridWidget::setColorIconSize(const QSize& s)
{
    DA_D(d);
    d->mIconSize = s;
    iterationColorBtns([s](QToolButton* btn) { btn->setIconSize(s); });
    d->mGridLayout->setRowMinimumHeight(0, s.height());
}

QSize DAColorGridWidget::colorIconSize() const
{
    DA_DC(d);
    return d->mIconSize;
}

void DAColorGridWidget::setColorCheckable(bool on)
{
    DA_D(d);
    d->mColorCheckable = on;
    iterationColorBtns([on](QToolButton* btn) { btn->setCheckable(on); });
}

bool DAColorGridWidget::isColorCheckable() const
{
    DA_DC(d);
    return d->mColorCheckable;
}

QColor DAColorGridWidget::currentCheckedColor() const
{
    DA_DC(d);
    auto* btn = qobject_cast<QToolButton*>(d->mButtonGroup->checkedButton());
    if (btn)
        return btn->property("color").value<QColor>();
    return QColor();
}

QToolButton* DAColorGridWidget::colorButton(int index) const
{
    DA_DC(d);
    // 遍历布局找到第 index 个 button
    int count = 0;
    for (int i = 0; i < d->mGridLayout->count(); ++i) {
        auto* item = d->mGridLayout->itemAt(i);
        if (item && item->widget()) {
            auto* btn = qobject_cast<QToolButton*>(item->widget());
            if (btn) {
                if (count == index)
                    return btn;
                ++count;
            }
        }
    }
    return nullptr;
}

void DAColorGridWidget::setVerticalSpacing(int v)
{
    DA_D(d);
    d->mGridLayout->setVerticalSpacing(v);
}

int DAColorGridWidget::verticalSpacing() const
{
    DA_DC(d);
    return d->mGridLayout->verticalSpacing();
}

void DAColorGridWidget::setHorizontalSpacing(int v)
{
    DA_D(d);
    d->mGridLayout->setHorizontalSpacing(v);
}

int DAColorGridWidget::horizontalSpacing() const
{
    DA_DC(d);
    return d->mGridLayout->horizontalSpacing();
}

void DAColorGridWidget::clearCheckedState()
{
    DA_D(d);
    if (d->mButtonGroup->exclusive()) {
        d->mButtonGroup->setExclusive(false);
        iterationColorBtns([](QToolButton* btn) { btn->setChecked(false); });
        d->mButtonGroup->setExclusive(true);
    } else {
        iterationColorBtns([](QToolButton* btn) {
            if (btn->isChecked())
                btn->setChecked(false);
        });
    }
}

void DAColorGridWidget::iterationColorBtns(FunColorBtn fn)
{
    DA_D(d);
    for (int i = 0; i < d->mGridLayout->count(); ++i) {
        auto* item = d->mGridLayout->itemAt(i);
        if (item && item->widget()) {
            auto* btn = qobject_cast<QToolButton*>(item->widget());
            if (btn)
                fn(btn);
        }
    }
}

void DAColorGridWidget::setRowMinimumHeight(int row, int minSize)
{
    DA_D(d);
    d->mGridLayout->setRowMinimumHeight(row, minSize);
}

void DAColorGridWidget::setHorizontalSpacerToRight(bool on)
{
    DA_D(d);
    if (on) {
        if (!d->mHorizontalSpacerToRight) {
            auto* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            d->mGridLayout->addItem(spacer, 0, d->mColumnCount, 1, 1);
            d->mHorizontalSpacerToRight = true;
        }
    } else {
        if (d->mHorizontalSpacerToRight) {
            d->mGridLayout->removeItem(d->mGridLayout->itemAtPosition(0, d->mColumnCount));
            d->mHorizontalSpacerToRight = false;
        }
    }
}

QSize DAColorGridWidget::sizeHint() const
{
    DA_DC(d);
    return d->mGridLayout->sizeHint();
}

void DAColorGridWidget::updateGridColor(bool isRemoveSpacer)
{
    DA_D(d);
    int col = d->mColumnCount;
    if (col <= 0)
        col = d->mColors.size();
    if (col <= 0)
        col = 1;

    const int n = d->mColors.size();
    for (int i = 0; i < n; ++i) {
        int r = i / col;
        int c = i % col;
        setColorAt(d->mColors[i], r, c);
    }
    // 移除多余的 button
    int extraStart = n;
    while (true) {
        int r = extraStart / col;
        int c = extraStart % col;
        auto* item = d->mGridLayout->itemAtPosition(r, c);
        if (!item)
            break;
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        d->mGridLayout->removeItem(item);
        delete item;
        ++extraStart;
    }
}

void DAColorGridWidget::setColorAt(const QColor& clr, int r, int c)
{
    DA_D(d);
    auto* item = d->mGridLayout->itemAtPosition(r, c);
    QToolButton* btn = nullptr;
    if (item && item->widget()) {
        btn = qobject_cast<QToolButton*>(item->widget());
    }
    if (!btn) {
        btn = new QToolButton(this);
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        btn->setIconSize(d->mIconSize);
        btn->setAutoRaise(true);
        btn->setCheckable(d->mColorCheckable);
        d->mButtonGroup->addButton(btn, r * 1000 + c);  // 唯一 id
        d->mGridLayout->addWidget(btn, r, c);
    }
    btn->setProperty("color", QVariant::fromValue(clr));
    btn->setIcon(makeColorIcon(clr, d->mIconSize));
}

QIcon DAColorGridWidget::makeColorIcon(const QColor& clr, const QSize& size)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    if (clr.isValid()) {
        painter.fillRect(QRect(QPoint(0, 0), size), clr);
    } else {
        // 无效颜色:绘制禁止符号
        QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        int ss = size.width() / 3;
        QRect r(QPoint(0, 0), size);
        painter.drawLine(QPoint(r.x() + ss, r.bottom()), QPoint(r.right() - ss, r.top()));
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawRect(r.adjusted(0, 0, -1, -1));
    }
    return QIcon(pixmap);
}
}  // namespace DA
