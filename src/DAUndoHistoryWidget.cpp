#include "DAUndoHistoryWidget.h"
#include "DAQt5Qt6Compat.h"
#include <QAbstractItemModel>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QUndoCommand>
#include <QUndoGroup>
#include <QUndoStack>
#include <QUndoView>
#include <QVBoxLayout>
#include <functional>

namespace DA
{
namespace
{
// 私有 delegate：左侧状态点 + 右侧时间；两个开关都关时完全交给基类
class UndoRowDelegate : public QStyledItemDelegate
{
public:
    using StampFn   = std::function< QDateTime(int row) >;
    using AppliedFn = std::function< bool(int row) >;
    using FlagFn    = std::function< bool() >;

    UndoRowDelegate(FlagFn timestamps, FlagFn icons, StampFn stamp, AppliedFn applied, QObject* parent)
        : QStyledItemDelegate(parent)
        , mTimestampsOn(std::move(timestamps))
        , mIconsOn(std::move(icons))
        , mStamp(std::move(stamp))
        , mApplied(std::move(applied))
    {
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        const bool timestamps = mTimestampsOn && mTimestampsOn();
        const bool icons      = mIconsOn && mIconsOn();
        if (!timestamps && !icons) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }
        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);
        const QRect full = opt.rect;
        const QFontMetrics fm(opt.font);
        QString timeText;
        QRect timeRect;
        if (timestamps) {
            const QDateTime dt = mStamp ? mStamp(index.row()) : QDateTime();
            if (dt.isValid()) {
                timeText       = dt.toString(QStringLiteral("HH:mm:ss"));
                const int tw   = Qt5Qt6Compat_fontMetrics_width(fm, timeText);
                timeRect       = QRect(full.right() - tw - 2, full.top(), tw, full.height());
                opt.rect.setRight(full.right() - tw - 8);
            }
        }
        if (icons)
            opt.rect.setLeft(full.left() + 14);
        QStyledItemDelegate::paint(painter, opt, index);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        if (icons && mApplied) {
            const QRect dot(full.left() + 6, full.center().y() - 3, 7, 7);
            if (mApplied(index.row())) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(0x67, 0xC2, 0x3A));
            } else {
                painter->setPen(QPen(QColor(0xC0, 0xC4, 0xCC), 1.2));
                painter->setBrush(Qt::NoBrush);
            }
            painter->drawEllipse(dot);
        }
        if (!timeText.isEmpty()) {
            painter->setPen(QColor(0x90, 0x93, 0x99));
            painter->drawText(timeRect, Qt::AlignRight | Qt::AlignVCenter, timeText);
        }
        painter->restore();
    }

private:
    FlagFn mTimestampsOn;
    FlagFn mIconsOn;
    StampFn mStamp;
    AppliedFn mApplied;
};
}  // namespace

/**
 * @brief 构造：撤销深度调节行 + QUndoView
 * @param parent 父对象
 */
DAUndoHistoryWidget::DAUndoHistoryWidget(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* main = new QVBoxLayout(this);
    main->setContentsMargins(4, 4, 4, 4);
    // 深度调节行（R19：默认 20，面板可调大；配置持久化在 APP 侧）
    QHBoxLayout* limitRow = new QHBoxLayout();
    QLabel* label         = new QLabel(tr("Undo depth:"), this);  // cn:撤销深度：
    mSpinLimit            = new QSpinBox(this);
    mSpinLimit->setObjectName(QStringLiteral("undoLimitSpin"));
    mSpinLimit->setRange(1, 10000);
    mSpinLimit->setValue(20);
    mSpinLimit->setToolTip(tr("Maximum number of undo steps kept per table page (larger uses more memory for swap-type commands)"));  // cn:每个表格页保留的最大撤销步数（调大对转置类命令会增加内存占用）
    limitRow->addWidget(label);
    limitRow->addWidget(mSpinLimit);
    limitRow->addStretch();
    main->addLayout(limitRow);
    // 历史列表（QUndoView：跟随激活栈、点击条目回退/前进、显示命令描述）
    mView = new QUndoView(this);
    mView->setObjectName(QStringLiteral("undoHistoryView"));
    mView->setItemDelegate(new UndoRowDelegate(
        [ this ]() { return mTimestampsVisible; }, [ this ]() { return mIconsVisible; },
        [ this ](int row) {
            const int idx = rowToCommandIndex(row);
            return (idx >= 0 && idx < mStamps.size()) ? mStamps.at(idx) : QDateTime();
        },
        [ this ](int row) { return isRowApplied(row); }, mView));
    main->addWidget(mView, 1);
    connect(mSpinLimit, QOverload< int >::of(&QSpinBox::valueChanged), this, [ this ](int v) {
        if (mUpdatingLimit) {
            return;
        }
        Q_EMIT undoLimitChangeRequested(v);
    });
    if (QAbstractItemModel* model = mView->model()) {
        connect(model, &QAbstractItemModel::modelReset, this, [ this ]() { refilterRows(); });
        connect(model, &QAbstractItemModel::rowsInserted, this, [ this ]() { refilterRows(); });
        connect(model, &QAbstractItemModel::rowsRemoved, this, [ this ]() { refilterRows(); });
    }
}

DAUndoHistoryWidget::~DAUndoHistoryWidget()
{
}

/**
 * @brief 绑定全局撤销组（视图自动跟随组内激活栈）
 * @param group QUndoGroup（非拥有，可空=显示空列表）
 */
void DAUndoHistoryWidget::setUndoGroup(QUndoGroup* group)
{
    if (!mView) {
        return;
    }
    if (QUndoGroup* old = mView->group())
        disconnect(old, &QUndoGroup::activeStackChanged, this, &DAUndoHistoryWidget::attachStack);
    mView->setGroup(group);
    if (group) {
        connect(group, &QUndoGroup::activeStackChanged, this, &DAUndoHistoryWidget::attachStack,
                Qt::UniqueConnection);
        attachStack(group->activeStack());
    } else {
        attachStack(nullptr);
    }
}

QUndoGroup* DAUndoHistoryWidget::undoGroup() const
{
    return mView ? mView->group() : nullptr;
}

int DAUndoHistoryWidget::undoLimit() const
{
    return mSpinLimit ? mSpinLimit->value() : 20;
}

/**
 * @brief 设置深度显示值（APP 读配置注入；屏蔽信号避免回环写配置）
 * @param limit 撤销深度
 */
void DAUndoHistoryWidget::setUndoLimit(int limit)
{
    if (!mSpinLimit) {
        return;
    }
    mUpdatingLimit = true;
    mSpinLimit->setValue(qBound(mSpinLimit->minimum(), limit, mSpinLimit->maximum()));
    mUpdatingLimit = false;
}

/**
 * @brief 打开/关闭每行右侧时间
 * @param on 是否显示（默认 false）
 */
void DAUndoHistoryWidget::setTimestampsVisible(bool on)
{
    if (mTimestampsVisible == on)
        return;
    mTimestampsVisible = on;
    if (mView)
        mView->viewport()->update();
}

/** @brief 时间显示是否启用 */
bool DAUndoHistoryWidget::timestampsVisible() const
{
    return mTimestampsVisible;
}

/**
 * @brief 打开/关闭每行左侧状态点
 * @param on 是否显示（默认 false）
 */
void DAUndoHistoryWidget::setIconsVisible(bool on)
{
    if (mIconsVisible == on)
        return;
    mIconsVisible = on;
    if (mView)
        mView->viewport()->update();
}

/** @brief 状态点是否启用 */
bool DAUndoHistoryWidget::iconsVisible() const
{
    return mIconsVisible;
}

/**
 * @brief 打开/关闭空命令隐藏
 * @param on 是否隐藏文本为空的命令行（默认 false）
 */
void DAUndoHistoryWidget::setEmptyCommandFilterEnabled(bool on)
{
    if (mEmptyFilterEnabled == on)
        return;
    mEmptyFilterEnabled = on;
    refilterRows();
}

/** @brief 空命令隐藏是否启用 */
bool DAUndoHistoryWidget::isEmptyCommandFilterEnabled() const
{
    return mEmptyFilterEnabled;
}

/**
 * @brief 查询某条命令的入栈时刻
 * @param index 与 QUndoStack 下标一致
 * @return 本地时间；未记录或越界返回无效值
 */
QDateTime DAUndoHistoryWidget::commandTimestamp(int index) const
{
    if (index < 0 || index >= mStamps.size())
        return QDateTime();
    return mStamps.at(index);
}

int DAUndoHistoryWidget::rowToCommandIndex(int row) const
{
    // QUndoModel: row 0 = 初始状态，row r = 命令 r-1
    if (!mWatchedStack || row <= 0)
        return -1;
    const int idx = row - 1;
    if (idx >= mWatchedStack->count())
        return -1;
    return idx;
}

bool DAUndoHistoryWidget::isRowApplied(int row) const
{
    const int idx = rowToCommandIndex(row);
    return idx >= 0 && mWatchedStack && idx < mWatchedStack->index();
}

void DAUndoHistoryWidget::attachStack(QUndoStack* stack)
{
    if (mWatchedStack)
        disconnect(mWatchedStack, nullptr, this, nullptr);
    mWatchedStack = stack;
    mStamps.clear();
    if (!stack) {
        refilterRows();
        return;
    }
    // 已有历史补一份当前时间（用户中途打开显示也能看到时间）
    const QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < stack->count(); ++i)
        mStamps.append(now);
    connect(stack, &QUndoStack::indexChanged, this, [ this ](int) {
        noteStackChange(mWatchedStack ? mWatchedStack->count() : 0);
        refilterRows();
    });
    refilterRows();
}

void DAUndoHistoryWidget::noteStackChange(int count)
{
    if (count > mStamps.size()) {
        while (mStamps.size() < count)
            mStamps.append(QDateTime::currentDateTime());
    } else if (count < mStamps.size()) {
        // redo 分支被截断：旧时间作废
        mStamps.resize(count);
    }
}

void DAUndoHistoryWidget::refilterRows()
{
    if (!mView || !mView->model())
        return;
    const int rows = mView->model()->rowCount();
    for (int row = 0; row < rows; ++row) {
        bool hide = false;
        const int idx = rowToCommandIndex(row);
        if (mEmptyFilterEnabled && idx >= 0 && mWatchedStack)
            hide = mWatchedStack->text(idx).trimmed().isEmpty();
        mView->setRowHidden(row, hide);
    }
}

}  // end of namespace DA
