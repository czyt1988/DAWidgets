#include "DAItemListEditWidget.h"
#include "DAItemListPickDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>
#include <utility>

namespace DA
{

bool DAItemEntry::operator==(const DAItemEntry& o) const
{
    if (data.isValid() || o.data.isValid()) {
        return data == o.data;
    }
    return text == o.text;
}

bool DAItemEntry::operator!=(const DAItemEntry& o) const
{
    return !(*this == o);
}

class DAItemListEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAItemListEditWidget)
public:
    PrivateData(DAItemListEditWidget* p);
    void init();
    // 追加一行（列表项 + 行内删除按钮），不发射信号
    void appendRow(const DAItemEntry& entry);
    // 删除行并释放行控件，不发射信号
    void removeRow(int row);
    // 条目是否已选中
    bool contains(const DAItemEntry& entry) const;
    // 重建添加菜单（aboutToShow：候选中尚未选中者）
    void rebuildAddMenu();
    // 添加按钮主体点击：搜索多选对话框一次性添加
    void onAddButtonClicked();
    // 移除所有（二次确认）
    void onRemoveAllClicked();

    QListWidget* mList { nullptr };
    QToolButton* mAddButton { nullptr };
    QMenu* mAddMenu { nullptr };
    QPushButton* mRemoveAllButton { nullptr };
    QVector< DAItemEntry > mAvailable;  ///< 候选条目
    QString mAddText;                  ///< 添加按钮文案
    QString mRemoveAllText;            ///< 移除所有按钮文案
    QString mConfirmTitle;             ///< 二次确认标题
    QString mConfirmText;              ///< 二次确认正文
};

DAItemListEditWidget::PrivateData::PrivateData(DAItemListEditWidget* p) : q_ptr(p)
{
}

void DAItemListEditWidget::PrivateData::init()
{
    DA_Q(DAItemListEditWidget, q);
    mAddText       = q->tr("Add");       // cn:添加
    mRemoveAllText = q->tr("Remove All");  // cn:移除所有
    mConfirmTitle  = q->tr("Remove All");  // cn:移除所有
    mConfirmText   = q->tr("Remove all selected items?");  // cn:确定移除所有已选条目吗？

    QVBoxLayout* root = new QVBoxLayout(q);
    root->setContentsMargins(0, 0, 0, 0);
    mList = new QListWidget(q);
    mList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    root->addWidget(mList, 1);

    QHBoxLayout* bar = new QHBoxLayout();
    mAddButton = new QToolButton(q);
    mAddButton->setText(mAddText);
    mAddButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mAddButton->setPopupMode(QToolButton::MenuButtonPopup);  // 箭头=候选菜单，主体=勾选对话框
    mAddMenu = new QMenu(mAddButton);
    mAddButton->setMenu(mAddMenu);
    QObject::connect(mAddMenu, &QMenu::aboutToShow, q, [ this ]() { rebuildAddMenu(); });
    QObject::connect(mAddButton, &QToolButton::clicked, q, [ this ]() { onAddButtonClicked(); });
    bar->addWidget(mAddButton);
    mRemoveAllButton = new QPushButton(mRemoveAllText, q);
    QObject::connect(mRemoveAllButton, &QPushButton::clicked, q, [ this ]() { onRemoveAllClicked(); });
    bar->addWidget(mRemoveAllButton);
    bar->addStretch(1);
    root->addLayout(bar);
}

void DAItemListEditWidget::PrivateData::appendRow(const DAItemEntry& entry)
{
    DA_Q(DAItemListEditWidget, q);
    QListWidgetItem* item = new QListWidgetItem(mList);
    item->setSizeHint(QSize(0, 26));
    // 展示文本与标识 data 存列表项（行控件只做展示与删除按钮）
    item->setText(entry.text);
    item->setData(Qt::UserRole, entry.data);
    QWidget* row = new QWidget(mList);
    QHBoxLayout* rl = new QHBoxLayout(row);
    rl->setContentsMargins(4, 0, 2, 0);
    rl->setSpacing(2);
    QLabel* label = new QLabel(entry.text, row);
    label->setToolTip(entry.text);
    rl->addWidget(label, 1);
    QToolButton* close = new QToolButton(row);
    close->setAutoRaise(true);
    close->setFixedSize(20, 20);
    close->setIcon(row->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    close->setToolTip(q->tr("Remove"));  // cn:移除
    // 行内删除：按 item 定位行号（item 在被删除前始终有效）
    QObject::connect(close, &QToolButton::clicked, q, [ this, item, q ]() {
        const int row = mList->row(item);
        if (row >= 0) {
            removeRow(row);
            Q_EMIT q->selectedItemsChanged();
        }
    });
    rl->addWidget(close);
    mList->addItem(item);
    mList->setItemWidget(item, row);
}

void DAItemListEditWidget::PrivateData::removeRow(int row)
{
    QListWidgetItem* item = mList->takeItem(row);
    if (!item) {
        return;
    }
    QWidget* w = mList->itemWidget(item);
    mList->removeItemWidget(item);
    delete item;
    if (w) {
        w->deleteLater();
    }
}

bool DAItemListEditWidget::PrivateData::contains(const DAItemEntry& entry) const
{
    const QVector< DAItemEntry > sel = q_func()->selectedItems();
    for (const DAItemEntry& e : sel) {
        if (e == entry) {
            return true;
        }
    }
    return false;
}

void DAItemListEditWidget::PrivateData::rebuildAddMenu()
{
    mAddMenu->clear();
    bool any = false;
    for (const DAItemEntry& e : std::as_const(mAvailable)) {
        if (contains(e)) {
            continue;
        }
        any = true;
        QAction* act = mAddMenu->addAction(e.text);
        QObject::connect(act, &QAction::triggered, q_func(), [ this, e ]() {
            if (contains(e)) {
                return;
            }
            appendRow(e);
            Q_EMIT q_func()->selectedItemsChanged();
        });
    }
    if (!any) {
        QAction* act = mAddMenu->addAction(q_func()->tr("No available items"));  // cn:无可用条目
        act->setEnabled(false);
    }
}

void DAItemListEditWidget::PrivateData::onAddButtonClicked()
{
    DA_Q(DAItemListEditWidget, q);
    QVector< DAItemEntry > candidates;
    for (const DAItemEntry& e : std::as_const(mAvailable)) {
        if (!contains(e)) {
            candidates.append(e);
        }
    }
    if (candidates.isEmpty()) {
        return;
    }
    DAItemListPickDialog dlg(q);
    dlg.setItems(candidates);
    if (QDialog::Accepted != dlg.exec()) {
        return;
    }
    const QVector< DAItemEntry > picked = dlg.checkedEntries();
    if (picked.isEmpty()) {
        return;
    }
    for (const DAItemEntry& e : picked) {
        appendRow(e);
    }
    Q_EMIT q->selectedItemsChanged();
}

void DAItemListEditWidget::PrivateData::onRemoveAllClicked()
{
    DA_Q(DAItemListEditWidget, q);
    if (mList->count() == 0) {
        return;
    }
    if (QMessageBox::Yes != QMessageBox::question(q, mConfirmTitle, mConfirmText, QMessageBox::Yes | QMessageBox::No,
                                                  QMessageBox::No)) {
        return;
    }
    while (mList->count() > 0) {
        removeRow(0);
    }
    Q_EMIT q->selectedItemsChanged();
}

DAItemListEditWidget::DAItemListEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->init();
}

DAItemListEditWidget::~DAItemListEditWidget()
{
}

void DAItemListEditWidget::setAvailableItems(const QVector< DAItemEntry >& items)
{
    DA_D(d);
    d->mAvailable = items;
}

QVector< DAItemEntry > DAItemListEditWidget::availableItems() const
{
    DA_DC(d);
    return d->mAvailable;
}

void DAItemListEditWidget::setSelectedItems(const QVector< DAItemEntry >& items)
{
    DA_D(d);
    while (d->mList->count() > 0) {
        d->removeRow(0);
    }
    for (const DAItemEntry& e : items) {
        d->appendRow(e);
    }
}

QVector< DAItemEntry > DAItemListEditWidget::selectedItems() const
{
    DA_DC(d);
    QVector< DAItemEntry > res;
    for (int i = 0; i < d->mList->count(); ++i) {
        const QListWidgetItem* item = d->mList->item(i);
        res.append(DAItemEntry { item->text(), item->data(Qt::UserRole) });
    }
    return res;
}

void DAItemListEditWidget::setAddButtonText(const QString& t)
{
    DA_D(d);
    d->mAddText = t;
    d->mAddButton->setText(t);
}

void DAItemListEditWidget::setRemoveAllButtonText(const QString& t)
{
    DA_D(d);
    d->mRemoveAllText = t;
    d->mRemoveAllButton->setText(t);
}

void DAItemListEditWidget::setRemoveAllConfirmText(const QString& title, const QString& text)
{
    DA_D(d);
    d->mConfirmTitle = title;
    d->mConfirmText  = text;
}

}  // namespace DA
