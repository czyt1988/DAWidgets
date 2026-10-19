#include "DAItemListPickDialog.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QDialogButtonBox>

namespace DA
{

class DAItemListPickDialog::PrivateData
{
    DA_DECLARE_PUBLIC(DAItemListPickDialog)
public:
    PrivateData(DAItemListPickDialog* p);
    void init();
    // 搜索过滤：隐藏不匹配行（勾选状态保存在列表项上，过滤不丢失）
    void applyFilter(const QString& key);

    QLineEdit* mSearch { nullptr };
    QListWidget* mList { nullptr };
};

DAItemListPickDialog::PrivateData::PrivateData(DAItemListPickDialog* p) : q_ptr(p)
{
}

void DAItemListPickDialog::PrivateData::init()
{
    DA_Q(DAItemListPickDialog, q);
    q->setWindowTitle(q->tr("Select Items"));  // cn:选择条目
    q->resize(320, 420);
    QVBoxLayout* root = new QVBoxLayout(q);
    mSearch           = new QLineEdit(q);
    mSearch->setPlaceholderText(q->tr("Search..."));  // cn:搜索...
    mSearch->setClearButtonEnabled(true);
    root->addWidget(mSearch);
    mList = new QListWidget(q);
    mList->setSelectionMode(QAbstractItemView::NoSelection);  // 勾选即选择，避免选中态干扰
    root->addWidget(mList, 1);
    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, q);
    QObject::connect(box, &QDialogButtonBox::accepted, q, &QDialog::accept);
    QObject::connect(box, &QDialogButtonBox::rejected, q, &QDialog::reject);
    root->addWidget(box);
    QObject::connect(mSearch, &QLineEdit::textChanged, q, [ this ](const QString& t) { applyFilter(t); });
}

void DAItemListPickDialog::PrivateData::applyFilter(const QString& key)
{
    const QString k = key.trimmed().toLower();
    for (int i = 0; i < mList->count(); ++i) {
        const bool hit = k.isEmpty() || mList->item(i)->text().toLower().contains(k);
        mList->setRowHidden(i, !hit);
    }
}

DAItemListPickDialog::DAItemListPickDialog(QWidget* parent) : QDialog(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->init();
}

DAItemListPickDialog::~DAItemListPickDialog()
{
}

void DAItemListPickDialog::setItems(const QVector< DAItemEntry >& items)
{
    DA_D(d);
    d->mList->clear();
    for (const DAItemEntry& e : items) {
        QListWidgetItem* item = new QListWidgetItem(e.text, d->mList);
        item->setData(Qt::UserRole, e.data);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
}

void DAItemListPickDialog::setCheckedEntries(const QVector< DAItemEntry >& entries)
{
    DA_D(d);
    for (int i = 0; i < d->mList->count(); ++i) {
        QListWidgetItem* item = d->mList->item(i);
        DAItemEntry cur { item->text(), item->data(Qt::UserRole) };
        for (const DAItemEntry& e : entries) {
            if (e == cur) {
                item->setCheckState(Qt::Checked);
                break;
            }
        }
    }
}

QVector< DAItemEntry > DAItemListPickDialog::checkedEntries() const
{
    DA_DC(d);
    QVector< DAItemEntry > res;
    for (int i = 0; i < d->mList->count(); ++i) {
        const QListWidgetItem* item = d->mList->item(i);
        if (item->checkState() == Qt::Checked) {
            res.append(DAItemEntry { item->text(), item->data(Qt::UserRole) });
        }
    }
    return res;
}

void DAItemListPickDialog::setSearchPlaceholderText(const QString& t)
{
    DA_D(d);
    d->mSearch->setPlaceholderText(t);
}

}  // namespace DA
