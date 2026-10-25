#include "DAItemListPickDialog.h"
#include "DACheckListWidget.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>

namespace DA
{

class DAItemListPickDialog::PrivateData
{
    DA_DECLARE_PUBLIC(DAItemListPickDialog)
public:
    PrivateData(DAItemListPickDialog* p);
    void init();

    DACheckListWidget* mCheckList { nullptr };
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
    // 勾选/搜索/forced 语义都在 DACheckListWidget 内实现，对话框只是壳
    mCheckList = new DACheckListWidget(q);
    mCheckList->setSearchVisible(true);
    root->addWidget(mCheckList, 1);
    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, q);
    QObject::connect(box, &QDialogButtonBox::accepted, q, &QDialog::accept);
    QObject::connect(box, &QDialogButtonBox::rejected, q, &QDialog::reject);
    root->addWidget(box);
}

/**
 * @brief 构造条目勾选对话框
 * @param parent 父窗口
 */
DAItemListPickDialog::DAItemListPickDialog(QWidget* parent) : QDialog(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->init();
}

/**
 * @brief 析构
 */
DAItemListPickDialog::~DAItemListPickDialog()
{
}

/**
 * @brief 设置候选条目（全部未勾选）
 * @param items 候选条目
 */
void DAItemListPickDialog::setItems(const QVector< DAItemEntry >& items)
{
    DA_D(d);
    d->mCheckList->setItems(items);
}

/**
 * @brief 预勾选若干条目
 * @param entries 要勾选的条目
 */
void DAItemListPickDialog::setCheckedEntries(const QVector< DAItemEntry >& entries)
{
    DA_D(d);
    d->mCheckList->setCheckedEntries(entries);
}

/**
 * @brief 取回勾中的条目
 * @return 勾选条目（列表顺序）
 */
QVector< DAItemEntry > DAItemListPickDialog::checkedEntries() const
{
    DA_DC(d);
    return d->mCheckList->checkedEntries();
}

/**
 * @brief 覆盖搜索框占位文本
 * @param t 文本
 */
void DAItemListPickDialog::setSearchPlaceholderText(const QString& t)
{
    DA_D(d);
    d->mCheckList->setSearchPlaceholderText(t);
}

}  // namespace DA
