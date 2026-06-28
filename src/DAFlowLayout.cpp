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

/**
 * @brief 构造流式布局并挂载到父控件
 * @param parent 父控件
 * @param margin 外边距，-1 表示使用 style 默认值
 * @param hSpacing 水平间距，-1 表示使用 style 默认值
 * @param vSpacing 垂直间距，-1 表示使用 style 默认值
 */
DAFlowLayout::DAFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->hSpace = (hSpacing >= 0 ? hSpacing : -1);
    d->vSpace = (vSpacing >= 0 ? vSpacing : -1);
    setContentsMargins(margin, margin, margin, margin);
}

/**
 * @brief 构造流式布局（无父控件）
 * @param margin 外边距，-1 表示使用 style 默认值
 * @param hSpacing 水平间距，-1 表示使用 style 默认值
 * @param vSpacing 垂直间距，-1 表示使用 style 默认值
 */
DAFlowLayout::DAFlowLayout(int margin, int hSpacing, int vSpacing)
    : DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->hSpace = (hSpacing >= 0 ? hSpacing : -1);
    d->vSpace = (vSpacing >= 0 ? vSpacing : -1);
    setContentsMargins(margin, margin, margin, margin);
}

/**
 * @brief 析构，释放所有布局项
 */
DAFlowLayout::~DAFlowLayout()
{
    while (auto *item = takeAt(0))
        delete item;
}

/**
 * @brief 添加布局项到末尾
 * @param item 布局项
 */
void DAFlowLayout::addItem(QLayoutItem *item)
{
    DA_D(d);
    d->items.append(item);
}

/**
 * @brief 在指定位置插入控件
 * @param index 插入位置，负数追加到末尾
 * @param widget 要插入的控件
 * @details 负索引自动修正为末尾，越界索引被 qBound 钳制到合法范围
 */
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

/**
 * @brief 获取水平间距
 * @return 间距值，-1 时回退到 style 默认值
 */
int DAFlowLayout::horizontalSpacing() const
{
    DA_DC(d);
    return d->hSpace >= 0 ? d->hSpace : smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

/**
 * @brief 获取垂直间距
 * @return 间距值，-1 时回退到 style 默认值
 */
int DAFlowLayout::verticalSpacing() const
{
    DA_DC(d);
    return d->vSpace >= 0 ? d->vSpace : smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

/**
 * @brief 获取布局项数量
 * @return 布局项数量
 */
int DAFlowLayout::count() const
{
    DA_DC(d);
    return d->items.size();
}

/**
 * @brief 获取指定索引的布局项
 * @param index 索引
 * @return 布局项指针，越界返回 nullptr
 */
QLayoutItem *DAFlowLayout::itemAt(int index) const
{
    DA_DC(d);
    return d->items.value(index);
}

/**
 * @brief 移除并返回指定索引的布局项
 * @param index 索引
 * @return 被移除的布局项指针，越界返回 nullptr
 */
QLayoutItem *DAFlowLayout::takeAt(int index)
{
    DA_D(d);
    return (index >= 0 && index < d->items.size()) ? d->items.takeAt(index) : nullptr;
}

/**
 * @brief 返回布局可扩展的方向
 * @return 空方向集——流式布局不主动扩展
 */
Qt::Orientations DAFlowLayout::expandingDirections() const
{
    return {};
}

/**
 * @brief 是否支持 height-for-width 布局
 * @return 固定返回 true——流式布局高度取决于可用宽度
 */
bool DAFlowLayout::hasHeightForWidth() const
{
    return true;
}

/**
 * @brief 根据给定宽度计算所需高度
 * @param width 可用宽度
 * @return 该宽度下排列所有子项所需的高度
 * @details 以 testOnly 模式调用 doLayout 进行测量计算
 */
int DAFlowLayout::heightForWidth(int width) const
{
    return doLayout(QRect(0, 0, width, 0), true);
}

/**
 * @brief 执行布局，排列所有子项
 * @param rect 布局区域
 * @details 先调用 QLayout::setGeometry 处理缓存，再以非测试模式调用 doLayout 实际排列
 */
void DAFlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

/**
 * @brief 获取推荐尺寸
 * @return 与 minimumSize 相同
 */
QSize DAFlowLayout::sizeHint() const
{
    return minimumSize();
}

/**
 * @brief 获取最小尺寸
 * @return 所有子项最小尺寸的扩展最大值加上 contentsMargins
 */
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

/**
 * @brief 设置末尾项是否占满剩余宽度
 * @param on 是否启用
 * @details 用于 DAChipsEdit 末尾内嵌输入框自适应宽度。变化时调用 invalidate 刷新布局缓存
 */
void DAFlowLayout::setTrailingExpanding(bool on)
{
    DA_D(d);
    if (d->trailingExpanding == on)
        return;
    d->trailingExpanding = on;
    invalidate();
}

/**
 * @brief 获取末尾项是否占满剩余宽度
 * @return 启用返回 true
 */
bool DAFlowLayout::trailingExpanding() const
{
    DA_DC(d);
    return d->trailingExpanding;
}

/**
 * @brief 核心流式排列算法
 * @param rect 可用布局区域
 * @param testOnly true 时仅计算高度不实际 setGeometry，false 时执行排列
 * @return 排列后所需总高度
 * @details 逐项按行排列，超出右边界自动换行。末尾项在 trailingExpanding 开启时
 *          占满当前行剩余宽度，不足最小宽度则换行占满整行
 */
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

/**
 * @brief 从父控件或父布局获取 style 默认间距
 * @param pm 要查询的 PixelMetric
 * @return 间距值，无父对象返回 -1
 * @details 父对象为控件时查询其 style，为布局时返回 layout spacing
 */
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
