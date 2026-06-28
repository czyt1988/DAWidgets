#include "DAColorPaletteGridWidget.h"

#include "DAColorGridWidget.h"

#include <QVBoxLayout>

namespace DA
{
/// Office 风格默认 10 色主色列表
static QList<QColor> getStandardColorList()
{
    return {
        QColor(192, 0, 0),    // 深红
        QColor(255, 0, 0),    // 红
        QColor(255, 192, 0),  // 橙
        QColor(255, 255, 0),  // 黄
        QColor(146, 208, 80), // 黄绿
        QColor(0, 176, 80),   // 绿
        QColor(0, 176, 240),  // 浅蓝
        QColor(0, 112, 192),  // 蓝
        QColor(0, 32, 96),    // 深蓝
        QColor(112, 48, 160), // 紫
    };
}

class DAColorPaletteGridWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAColorPaletteGridWidget)
public:
    PrivateData(DAColorPaletteGridWidget* p);
    QList<int> mFactor { 180, 160, 140, 75, 50 };  // 3 行浅色 + 2 行深色
    QVBoxLayout* mLayout { nullptr };
    DAColorGridWidget* mMainColorList { nullptr };
    DAColorGridWidget* mPaletteColorGrid { nullptr };
    QList<QColor> mColors;
};

DAColorPaletteGridWidget::PrivateData::PrivateData(DAColorPaletteGridWidget* p)
    : q_ptr(p)
{
    mLayout = new QVBoxLayout(p);
    mLayout->setContentsMargins(1, 1, 1, 1);
    mLayout->setSpacing(8);

    mMainColorList = new DAColorGridWidget(p);
    mMainColorList->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    mMainColorList->setColumnCount(0);  // 单行
    mLayout->addWidget(mMainColorList);

    mPaletteColorGrid = new DAColorGridWidget(p);
    mPaletteColorGrid->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    mPaletteColorGrid->setVerticalSpacing(0);
    mLayout->addWidget(mPaletteColorGrid);
}

//===================================================
// DAColorPaletteGridWidget
//===================================================

DAColorPaletteGridWidget::DAColorPaletteGridWidget(QWidget* par) : QWidget(par), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->mColors = getStandardColorList();
    init();
}

DAColorPaletteGridWidget::DAColorPaletteGridWidget(const QList<QColor>& cls, QWidget* par)
    : QWidget(par), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->mColors = cls;
    init();
}

DAColorPaletteGridWidget::~DAColorPaletteGridWidget()
{
}

void DAColorPaletteGridWidget::init()
{
    DA_D(d);
    setColorIconSize(QSize(10, 10));
    setColorList(d->mColors);

    connect(d->mMainColorList, &DAColorGridWidget::colorClicked,
            this, &DAColorPaletteGridWidget::onMainColorClicked);
    connect(d->mPaletteColorGrid, &DAColorGridWidget::colorClicked,
            this, &DAColorPaletteGridWidget::onPaletteColorClicked);
}

void DAColorPaletteGridWidget::setColorList(const QList<QColor>& cls)
{
    DA_D(d);
    d->mColors = cls;
    d->mMainColorList->setColumnCount(0);  // 单行
    d->mMainColorList->setColorList(cls);
    d->mPaletteColorGrid->setColumnCount(cls.size());
    d->mPaletteColorGrid->setColorList(makeColorPalette(cls));
}

QList<QColor> DAColorPaletteGridWidget::colorList() const
{
    DA_DC(d);
    return d->mColors;
}

void DAColorPaletteGridWidget::setFactor(const QList<int>& factor)
{
    DA_D(d);
    d->mFactor = factor;
    d->mPaletteColorGrid->setColorList(makeColorPalette(d->mColors));
}

QList<int> DAColorPaletteGridWidget::factor() const
{
    DA_DC(d);
    return d->mFactor;
}

void DAColorPaletteGridWidget::setColorIconSize(const QSize& s)
{
    DA_D(d);
    d->mMainColorList->setColorIconSize(s);
    d->mPaletteColorGrid->setColorIconSize(s);
}

QSize DAColorPaletteGridWidget::colorIconSize() const
{
    DA_DC(d);
    return d->mMainColorList->colorIconSize();
}

void DAColorPaletteGridWidget::setColorCheckable(bool on)
{
    DA_D(d);
    d->mMainColorList->setColorCheckable(on);
    d->mPaletteColorGrid->setColorCheckable(on);
}

bool DAColorPaletteGridWidget::isColorCheckable() const
{
    DA_DC(d);
    return d->mMainColorList->isColorCheckable();
}

QList<QColor> DAColorPaletteGridWidget::makeColorPalette(const QList<QColor>& cls) const
{
    DA_DC(d);
    QList<QColor> res;
    for (int f : d->mFactor) {
        for (const QColor& c : cls) {
            res.append(c.lighter(f));
        }
    }
    return res;
}

void DAColorPaletteGridWidget::onMainColorClicked(const QColor& c)
{
    DA_D(d);
    d->mPaletteColorGrid->clearCheckedState();
    Q_EMIT colorClicked(c);
}

void DAColorPaletteGridWidget::onPaletteColorClicked(const QColor& c)
{
    DA_D(d);
    d->mMainColorList->clearCheckedState();
    Q_EMIT colorClicked(c);
}
}  // namespace DA
