#include "DACollapseLayout.h"

#include <QWidget>
#include <QSpacerItem>
#include <QWidgetItem>

namespace DA
{
/**
 * @brief 构造布局器
 * @param parent 宿主控件,可为 nullptr
 */
DACollapseLayout::DACollapseLayout(QWidget* parent) : QLayout(parent), m_dirty(true)
{
}

/** @brief 析构,释放所有 layout item */
DACollapseLayout::~DACollapseLayout()
{
    while (auto* item = takeAt(0))
        delete item;
}

/**
 * @brief 添加 layout item(末尾追加)
 * @param item 待添加项
 */
void DACollapseLayout::addItem(QLayoutItem* item)
{
    m_items.append(item);
    invalidate();
}

/** @brief 返回子项数量 */
int DACollapseLayout::count() const
{
    return m_items.size();
}

/**
 * @brief 获取指定索引的 layout item
 * @param index 索引
 * @return 索引有效时返回 item,否则返回 nullptr
 */
QLayoutItem* DACollapseLayout::itemAt(int index) const
{
    return (index >= 0 && index < m_items.size()) ? m_items.at(index) : nullptr;
}

/**
 * @brief 移除并返回指定索引的 layout item
 * @param index 索引
 * @return 索引有效时返回 item(调用方负责删除),否则返回 nullptr
 */
QLayoutItem* DACollapseLayout::takeAt(int index)
{
    if (index < 0 || index >= m_items.size())
        return nullptr;
    auto* item = m_items.takeAt(index);
    invalidate();
    return item;
}

/**
 * @brief 重新计算缓存的 sizeHint 和 minimumSize
 * @details 遍历所有 item,高度累加,宽度取最大值。
 *          SpacerItem(末尾 stretch)的高度按 sizeHint 累加,通常为 0。
 */
void DACollapseLayout::recalc() const
{
    if (!m_dirty)
        return;
    const int spacing = this->spacing();
    int h   = 0;
    int w   = 0;
    int minH = 0;
    for (int i = 0; i < m_items.size(); ++i) {
        if (i > 0) {
            h += spacing;
            minH += spacing;
        }
        const auto* item = m_items.at(i);
        h += item->sizeHint().height();
        w = qMax(w, item->sizeHint().width());
        minH += item->minimumSize().height();
        w = qMax(w, item->minimumSize().width());
    }
    auto margins   = contentsMargins();
    h += margins.top() + margins.bottom();
    w += margins.left() + margins.right();
    minH += margins.top() + margins.bottom();
    w = qMax(w, margins.left() + margins.right());
    m_sizeHint = QSize(w, h);
    m_minSize  = QSize(w, minH);
    m_dirty    = false;
}

/**
 * @brief 布局核心:从上到下依次给每个 item 分配 sizeHint 高度
 * @param rect 可用矩形区域
 * @details 不调用 qGeomCalc(),不做比例分配或压缩。
 *          每个 item 永远获得 sizeHint 高度,空间不足时溢出底部。
 */
void DACollapseLayout::setGeometry(const QRect& rect)
{
    QLayout::setGeometry(rect);
    auto margins = contentsMargins();
    const int x  = rect.x() + margins.left();
    const int w  = rect.width() - margins.left() - margins.right();
    const int sp = spacing();
    int y        = rect.y() + margins.top();

    for (int i = 0; i < m_items.size(); ++i) {
        if (i > 0)
            y += sp;
        QLayoutItem* item = m_items[i];
        const int h = item->sizeHint().height();
        item->setGeometry(QRect(x, y, w, h));
        y += h;
    }
}

/** @brief 推荐尺寸:所有 item sizeHint 高度之和 */
QSize DACollapseLayout::sizeHint() const
{
    recalc();
    return m_sizeHint;
}

/** @brief 最小尺寸:所有 item minimumSize 高度之和 */
QSize DACollapseLayout::minimumSize() const
{
    recalc();
    return m_minSize;
}

/** @brief 最大尺寸:不限制 */
QSize DACollapseLayout::maximumSize() const
{
    return QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

/** @brief 水平方向可扩展,垂直方向不可扩展 */
Qt::Orientations DACollapseLayout::expandingDirections() const
{
    return Qt::Horizontal;
}

/** @brief 不支持 heightForWidth 布局 */
bool DACollapseLayout::hasHeightForWidth() const
{
    return false;
}

/** @brief heightForWidth 返回 sizeHint 高度(不支持宽度依赖) */
int DACollapseLayout::heightForWidth(int /*width*/) const
{
    return sizeHint().height();
}

/** @brief 置 dirty 标志,下次查询时重新计算缓存 */
void DACollapseLayout::invalidate()
{
    m_dirty = true;
    QLayout::invalidate();
}

/**
 * @brief 在指定索引处插入控件
 * @param index 插入位置
 * @param widget 待插入控件
 * @details 创建 QWidgetItem 并插入到 m_items 的指定位置,
 *          同时调用 addChildWidget 完成 Qt 内部父子关系注册
 */
void DACollapseLayout::insertWidget(int index, QWidget* widget)
{
    addChildWidget(widget);
    const int clampedIndex = qBound(0, index, m_items.size());
    auto* item = new QWidgetItem(widget);
    m_items.insert(clampedIndex, item);
    invalidate();
}

/**
 * @brief 添加末尾弹簧
 * @param stretch 弹簧伸缩因子(0 时为扩展弹簧,占满剩余空间)
 */
void DACollapseLayout::addStretch(int stretch)
{
    auto* spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    if (stretch > 0)
        spacer->changeSize(0, stretch, QSizePolicy::Minimum, QSizePolicy::Fixed);
    addItem(spacer);
}
}  // namespace DA
