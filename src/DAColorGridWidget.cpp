#include "DAColorGridWidget.h"

#include <QBrush>
#include <QEvent>
#include <QGridLayout>
#include <QButtonGroup>
#include <QKeyEvent>
#include <QToolButton>
#include <QSpacerItem>
#include <QPainter>
#include <QPixmap>
#include <QVariant>

namespace DA
{
namespace
{
// 16×16、8px 见方的棋盘格（#E6E9EE / #FFFFFF）
QPixmap makeCheckerPixmap(int cell = 8)
{
    QPixmap pm(cell * 2, cell * 2);
    QPainter p(&pm);
    p.fillRect(QRect(0, 0, cell, cell), QColor(0xE6, 0xE9, 0xEE));
    p.fillRect(QRect(cell, 0, cell, cell), Qt::white);
    p.fillRect(QRect(0, cell, cell, cell), Qt::white);
    p.fillRect(QRect(cell, cell, cell, cell), QColor(0xE6, 0xE9, 0xEE));
    p.end();
    return pm;
}
}  // namespace

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
    bool mKeyboardNavigation { false };
    bool mAlphaBackground { false };
    QPixmap mCheckerPixmap;
};

DAColorGridWidget::PrivateData::PrivateData(DAColorGridWidget* p)
    : q_ptr(p)
{
    mGridLayout = new QGridLayout(p);
    mGridLayout->setSpacing(0);
    mGridLayout->setContentsMargins(1, 1, 1, 1);
    mButtonGroup = new QButtonGroup(p);
    mButtonGroup->setExclusive(true);
    mCheckerPixmap = makeCheckerPixmap();
}

//===================================================
// DAColorGridWidget
//===================================================

/**
 * @brief 构造颜色网格控件
 * @param par 父控件
 * @details 创建QGridLayout+QButtonGroup(exclusive)，连接按钮组的点击/按下/释放信号
 */
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

/**
 * @brief 析构函数
 */
DAColorGridWidget::~DAColorGridWidget()
{
}

/**
 * @brief 设置网格列数
 * @param c 列数
 * @details 颜色按钮按行优先排列，改变列数会触发网格重建
 */
void DAColorGridWidget::setColumnCount(int c)
{
    DA_D(d);
    d->mColumnCount = c;
    updateGridColor();
}

/**
 * @brief 获取网格列数
 * @return 当前列数
 */
int DAColorGridWidget::columnCount() const
{
    DA_DC(d);
    return d->mColumnCount;
}

/**
 * @brief 设置颜色列表
 * @param cls 颜色列表
 * @details 会触发网格重建，原有按钮会被回收或复用
 */
void DAColorGridWidget::setColorList(const QList<QColor>& cls)
{
    DA_D(d);
    d->mColors = cls;
    updateGridColor();
}

/**
 * @brief 获取当前颜色列表
 * @return 颜色列表
 */
QList<QColor> DAColorGridWidget::getColorList() const
{
    DA_DC(d);
    return d->mColors;
}

/**
 * @brief 获取颜色间距
 * @return 间距像素值
 */
int DAColorGridWidget::spacing() const
{
    DA_DC(d);
    return d->mGridLayout->spacing();
}

/**
 * @brief 设置颜色间距（水平和垂直统一）
 * @param v 间距像素值
 */
void DAColorGridWidget::setSpacing(int v)
{
    DA_D(d);
    d->mGridLayout->setSpacing(v);
}

/**
 * @brief 获取颜色数量
 * @return 颜色列表的大小
 */
int DAColorGridWidget::colorCount() const
{
    DA_DC(d);
    return d->mColors.size();
}

/**
 * @brief 设置色块图标尺寸
 * @param s 图标尺寸
 * @details 遍历所有按钮更新图标大小，并设置第一行的最小高度
 */
void DAColorGridWidget::setColorIconSize(const QSize& s)
{
    DA_D(d);
    d->mIconSize = s;
    iterationColorBtns([s](QToolButton* btn) { btn->setIconSize(s); });
    d->mGridLayout->setRowMinimumHeight(0, s.height());
}

/**
 * @brief 获取色块图标尺寸
 * @return 图标尺寸
 */
QSize DAColorGridWidget::colorIconSize() const
{
    DA_DC(d);
    return d->mIconSize;
}

/**
 * @brief 设置色块是否可选中
 * @param on 是否可选中
 * @details 遍历所有按钮设置checkable属性
 */
void DAColorGridWidget::setColorCheckable(bool on)
{
    DA_D(d);
    d->mColorCheckable = on;
    iterationColorBtns([on](QToolButton* btn) { btn->setCheckable(on); });
}

/**
 * @brief 返回色块是否可选中
 * @return 是否可选中
 */
bool DAColorGridWidget::isColorCheckable() const
{
    DA_DC(d);
    return d->mColorCheckable;
}

/**
 * @brief 获取当前被选中的颜色
 * @return 被选中的颜色，无选中项返回无效颜色
 * @details 基于QButtonGroup的checkedButton查询
 */
QColor DAColorGridWidget::currentCheckedColor() const
{
    DA_DC(d);
    auto* btn = qobject_cast<QToolButton*>(d->mButtonGroup->checkedButton());
    if (btn)
        return btn->property("color").value<QColor>();
    return QColor();
}

/**
 * @brief 根据索引获取色块按钮
 * @param index 按钮索引（按布局顺序）
 * @return 对应的QToolButton，索引越界返回nullptr
 */
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

/**
 * @brief 设置垂直间距
 * @param v 垂直间距像素值
 */
void DAColorGridWidget::setVerticalSpacing(int v)
{
    DA_D(d);
    d->mGridLayout->setVerticalSpacing(v);
}

/**
 * @brief 获取垂直间距
 * @return 垂直间距像素值
 */
int DAColorGridWidget::verticalSpacing() const
{
    DA_DC(d);
    return d->mGridLayout->verticalSpacing();
}

/**
 * @brief 设置水平间距
 * @param v 水平间距像素值
 */
void DAColorGridWidget::setHorizontalSpacing(int v)
{
    DA_D(d);
    d->mGridLayout->setHorizontalSpacing(v);
}

/**
 * @brief 获取水平间距
 * @return 水平间距像素值
 */
int DAColorGridWidget::horizontalSpacing() const
{
    DA_DC(d);
    return d->mGridLayout->horizontalSpacing();
}

/**
 * @brief 清除所有色块的选中状态
 * @details 临时取消互斥模式以清除选中，然后恢复互斥模式
 */
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

/**
 * @brief 遍历所有色块按钮并执行回调
 * @param fn 对每个按钮执行的回调函数
 */
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

/**
 * @brief 设置指定行的最小高度
 * @param row 行号
 * @param minSize 最小高度
 */
void DAColorGridWidget::setRowMinimumHeight(int row, int minSize)
{
    DA_D(d);
    d->mGridLayout->setRowMinimumHeight(row, minSize);
}

/**
 * @brief 在右侧添加水平弹簧，使色块左对齐
 * @param on 是否添加弹簧
 * @details true时在第0行末列添加Expanding弹簧，false时移除已有弹簧
 */
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

/**
 * @brief 推荐尺寸
 * @return 基于网格布局的推荐尺寸
 */
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
        btn->installEventFilter(this);
    }
    btn->setProperty("color", QVariant::fromValue(clr));
    btn->setIcon(d->mAlphaBackground ? makeColorIconWithAlpha(clr, d->mIconSize)
                                     : makeColorIcon(clr, d->mIconSize));
    if (d->mKeyboardNavigation)
        applyFocusPolicies();
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

/**
 * @brief 带棋盘格底的色块图标（仅半透明色需要）
 * @param clr 颜色
 * @param size 图标尺寸
 * @return 图标；不透明色与 makeColorIcon() 逐像素一致
 */
QIcon DAColorGridWidget::makeColorIconWithAlpha(const QColor& clr, const QSize& size) const
{
    DA_DC(d);
    if (!clr.isValid() || clr.alpha() == 255)
        return makeColorIcon(clr, size);
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.fillRect(QRect(QPoint(0, 0), size), QBrush(d->mCheckerPixmap));
    painter.fillRect(QRect(QPoint(0, 0), size), clr);
    painter.end();
    return QIcon(pixmap);
}

/** @brief 开启/关闭色块键盘导航（默认关闭） */
void DAColorGridWidget::setKeyboardNavigationEnabled(bool on)
{
    DA_D(d);
    if (d->mKeyboardNavigation == on)
        return;
    d->mKeyboardNavigation = on;
    applyFocusPolicies();
}

/** @brief 开启/关闭棋盘格底（默认关闭） */
void DAColorGridWidget::setAlphaBackgroundEnabled(bool on)
{
    DA_D(d);
    if (d->mAlphaBackground == on)
        return;
    d->mAlphaBackground = on;
    updateGridColor();
}

/** @brief columnCount()==0 时的单行规则 */
int DAColorGridWidget::effectiveColumns() const
{
    DA_DC(d);
    int col = d->mColumnCount;
    if (col <= 0)
        col = d->mColors.size();
    if (col <= 0)
        col = 1;
    return col;
}

/**
 * @brief 应用焦点策略
 * @details 开启时只让首块进 Tab 链（其余 ClickFocus），关闭时恢复 QToolButton 默认 StrongFocus
 */
void DAColorGridWidget::applyFocusPolicies()
{
    DA_D(d);
    const int n = d->mColors.size();
    for (int i = 0; i < n; ++i) {
        QToolButton* btn = colorButton(i);
        if (!btn)
            continue;
        btn->setFocusPolicy(d->mKeyboardNavigation ? (i == 0 ? Qt::StrongFocus : Qt::ClickFocus)
                                                   : Qt::StrongFocus);
    }
}

/**
 * @brief 色块键盘导航：方向键移动、Enter 取色
 * @details 行列数从 effectiveColumns() 读取，不写死；越界时停在原块并吞掉按键
 */
bool DAColorGridWidget::eventFilter(QObject* watched, QEvent* event)
{
    DA_D(d);
    if (!d->mKeyboardNavigation || event->type() != QEvent::KeyPress)
        return QWidget::eventFilter(watched, event);
    auto* btn = qobject_cast< QToolButton* >(watched);
    if (!btn)
        return QWidget::eventFilter(watched, event);
    const int n = d->mColors.size();
    int idx = -1;
    for (int i = 0; i < n; ++i) {
        if (colorButton(i) == btn) {
            idx = i;
            break;
        }
    }
    if (idx < 0)
        return QWidget::eventFilter(watched, event);

    auto* ke = static_cast< QKeyEvent* >(event);
    const int key = ke->key();
    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
        btn->click();
        return true;
    }
    const int cols = effectiveColumns();
    int next = idx;
    switch (key) {
    case Qt::Key_Left:
        if (idx % cols != 0)
            next = idx - 1;
        break;
    case Qt::Key_Right:
        if ((idx % cols) != (cols - 1) && idx + 1 < n)
            next = idx + 1;
        break;
    case Qt::Key_Up:
        if (idx - cols >= 0)
            next = idx - cols;
        break;
    case Qt::Key_Down:
        if (idx + cols < n)
            next = idx + cols;
        break;
    default:
        return QWidget::eventFilter(watched, event);
    }
    if (next != idx) {
        if (QToolButton* nb = colorButton(next))
            nb->setFocus(Qt::ShortcutFocusReason);
    }
    return true;
}
}  // namespace DA
