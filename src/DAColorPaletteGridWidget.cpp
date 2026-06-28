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

/**
 * @brief 构造默认Office风格色板
 * @param par 父控件
 * @details 使用内置10色Office主色列表初始化
 */
DAColorPaletteGridWidget::DAColorPaletteGridWidget(QWidget* par) : QWidget(par), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->mColors = getStandardColorList();
    init();
}

/**
 * @brief 构造自定义主色色板
 * @param cls 主色列表
 * @param par 父控件
 */
DAColorPaletteGridWidget::DAColorPaletteGridWidget(const QList<QColor>& cls, QWidget* par)
    : QWidget(par), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->mColors = cls;
    init();
}

/**
 * @brief 析构函数
 */
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

/**
 * @brief 设置主色列表
 * @param cls 主色列表
 * @details 同时重建主色行和调色板行
 */
void DAColorPaletteGridWidget::setColorList(const QList<QColor>& cls)
{
    DA_D(d);
    d->mColors = cls;
    d->mMainColorList->setColumnCount(0);  // 单行
    d->mMainColorList->setColorList(cls);
    d->mPaletteColorGrid->setColumnCount(cls.size());
    d->mPaletteColorGrid->setColorList(makeColorPalette(cls));
}

/**
 * @brief 获取主色列表
 * @return 主色列表
 */
QList<QColor> DAColorPaletteGridWidget::colorList() const
{
    DA_DC(d);
    return d->mColors;
}

/**
 * @brief 设置浅色/深色变体因子列表
 * @param factor lighter因子列表，前几个为浅色(>100)，后几个为深色(<100)
 * @details 会根据新因子重新生成调色板
 */
void DAColorPaletteGridWidget::setFactor(const QList<int>& factor)
{
    DA_D(d);
    d->mFactor = factor;
    d->mPaletteColorGrid->setColorList(makeColorPalette(d->mColors));
}

/**
 * @brief 获取浅色/深色变体因子列表
 * @return 因子列表
 */
QList<int> DAColorPaletteGridWidget::factor() const
{
    DA_DC(d);
    return d->mFactor;
}

/**
 * @brief 设置色块图标尺寸
 * @param s 图标尺寸
 * @details 同时设置主色行和调色板行的图标尺寸
 */
void DAColorPaletteGridWidget::setColorIconSize(const QSize& s)
{
    DA_D(d);
    d->mMainColorList->setColorIconSize(s);
    d->mPaletteColorGrid->setColorIconSize(s);
}

/**
 * @brief 获取色块图标尺寸
 * @return 图标尺寸
 */
QSize DAColorPaletteGridWidget::colorIconSize() const
{
    DA_DC(d);
    return d->mMainColorList->colorIconSize();
}

/**
 * @brief 设置色块是否可选中
 * @param on 是否可选中
 * @details 同时设置主色行和调色板行的checkable
 */
void DAColorPaletteGridWidget::setColorCheckable(bool on)
{
    DA_D(d);
    d->mMainColorList->setColorCheckable(on);
    d->mPaletteColorGrid->setColorCheckable(on);
}

/**
 * @brief 返回色块是否可选中
 * @return 是否可选中
 */
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
