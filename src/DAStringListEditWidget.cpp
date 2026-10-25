#include "DAStringListEditWidget.h"
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace DA
{

class DAStringListEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAStringListEditWidget)
public:
    PrivateData(DAStringListEditWidget* p);
    void init();
    // 统一刷新按钮可用状态与空状态提示
    void updateUiState();
    // 更新占位提示的位置与可见性
    void updatePlaceholder();
    // 当前行上移/下移，返回是否真的移动
    bool moveCurrent(int offset);
    void onAddClicked();
    void onRemoveClicked();

    QListWidget* mList { nullptr };
    QPushButton* mAdd { nullptr };
    QPushButton* mRemove { nullptr };
    QPushButton* mUp { nullptr };
    QPushButton* mDown { nullptr };
    QLabel* mPlaceholder { nullptr };
    AddMode mAddMode { AddMode::Inline };
    bool mReorderEnabled { false };
    bool mRemoveEnabled { true };
    bool mEditable { true };
    int mMaxCount { 0 };
    QString mPlaceholderText;
    bool mUpdating { false };  ///< 程序化增删期间屏蔽 itemChanged 回环
};

DAStringListEditWidget::PrivateData::PrivateData(DAStringListEditWidget* p) : q_ptr(p)
{
}

void DAStringListEditWidget::PrivateData::init()
{
    DA_Q(DAStringListEditWidget, q);
    QHBoxLayout* root = new QHBoxLayout(q);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(4);

    mList = new QListWidget(q);
    mList->setObjectName(QStringLiteral("stringList"));
    mList->setSelectionMode(QAbstractItemView::SingleSelection);
    mList->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    root->addWidget(mList, 1);

    // 空状态提示：叠在 viewport 上，不参与布局、不吃鼠标
    mPlaceholder = new QLabel(mList->viewport());
    mPlaceholder->setObjectName(QStringLiteral("placeholderLabel"));
    mPlaceholder->setAlignment(Qt::AlignCenter);
    mPlaceholder->setWordWrap(true);
    mPlaceholder->setAttribute(Qt::WA_TransparentForMouseEvents);
    mPlaceholder->setStyleSheet(QStringLiteral("color:#909399;background:transparent;"));
    mPlaceholder->hide();
    mList->viewport()->installEventFilter(q);

    QVBoxLayout* btnLayout = new QVBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(2);
    mAdd = new QPushButton(q->tr("Add"), q);  // cn:添加
    mAdd->setObjectName(QStringLiteral("addButton"));
    mRemove = new QPushButton(q->tr("Remove"), q);  // cn:删除
    mRemove->setObjectName(QStringLiteral("removeButton"));
    mUp = new QPushButton(q->tr("Up"), q);  // cn:上移
    mUp->setObjectName(QStringLiteral("upButton"));
    mDown = new QPushButton(q->tr("Down"), q);  // cn:下移
    mDown->setObjectName(QStringLiteral("downButton"));
    btnLayout->addWidget(mAdd);
    btnLayout->addWidget(mRemove);
    btnLayout->addWidget(mUp);
    btnLayout->addWidget(mDown);
    btnLayout->addStretch();
    root->addLayout(btnLayout);

    QObject::connect(mAdd, &QPushButton::clicked, q, [ this ]() { onAddClicked(); });
    QObject::connect(mRemove, &QPushButton::clicked, q, [ this ]() { onRemoveClicked(); });
    QObject::connect(mUp, &QPushButton::clicked, q, [ this ]() { moveCurrent(-1); });
    QObject::connect(mDown, &QPushButton::clicked, q, [ this ]() { moveCurrent(1); });
    QObject::connect(mList, &QListWidget::itemChanged, q, [ this ]() {
        if (mUpdating) {
            return;
        }
        updateUiState();
        Q_EMIT q_func()->itemsChanged();
    });
    QObject::connect(mList, &QListWidget::currentRowChanged, q, [ this ](int) { updateUiState(); });
    updateUiState();
}

void DAStringListEditWidget::PrivateData::updateUiState()
{
    const int count      = mList->count();
    const bool overLimit = mMaxCount > 0 && count >= mMaxCount;
    const bool canInlineAdd = (mAddMode == AddMode::Inline) ? mEditable : true;
    mAdd->setEnabled(!overLimit && canInlineAdd);
    mRemove->setVisible(mRemoveEnabled);
    mRemove->setEnabled(mRemoveEnabled && mList->currentRow() >= 0);
    mUp->setVisible(mReorderEnabled);
    mDown->setVisible(mReorderEnabled);
    mUp->setEnabled(mReorderEnabled && mList->currentRow() > 0);
    mDown->setEnabled(mReorderEnabled && mList->currentRow() >= 0 && mList->currentRow() < count - 1);
    updatePlaceholder();
}

void DAStringListEditWidget::PrivateData::updatePlaceholder()
{
    const bool show = mList->count() == 0 && !mPlaceholderText.isEmpty();
    mPlaceholder->setText(mPlaceholderText);
    mPlaceholder->setGeometry(mList->viewport()->rect().adjusted(6, 6, -6, -6));
    mPlaceholder->setVisible(show);
}

bool DAStringListEditWidget::PrivateData::moveCurrent(int offset)
{
    DA_Q(DAStringListEditWidget, q);
    const int row = mList->currentRow();
    const int dst = row + offset;
    if (row < 0 || dst < 0 || dst >= mList->count()) {
        return false;
    }
    mUpdating = true;
    QListWidgetItem* item = mList->takeItem(row);
    mList->insertItem(dst, item);
    mList->setCurrentRow(dst);
    mUpdating = false;
    updateUiState();
    Q_EMIT q->itemsChanged();
    return true;
}

void DAStringListEditWidget::PrivateData::onAddClicked()
{
    DA_Q(DAStringListEditWidget, q);
    if (mAddMode == AddMode::External) {
        Q_EMIT q->addRequested();
        return;
    }
    if (mMaxCount > 0 && mList->count() >= mMaxCount) {
        return;
    }
    if (!mEditable) {
        return;
    }
    mUpdating = true;
    QListWidgetItem* item = new QListWidgetItem(QString(), mList);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    mList->setCurrentItem(item);
    mUpdating = false;
    // Inline：直接进入编辑，不插入 "New Item" 之类的占位文本
    mList->editItem(item);
    updateUiState();
    Q_EMIT q->itemsChanged();
}

void DAStringListEditWidget::PrivateData::onRemoveClicked()
{
    DA_Q(DAStringListEditWidget, q);
    const int row = mList->currentRow();
    if (row < 0) {
        return;
    }
    mUpdating = true;
    delete mList->takeItem(row);
    mUpdating = false;
    updateUiState();
    Q_EMIT q->itemsChanged();
}

//===============================================================
// DAStringListEditWidget
//===============================================================

/**
 * @brief 构造字符串列表编辑器
 * @param parent 父控件
 */
DAStringListEditWidget::DAStringListEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    DA_D(d);
    d->init();
}

/**
 * @brief 析构
 */
DAStringListEditWidget::~DAStringListEditWidget()
{
}

/**
 * @brief 整体替换列表（不发 itemsChanged）
 * @param items 新列表
 */
void DAStringListEditWidget::setItems(const QStringList& items)
{
    DA_D(d);
    d->mUpdating = true;
    d->mList->clear();
    for (const QString& s : items) {
        QListWidgetItem* item = new QListWidgetItem(s, d->mList);
        if (d->mEditable) {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }
    d->mUpdating = false;
    d->updateUiState();
}

/**
 * @brief 获取当前列表
 * @return 字符串列表
 */
QStringList DAStringListEditWidget::items() const
{
    DA_DC(d);
    QStringList res;
    for (int i = 0; i < d->mList->count(); ++i) {
        res.append(d->mList->item(i)->text());
    }
    return res;
}

/**
 * @brief 追加一条（External 模式下由调用方使用）
 * @param text 文本（允许空串）
 */
void DAStringListEditWidget::appendItem(const QString& text)
{
    DA_D(d);
    if (d->mMaxCount > 0 && d->mList->count() >= d->mMaxCount) {
        return;
    }
    d->mUpdating = true;
    QListWidgetItem* item = new QListWidgetItem(text, d->mList);
    if (d->mEditable) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
    d->mList->setCurrentItem(item);
    d->mUpdating = false;
    d->updateUiState();
    Q_EMIT itemsChanged();
}

/**
 * @brief 设置 Add 按钮行为
 * @param m Inline/External
 */
void DAStringListEditWidget::setAddMode(AddMode m)
{
    DA_D(d);
    d->mAddMode = m;
    d->updateUiState();
}

/**
 * @brief 获取 Add 按钮行为
 * @return AddMode
 */
DAStringListEditWidget::AddMode DAStringListEditWidget::addMode() const
{
    DA_DC(d);
    return d->mAddMode;
}

/**
 * @brief 设置 Up/Down 按钮显隐
 * @param on 是否显示
 */
void DAStringListEditWidget::setReorderEnabled(bool on)
{
    DA_D(d);
    d->mReorderEnabled = on;
    d->updateUiState();
}

/**
 * @brief 获取 Up/Down 按钮显隐
 * @return 是否启用排序
 */
bool DAStringListEditWidget::isReorderEnabled() const
{
    DA_DC(d);
    return d->mReorderEnabled;
}

/**
 * @brief 设置 Remove 按钮显隐
 * @param on 是否显示
 */
void DAStringListEditWidget::setRemoveEnabled(bool on)
{
    DA_D(d);
    d->mRemoveEnabled = on;
    d->updateUiState();
}

/**
 * @brief 获取 Remove 按钮显隐
 * @return 是否启用删除
 */
bool DAStringListEditWidget::isRemoveEnabled() const
{
    DA_DC(d);
    return d->mRemoveEnabled;
}

/**
 * @brief 设置是否允许编辑条目文本
 * @param on 是否可编辑
 */
void DAStringListEditWidget::setEditable(bool on)
{
    DA_D(d);
    d->mEditable = on;
    for (int i = 0; i < d->mList->count(); ++i) {
        QListWidgetItem* item = d->mList->item(i);
        Qt::ItemFlags flags    = item->flags();
        if (on) {
            flags |= Qt::ItemIsEditable;
        } else {
            flags &= ~Qt::ItemIsEditable;
        }
        item->setFlags(flags);
    }
    d->mList->setEditTriggers(on ? (QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed)
                                 : QAbstractItemView::NoEditTriggers);
    d->updateUiState();
}

/**
 * @brief 获取是否允许编辑
 * @return 是否可编辑
 */
bool DAStringListEditWidget::isEditable() const
{
    DA_DC(d);
    return d->mEditable;
}

/**
 * @brief 设置数量上限（<=0 不限）
 * @param n 上限
 */
void DAStringListEditWidget::setMaxCount(int n)
{
    DA_D(d);
    d->mMaxCount = n;
    d->updateUiState();
}

/**
 * @brief 获取数量上限
 * @return 上限（<=0 表示不限）
 */
int DAStringListEditWidget::maxCount() const
{
    DA_DC(d);
    return d->mMaxCount;
}

/**
 * @brief 设置空列表提示文本
 * @param t 文本
 */
void DAStringListEditWidget::setPlaceholderText(const QString& t)
{
    DA_D(d);
    d->mPlaceholderText = t;
    d->updatePlaceholder();
}

/**
 * @brief 获取空列表提示文本
 * @return 文本
 */
QString DAStringListEditWidget::placeholderText() const
{
    DA_DC(d);
    return d->mPlaceholderText;
}

/**
 * @brief 监听 viewport 尺寸变化以重排空状态提示
 * @param obj 事件目标
 * @param e 事件
 * @return 是否拦截
 */
bool DAStringListEditWidget::eventFilter(QObject* obj, QEvent* e)
{
    DA_D(d);
    if (d->mList && obj == d->mList->viewport() && e->type() == QEvent::Resize) {
        d->updatePlaceholder();
    }
    return QWidget::eventFilter(obj, e);
}

}  // namespace DA
