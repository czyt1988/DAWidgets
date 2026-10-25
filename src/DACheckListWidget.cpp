#include "DACheckListWidget.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <utility>

namespace DA
{

namespace
{
// 列表项上的附加数据（Qt::UserRole 已被 DAItemEntry::data 占用）
constexpr int kForcedRole    = Qt::UserRole + 1;
constexpr int kCheckableRole = Qt::UserRole + 2;
}  // namespace

class DACheckListWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DACheckListWidget)
public:
    PrivateData(DACheckListWidget* p);
    void init();
    // 搜索过滤：只隐藏行，不动勾选状态
    void applyFilter(const QString& key);
    // 按条目查找列表项（DAItemEntry 相等语义）
    QListWidgetItem* findItem(const DAItemEntry& e) const;
    DAItemEntry entryOf(const QListWidgetItem* item) const;
    bool itemForced(const QListWidgetItem* item) const;
    bool itemCheckable(const QListWidgetItem* item) const;
    bool itemUsable(const QListWidgetItem* item) const;
    bool rowVisible(int row) const;
    void setCheckedSilently(QListWidgetItem* item, bool on);
    // 全选/反选：只作用于可见 + 可勾选 + 非 forced + 可用的条目
    void selectAllVisible();
    void invertVisible();
    void onItemChanged(QListWidgetItem* item);

    QLineEdit* mSearch { nullptr };
    QListWidget* mList { nullptr };
    QPushButton* mSelectAll { nullptr };
    QPushButton* mInvert { nullptr };
    bool mUpdating { false };  ///< 程序化改动期间屏蔽 itemChanged 回环
};

DACheckListWidget::PrivateData::PrivateData(DACheckListWidget* p) : q_ptr(p)
{
}

void DACheckListWidget::PrivateData::init()
{
    DA_Q(DACheckListWidget, q);
    QVBoxLayout* root = new QVBoxLayout(q);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(4);

    mSearch = new QLineEdit(q);
    mSearch->setObjectName(QStringLiteral("searchEdit"));
    mSearch->setPlaceholderText(q->tr("Search..."));  // cn:搜索...
    mSearch->setClearButtonEnabled(true);
    root->addWidget(mSearch);

    mList = new QListWidget(q);
    mList->setObjectName(QStringLiteral("checkList"));
    // 勾选即选择，避免选中态干扰
    mList->setSelectionMode(QAbstractItemView::NoSelection);
    root->addWidget(mList, 1);

    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 0, 0, 0);
    mSelectAll = new QPushButton(q->tr("Select All"), q);  // cn:全选
    mSelectAll->setObjectName(QStringLiteral("selectAllButton"));
    mInvert = new QPushButton(q->tr("Invert Selection"), q);  // cn:反选
    mInvert->setObjectName(QStringLiteral("invertButton"));
    btnRow->addWidget(mSelectAll);
    btnRow->addWidget(mInvert);
    btnRow->addStretch(1);
    root->addLayout(btnRow);

    QObject::connect(mSearch, &QLineEdit::textChanged, q, [ this ](const QString& t) { applyFilter(t); });
    QObject::connect(mSelectAll, &QPushButton::clicked, q, [ this ]() { selectAllVisible(); });
    QObject::connect(mInvert, &QPushButton::clicked, q, [ this ]() { invertVisible(); });
    QObject::connect(mList, &QListWidget::itemChanged, q, [ this ](QListWidgetItem* item) { onItemChanged(item); });
}

void DACheckListWidget::PrivateData::applyFilter(const QString& key)
{
    const QString k = key.trimmed().toLower();
    for (int i = 0; i < mList->count(); ++i) {
        const bool hit = k.isEmpty() || mList->item(i)->text().toLower().contains(k);
        mList->setRowHidden(i, !hit);
    }
}

QListWidgetItem* DACheckListWidget::PrivateData::findItem(const DAItemEntry& e) const
{
    for (int i = 0; i < mList->count(); ++i) {
        QListWidgetItem* item = mList->item(i);
        if (entryOf(item) == e) {
            return item;
        }
    }
    return nullptr;
}

DAItemEntry DACheckListWidget::PrivateData::entryOf(const QListWidgetItem* item) const
{
    return DAItemEntry { item->text(), item->data(Qt::UserRole) };
}

bool DACheckListWidget::PrivateData::itemForced(const QListWidgetItem* item) const
{
    return item->data(kForcedRole).toBool();
}

bool DACheckListWidget::PrivateData::itemCheckable(const QListWidgetItem* item) const
{
    return item->data(kCheckableRole).toBool();
}

bool DACheckListWidget::PrivateData::itemUsable(const QListWidgetItem* item) const
{
    return item->flags().testFlag(Qt::ItemIsEnabled);
}

bool DACheckListWidget::PrivateData::rowVisible(int row) const
{
    return !mList->isRowHidden(row);
}

void DACheckListWidget::PrivateData::setCheckedSilently(QListWidgetItem* item, bool on)
{
    const Qt::CheckState target = on ? Qt::Checked : Qt::Unchecked;
    if (item->checkState() == target) {
        return;
    }
    const bool prev = mUpdating;
    mUpdating       = true;
    item->setCheckState(target);
    mUpdating = prev;
}

void DACheckListWidget::PrivateData::selectAllVisible()
{
    DA_Q(DACheckListWidget, q);
    bool changed = false;
    for (int i = 0; i < mList->count(); ++i) {
        if (!rowVisible(i)) {
            continue;
        }
        QListWidgetItem* item = mList->item(i);
        if (itemForced(item) || !itemCheckable(item) || !itemUsable(item)) {
            continue;
        }
        if (item->checkState() != Qt::Checked) {
            setCheckedSilently(item, true);
            changed = true;
        }
    }
    if (changed) {
        Q_EMIT q->checkedEntriesChanged();
    }
}

void DACheckListWidget::PrivateData::invertVisible()
{
    DA_Q(DACheckListWidget, q);
    bool changed = false;
    for (int i = 0; i < mList->count(); ++i) {
        if (!rowVisible(i)) {
            continue;
        }
        QListWidgetItem* item = mList->item(i);
        if (itemForced(item) || !itemCheckable(item) || !itemUsable(item)) {
            continue;
        }
        setCheckedSilently(item, item->checkState() != Qt::Checked);
        changed = true;
    }
    if (changed) {
        Q_EMIT q->checkedEntriesChanged();
    }
}

void DACheckListWidget::PrivateData::onItemChanged(QListWidgetItem* item)
{
    DA_Q(DACheckListWidget, q);
    if (mUpdating) {
        return;
    }
    // forced 条目永远勾选：用户取消后立即恢复，不发信号
    if (itemForced(item)) {
        setCheckedSilently(item, true);
        return;
    }
    // 没有复选框的条目不应出现勾选态
    if (!itemCheckable(item)) {
        setCheckedSilently(item, false);
        return;
    }
    Q_EMIT q->checkedEntriesChanged();
}

//===============================================================
// DACheckListWidget
//===============================================================

/**
 * @brief 构造勾选列表
 * @param parent 父控件
 */
DACheckListWidget::DACheckListWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->init();
}

/**
 * @brief 析构
 */
DACheckListWidget::~DACheckListWidget()
{
}

/**
 * @brief 设置候选条目（全部未勾选、可用、可勾选、非 forced）
 * @param items 候选条目
 * @note 整体替换，不发 checkedEntriesChanged
 */
void DACheckListWidget::setItems(const QVector< DAItemEntry >& items)
{
    DA_D(d);
    d->mUpdating = true;
    d->mList->clear();
    for (const DAItemEntry& e : items) {
        QListWidgetItem* item = new QListWidgetItem(e.text, d->mList);
        item->setData(Qt::UserRole, e.data);
        item->setData(kForcedRole, false);
        item->setData(kCheckableRole, true);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
    d->mUpdating = false;
    // 新条目也要遵守当前搜索条件
    d->applyFilter(d->mSearch->text());
}

/**
 * @brief 获取全部候选条目
 * @return 条目列表（含被过滤掉/禁用的）
 */
QVector< DAItemEntry > DACheckListWidget::items() const
{
    DA_DC(d);
    QVector< DAItemEntry > res;
    res.reserve(d->mList->count());
    for (int i = 0; i < d->mList->count(); ++i) {
        res.append(d->entryOf(d->mList->item(i)));
    }
    return res;
}

/**
 * @brief 预勾选若干条目
 * @param entries 要勾选的条目（按 DAItemEntry 相等匹配）
 * @note forced 条目始终勾选；不发 checkedEntriesChanged
 */
void DACheckListWidget::setCheckedEntries(const QVector< DAItemEntry >& entries)
{
    DA_D(d);
    d->mUpdating = true;
    for (int i = 0; i < d->mList->count(); ++i) {
        QListWidgetItem* item = d->mList->item(i);
        // forced 优先：永远勾选
        if (d->itemForced(item)) {
            d->setCheckedSilently(item, true);
            continue;
        }
        bool matched = false;
        const DAItemEntry cur = d->entryOf(item);
        for (const DAItemEntry& e : entries) {
            if (e == cur) {
                matched = true;
                break;
            }
        }
        d->setCheckedSilently(item, matched && d->itemCheckable(item));
    }
    d->mUpdating = false;
}

/**
 * @brief 获取勾中的条目
 * @return 勾选条目（列表顺序；含被过滤掉/禁用的；含 forced）
 */
QVector< DAItemEntry > DACheckListWidget::checkedEntries() const
{
    DA_DC(d);
    QVector< DAItemEntry > res;
    for (int i = 0; i < d->mList->count(); ++i) {
        const QListWidgetItem* item = d->mList->item(i);
        if (item->checkState() == Qt::Checked && d->itemCheckable(item)) {
            res.append(d->entryOf(item));
        }
    }
    return res;
}

/**
 * @brief 设置条目可用性
 * @param e 条目
 * @param enabled 是否可用（不可用时用户不能勾选）
 */
void DACheckListWidget::setEntryEnabled(const DAItemEntry& e, bool enabled)
{
    DA_D(d);
    QListWidgetItem* item = d->findItem(e);
    if (!item) {
        return;
    }
    Qt::ItemFlags flags = item->flags();
    if (enabled) {
        flags |= Qt::ItemIsEnabled;
    } else {
        flags &= ~Qt::ItemIsEnabled;
    }
    d->mUpdating = true;
    item->setFlags(flags);
    d->mUpdating = false;
}

/**
 * @brief 设置条目是否有复选框
 * @param e 条目
 * @param checkable false 时按未勾选处理
 */
void DACheckListWidget::setEntryCheckable(const DAItemEntry& e, bool checkable)
{
    DA_D(d);
    QListWidgetItem* item = d->findItem(e);
    if (!item) {
        return;
    }
    d->mUpdating = true;
    item->setData(kCheckableRole, checkable);
    Qt::ItemFlags flags = item->flags();
    if (checkable) {
        flags |= Qt::ItemIsUserCheckable;
    } else {
        flags &= ~Qt::ItemIsUserCheckable;
        // forced 优先：强制勾选项保持勾选
        if (!d->itemForced(item)) {
            d->setCheckedSilently(item, false);
        }
    }
    item->setFlags(flags);
    d->mUpdating = false;
}

/**
 * @brief 设置/取消强制勾选
 * @param e 条目
 * @param on true 时永远勾选且用户不能取消
 */
void DACheckListWidget::setEntryForcedChecked(const DAItemEntry& e, bool on)
{
    DA_D(d);
    QListWidgetItem* item = d->findItem(e);
    if (!item) {
        return;
    }
    d->mUpdating = true;
    item->setData(kForcedRole, on);
    if (on) {
        d->setCheckedSilently(item, true);
    }
    d->mUpdating = false;
}

/**
 * @brief 搜索框显隐
 * @param on 是否显示
 */
void DACheckListWidget::setSearchVisible(bool on)
{
    DA_D(d);
    d->mSearch->setVisible(on);
}

/**
 * @brief 设置搜索框占位文本
 * @param t 文本
 */
void DACheckListWidget::setSearchPlaceholderText(const QString& t)
{
    DA_D(d);
    d->mSearch->setPlaceholderText(t);
}

/**
 * @brief 全选按钮显隐
 * @param on 是否显示
 */
void DACheckListWidget::setSelectAllVisible(bool on)
{
    DA_D(d);
    d->mSelectAll->setVisible(on);
}

/**
 * @brief 反选按钮显隐
 * @param on 是否显示
 */
void DACheckListWidget::setInvertVisible(bool on)
{
    DA_D(d);
    d->mInvert->setVisible(on);
}

/**
 * @brief 条目总数
 * @return 行数
 */
int DACheckListWidget::itemCount() const
{
    DA_DC(d);
    return d->mList->count();
}

/**
 * @brief 勾选总数（含被过滤掉/禁用的/forced 条目）
 * @return 勾选数
 */
int DACheckListWidget::checkedCount() const
{
    return checkedEntries().size();
}

}  // namespace DA
