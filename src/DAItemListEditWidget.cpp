#include "DAItemListEditWidget.h"
#include "DAItemListPickDialog.h"
#include "DAQt5Qt6Compat.h"
#include "DAStringFilterProxyModel.h"
#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <functional>
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

namespace
{
/// 拖拽 MIME 类型：只传源行号，不序列化条目（行控件与 data 都不能走 Qt 的 InternalMove）
const char kRowMime[] = "application/x-dawidgets-itemlist-row";

QString rowMimeType()
{
    return QString::fromLatin1(kRowMime);
}

/**
 * @brief 行首 6 点拖拽手柄（文件内私有，不带 Q_OBJECT）
 * @details 行控件会吃掉鼠标事件，不能依赖 QListWidget::startDrag；
 *          手柄自己发起 QDrag，列表只当落点（DropOnly）。
 */
class RowHandle : public QWidget
{
public:
    RowHandle(QListWidget* list, QListWidgetItem* item, QWidget* parent)
        : QWidget(parent), mList(list), mItem(item)
    {
        setObjectName(QStringLiteral("rowHandle"));
        setFixedSize(14, 26);
        setCursor(Qt::OpenHandCursor);
        setToolTip(QObject::tr("Drag to reorder"));  //cn:拖动排序
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(Qt::NoPen);
        p.setBrush(underMouse() ? QColor(0x90, 0x93, 0x99) : QColor(0xC0, 0xC4, 0xCC));
        for (int col = 0; col < 2; ++col) {
            for (int row = 0; row < 3; ++row)
                p.drawEllipse(QPointF(4.5 + col * 5.0, 7.0 + row * 6.0), 1.3, 1.3);
        }
    }

    bool event(QEvent* e) override
    {
        if (e->type() == QEvent::Enter || e->type() == QEvent::Leave)
            update();
        return QWidget::event(e);
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton) {
            mPress   = e->pos();
            mPressed = true;
            e->accept();
            return;
        }
        QWidget::mousePressEvent(e);
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        if (!mPressed || !(e->buttons() & Qt::LeftButton))
            return;
        if ((e->pos() - mPress).manhattanLength() < QApplication::startDragDistance())
            return;
        mPressed = false;
        QWidget* row = parentWidget();
        const int src = mList->row(mItem);
        if (!row || src < 0)
            return;
        auto* mime = new QMimeData;
        mime->setData(rowMimeType(), QByteArray::number(src));
        auto* drag = new QDrag(this);
        drag->setMimeData(mime);
        // 拖动影像是行控件当前外观，必须在任何半透明 effect 之前抓
        drag->setPixmap(row->grab());
        drag->exec(Qt::MoveAction);
    }

    void mouseReleaseEvent(QMouseEvent* e) override
    {
        mPressed = false;
        QWidget::mouseReleaseEvent(e);
    }

private:
    QListWidget* mList { nullptr };
    QListWidgetItem* mItem { nullptr };
    QPoint mPress;
    bool mPressed { false };
};

/**
 * @brief 只当落点的列表（文件内私有，不带 Q_OBJECT）
 * @details 接受自定义 MIME 时显示 2px 插入线，落位回调宿主 PrivateData::moveRow，
 *          因此 data 与行控件都不会丢。
 */
class ItemListWidget : public QListWidget
{
public:
    ItemListWidget(QWidget* parent, std::function< void(int, int) > moveRow)
        : QListWidget(parent), mMoveRow(std::move(moveRow))
    {
        mIndicator = new QFrame(viewport());
        mIndicator->setObjectName(QStringLiteral("dropIndicator"));
        mIndicator->setStyleSheet(QStringLiteral("background:#409eff;"));
        mIndicator->hide();
    }

protected:
    void dragEnterEvent(QDragEnterEvent* e) override
    {
        if (e->mimeData()->hasFormat(rowMimeType())) {
            e->acceptProposedAction();
            return;
        }
        // 外部拖入（文件 URL 等）一律放行给父控件
        e->ignore();
    }

    void dragMoveEvent(QDragMoveEvent* e) override
    {
        if (!e->mimeData()->hasFormat(rowMimeType())) {
            e->ignore();
            return;
        }
        updateIndicator(Qt5Qt6Compat_QXXEvent_Pos(e));
        e->acceptProposedAction();
    }

    void dragLeaveEvent(QDragLeaveEvent* e) override
    {
        mIndicator->hide();
        QListWidget::dragLeaveEvent(e);
    }

    void dropEvent(QDropEvent* e) override
    {
        mIndicator->hide();
        if (!e->mimeData()->hasFormat(rowMimeType())) {
            e->ignore();
            return;
        }
        bool ok = false;
        const int src = e->mimeData()->data(rowMimeType()).toInt(&ok);
        if (!ok) {
            e->ignore();
            return;
        }
        const QPoint vp = Qt5Qt6Compat_QXXEvent_Pos(e);
        const QModelIndex idx = indexAt(vp);
        int insertAt = count();
        if (idx.isValid()) {
            const QRect r = visualRect(idx);
            insertAt = idx.row() + ((vp.y() > r.center().y()) ? 1 : 0);
        }
        if (src < insertAt)
            --insertAt;
        if (insertAt == src || insertAt < 0 || insertAt >= count()) {
            e->ignore();
            return;
        }
        mMoveRow(src, insertAt);
        e->acceptProposedAction();
    }

private:
    void updateIndicator(const QPoint& vp)
    {
        const QModelIndex idx = indexAt(vp);
        int y = viewport()->height() - 2;
        if (idx.isValid()) {
            const QRect r = visualRect(idx);
            y = (vp.y() > r.center().y()) ? r.bottom() : r.top() - 1;
        }
        mIndicator->setGeometry(0, y, viewport()->width(), 2);
        mIndicator->show();
        mIndicator->raise();
    }

    QFrame* mIndicator { nullptr };
    std::function< void(int, int) > mMoveRow;
};
}  // namespace

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
    // 当前行上移/下移（按钮点击）
    void moveCurrent(int offset);
    // 把 from 行移动到 to（最终索引），只发 orderChanged
    void moveRow(int from, int to);
    // 刷新 Up/Down 按钮的显隐与可用状态
    void updateReorderButtons();
    // 过滤：只改行的隐藏状态
    void applyFilter(const QString& key);
    // 空态与列表互斥显隐
    void updateEmptyState();
    // 拖拽手柄增删
    void updateRowDecorations(int row);
    // 还原「再次点击清空」的按钮文案与武装状态
    void disarmClear();

    ItemListWidget* mList { nullptr };
    QToolButton* mAddButton { nullptr };
    QMenu* mAddMenu { nullptr };
    QPushButton* mUpButton { nullptr };
    QPushButton* mDownButton { nullptr };
    QPushButton* mRemoveAllButton { nullptr };
    QLineEdit* mFilter { nullptr };
    QLabel* mEmpty { nullptr };
    bool mReorderEnabled { false };      ///< 是否显示 Up/Down 按钮
    bool mDragReorderEnabled { false };  ///< 是否启用手柄拖拽
    bool mFilterVisible { false };       ///< 是否显示过滤框
    QVector< DAItemEntry > mAvailable;  ///< 候选条目
    QString mAddText;                  ///< 添加按钮文案
    QString mRemoveAllText;            ///< 移除所有按钮文案
    QString mConfirmTitle;             ///< 二次确认标题
    QString mConfirmText;              ///< 二次确认正文
    QString mEmptyText;                ///< 空态文案
    bool mConfirmClearEnabled { false };
    int mConfirmClearInterval { 2500 };
    QString mConfirmClearText;
    QTimer mArmTimer;
    bool mArmed { false };
};

DAItemListEditWidget::PrivateData::PrivateData(DAItemListEditWidget* p) : q_ptr(p)
{
}

void DAItemListEditWidget::PrivateData::init()
{
    DA_Q(DAItemListEditWidget, q);
    mAddText          = q->tr("Add");                           // cn:添加
    mRemoveAllText    = q->tr("Remove All");                    // cn:移除所有
    mConfirmTitle     = q->tr("Remove All");                    // cn:移除所有
    mConfirmText      = q->tr("Remove all selected items?");    // cn:确定移除所有已选条目吗？
    mEmptyText        = q->tr("No matching items");             // cn:没有匹配的条目
    mConfirmClearText = q->tr("Click again to clear");          // cn:再次点击清空

    mArmTimer.setSingleShot(true);
    QObject::connect(&mArmTimer, &QTimer::timeout, q, [ this ]() { disarmClear(); });

    QVBoxLayout* root = new QVBoxLayout(q);
    root->setContentsMargins(0, 0, 0, 0);

    // 过滤框（默认隐藏，setFilterVisible 打开）
    mFilter = new QLineEdit(q);
    mFilter->setObjectName(QStringLiteral("filterEdit"));
    mFilter->setPlaceholderText(q->tr("Filter..."));  // cn:过滤...
    mFilter->setClearButtonEnabled(true);
    mFilter->setVisible(false);
    root->insertWidget(0, mFilter);
    QObject::connect(mFilter, &QLineEdit::textChanged, q, [ this ](const QString& text) { applyFilter(text); });

    // 空态标签与列表互斥显隐（不插占位 item）
    mEmpty = new QLabel(mEmptyText, q);
    mEmpty->setObjectName(QStringLiteral("emptyLabel"));
    mEmpty->setAlignment(Qt::AlignCenter);
    mEmpty->setWordWrap(true);
    mEmpty->setVisible(false);
    root->insertWidget(1, mEmpty, 1);

    mList = new ItemListWidget(q, [ this ](int from, int to) { moveRow(from, to); });
    mList->setObjectName(QStringLiteral("itemList"));
    mList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 行文本由 setItemWidget 的行控件显示；delegate 再画一遍会在手柄把 label 挤右后透出重影，
    // 这里把 item 文字色设为透明（item->text() 仍保留，selectedItems()/过滤继续读它）
    QPalette listPalette = mList->palette();
    listPalette.setColor(QPalette::Text, Qt::transparent);
    listPalette.setColor(QPalette::HighlightedText, Qt::transparent);
    mList->setPalette(listPalette);
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
    mUpButton = new QPushButton(q->tr("Up"), q);  // cn:上移
    mUpButton->setObjectName(QStringLiteral("upButton"));
    mUpButton->setVisible(false);
    QObject::connect(mUpButton, &QPushButton::clicked, q, [ this ]() { moveCurrent(-1); });
    bar->addWidget(mUpButton);
    mDownButton = new QPushButton(q->tr("Down"), q);  // cn:下移
    mDownButton->setObjectName(QStringLiteral("downButton"));
    mDownButton->setVisible(false);
    QObject::connect(mDownButton, &QPushButton::clicked, q, [ this ]() { moveCurrent(1); });
    bar->addWidget(mDownButton);
    mRemoveAllButton = new QPushButton(mRemoveAllText, q);
    mRemoveAllButton->setObjectName(QStringLiteral("removeAllButton"));
    QObject::connect(mRemoveAllButton, &QPushButton::clicked, q, [ this ]() { onRemoveAllClicked(); });
    bar->addWidget(mRemoveAllButton);
    bar->addStretch(1);
    root->addLayout(bar);
    QObject::connect(mList, &QListWidget::currentRowChanged, q, [ this ](int) { updateReorderButtons(); });
    updateReorderButtons();
    updateEmptyState();
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
    updateRowDecorations(mList->row(item));
    applyFilter(mFilter->text());
    updateReorderButtons();
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
    applyFilter(mFilter->text());
    updateReorderButtons();
}

void DAItemListEditWidget::PrivateData::moveCurrent(int offset)
{
    DA_Q(DAItemListEditWidget, q);
    const int row = mList->currentRow();
    const int dst = row + offset;
    if (row < 0 || dst < 0 || dst >= mList->count()) {
        return;
    }
    moveRow(row, dst);
    mList->setCurrentRow(dst);
    updateReorderButtons();
    Q_EMIT q->selectedItemsChanged();
}

/**
 * @brief 把 from 行的 item（连带行控件）移动到 to
 * @param from 源行
 * @param to 目标最终索引
 * @details 顺序照抄旧 moveCurrent：takeItem 之后仍能取到 itemWidget；重插后挂回。
 *          不发 selectedItemsChanged，只发 orderChanged。
 */
void DAItemListEditWidget::PrivateData::moveRow(int from, int to)
{
    DA_Q(DAItemListEditWidget, q);
    if (from < 0 || from >= mList->count() || to < 0 || to >= mList->count() || from == to)
        return;
    QListWidgetItem* item = mList->takeItem(from);
    if (!item)
        return;
    // takeItem 会解除 itemWidget 关联，重插后需重新挂回行控件
    QWidget* w = mList->itemWidget(item);
    if (w) {
        mList->removeItemWidget(item);
    }
    mList->insertItem(to, item);
    if (w) {
        mList->setItemWidget(item, w);
    }
    // 行号已变化，隐藏状态必须按当前过滤词重算
    applyFilter(mFilter->text());
    Q_EMIT q->orderChanged(q->selectedItems());
}

/**
 * @brief 过滤：只设置行的隐藏状态，不改已选集合
 * @param key 过滤文本（去首尾空白，大小写不敏感，子串匹配）
 */
void DAItemListEditWidget::PrivateData::applyFilter(const QString& key)
{
    const QString k = key.trimmed();
    for (int i = 0; i < mList->count(); ++i) {
        QListWidgetItem* item = mList->item(i);
        const bool hit = k.isEmpty()
                         || DAStringFilterProxyModel::matchesText(item->text(), k, Qt::CaseInsensitive);
        mList->setRowHidden(i, !hit);
    }
    updateEmptyState();
}

/** @brief 空态与列表互斥显隐（0 条已选或过滤无命中） */
void DAItemListEditWidget::PrivateData::updateEmptyState()
{
    int visible = 0;
    for (int i = 0; i < mList->count(); ++i) {
        if (!mList->isRowHidden(i) && !mList->item(i)->isHidden())
            ++visible;
    }
    const bool empty = visible == 0;
    if (mEmpty) {
        mEmpty->setText(mEmptyText);
        mEmpty->setVisible(empty);
    }
    mList->setVisible(!empty);
}

/**
 * @brief 按 mDragReorderEnabled 给行增删手柄
 * @param row 行号
 */
void DAItemListEditWidget::PrivateData::updateRowDecorations(int row)
{
    QListWidgetItem* item = mList->item(row);
    if (!item)
        return;
    QWidget* rowWidget = mList->itemWidget(item);
    if (!rowWidget)
        return;
    // RowHandle 没有 Q_OBJECT，不能用 findChild<RowHandle*>（会被当成 QWidget 匹配）；
    // 用 objectName + 直接子控件过滤
    QWidget* handle = rowWidget->findChild< QWidget* >(QStringLiteral("rowHandle"), Qt::FindDirectChildrenOnly);
    if (mDragReorderEnabled && !handle) {
        if (auto* lay = qobject_cast< QHBoxLayout* >(rowWidget->layout())) {
            auto* h = new RowHandle(mList, item, rowWidget);
            lay->insertWidget(0, h);
        }
    } else if (!mDragReorderEnabled && handle) {
        handle->hide();
        delete handle;
    }
}

/** @brief 还原「再次点击清空」的按钮文案与武装状态 */
void DAItemListEditWidget::PrivateData::disarmClear()
{
    if (mArmTimer.isActive())
        mArmTimer.stop();
    if (mArmed) {
        mArmed = false;
        mRemoveAllButton->setText(mRemoveAllText);
    }
}

void DAItemListEditWidget::PrivateData::updateReorderButtons()
{
    if (!mUpButton || !mDownButton) {
        return;
    }
    mUpButton->setVisible(mReorderEnabled);
    mDownButton->setVisible(mReorderEnabled);
    const int row = mList->currentRow();
    mUpButton->setEnabled(mReorderEnabled && row > 0);
    mDownButton->setEnabled(mReorderEnabled && row >= 0 && row < mList->count() - 1);
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
    if (!mConfirmClearEnabled) {
        // 原有模态确认路径，一字不改
        if (QMessageBox::Yes
            != QMessageBox::question(q, mConfirmTitle, mConfirmText, QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No)) {
            return;
        }
        while (mList->count() > 0) {
            removeRow(0);
        }
        Q_EMIT q->selectedItemsChanged();
        return;
    }
    if (!mArmed) {
        // 首次点击只武装：换文案 + 起定时器，不发信号
        mArmed = true;
        mRemoveAllButton->setText(mConfirmClearText);
        mArmTimer.start(mConfirmClearInterval);
        return;
    }
    mArmTimer.stop();
    mArmed = false;
    mRemoveAllButton->setText(mRemoveAllText);
    while (mList->count() > 0) {
        removeRow(0);
    }
    Q_EMIT q->selectedItemsChanged();
}

DAItemListEditWidget::DAItemListEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    // 信号参数是自定义类型：注册一次，QSignalSpy / 队列连接才能解析。
    // Qt5 的 moc 在命名空间内把签名记成 "QVector<DAItemEntry>"，Qt6 记全限定名，
    // 两个名字都注册到同一 id，跨版本才都能按签名查到。
    static const int entryVectorTypeId = qRegisterMetaType< QVector< DAItemEntry > >("QVector<DA::DAItemEntry>");
    qRegisterMetaType< QVector< DAItemEntry > >("QVector<DAItemEntry>");
    Q_UNUSED(entryVectorTypeId);
    DA_D(d);
    d->init();
}

DAItemListEditWidget::~DAItemListEditWidget()
{
    DA_D(d);
    d->disarmClear();
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
    d->disarmClear();
    while (d->mList->count() > 0) {
        d->removeRow(0);
    }
    for (const DAItemEntry& e : items) {
        d->appendRow(e);
    }
    d->applyFilter(d->mFilter->text());
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

/**
 * @brief 显示 Up/Down 按钮（按钮移动当前行；与手柄拖拽互不依赖）
 * @param on 是否启用排序
 */
void DAItemListEditWidget::setReorderEnabled(bool on)
{
    DA_D(d);
    d->mReorderEnabled = on;
    d->updateReorderButtons();
}

/**
 * @brief 是否启用排序
 * @return true = 显示 Up/Down 按钮
 */
bool DAItemListEditWidget::isReorderEnabled() const
{
    DA_DC(d);
    return d->mReorderEnabled;
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
    // 武装态显示的是「再次点击清空」，基础文案等 disarm 后再生效
    if (!d->mArmed)
        d->mRemoveAllButton->setText(t);
}

void DAItemListEditWidget::setRemoveAllConfirmText(const QString& title, const QString& text)
{
    DA_D(d);
    d->mConfirmTitle = title;
    d->mConfirmText  = text;
}

/**
 * @brief 打开/关闭行首手柄拖拽排序
 * @param on 是否启用（默认 false）
 * @details 只把列表设为 DropOnly（当落点），拖源是手柄自己发起的 QDrag
 */
void DAItemListEditWidget::setDragReorderEnabled(bool on)
{
    DA_D(d);
    if (d->mDragReorderEnabled == on)
        return;
    d->mDragReorderEnabled = on;
    d->mList->setDragDropMode(on ? QAbstractItemView::DropOnly : QAbstractItemView::NoDragDrop);
    for (int i = 0; i < d->mList->count(); ++i)
        d->updateRowDecorations(i);
}

/** @brief 是否启用行首手柄拖拽 */
bool DAItemListEditWidget::isDragReorderEnabled() const
{
    DA_DC(d);
    return d->mDragReorderEnabled;
}

/**
 * @brief 显示/隐藏顶部过滤框
 * @param on 是否显示（默认 false）
 * @details 关闭时清空过滤文本并恢复全部行
 */
void DAItemListEditWidget::setFilterVisible(bool on)
{
    DA_D(d);
    if (d->mFilterVisible == on)
        return;
    d->mFilterVisible = on;
    if (!on)
        d->mFilter->clear();  // 触发 applyFilter("")
    d->mFilter->setVisible(on);
    d->applyFilter(on ? d->mFilter->text() : QString());
}

/** @brief 过滤框是否显示 */
bool DAItemListEditWidget::isFilterVisible() const
{
    DA_DC(d);
    return d->mFilterVisible;
}

/**
 * @brief 设置空态文案
 * @param text 文案
 */
void DAItemListEditWidget::setEmptyText(const QString& text)
{
    DA_D(d);
    d->mEmptyText = text;
    d->updateEmptyState();
}

/** @brief 获取空态文案 */
QString DAItemListEditWidget::emptyText() const
{
    DA_DC(d);
    return d->mEmptyText;
}

/**
 * @brief 打开/关闭「再次点击清空」
 * @param on 是否启用（默认 false = 保持原有 QMessageBox）
 */
void DAItemListEditWidget::setConfirmClearEnabled(bool on)
{
    DA_D(d);
    if (d->mConfirmClearEnabled == on)
        return;
    d->mConfirmClearEnabled = on;
    if (!on)
        d->disarmClear();
}

/** @brief 是否启用「再次点击清空」 */
bool DAItemListEditWidget::isConfirmClearEnabled() const
{
    DA_DC(d);
    return d->mConfirmClearEnabled;
}

/**
 * @brief 设置武装态自动还原时间
 * @param ms 毫秒，最小 200
 */
void DAItemListEditWidget::setConfirmClearInterval(int ms)
{
    DA_D(d);
    d->mConfirmClearInterval = qMax(200, ms);
    if (d->mArmed)
        d->mArmTimer.start(d->mConfirmClearInterval);
}

/** @brief 获取武装态自动还原时间（毫秒） */
int DAItemListEditWidget::confirmClearInterval() const
{
    DA_DC(d);
    return d->mConfirmClearInterval;
}

/**
 * @brief 设置武装态按钮文案
 * @param text 文案（i18n 覆盖用）
 */
void DAItemListEditWidget::setConfirmClearText(const QString& text)
{
    DA_D(d);
    d->mConfirmClearText = text;
    if (d->mArmed)
        d->mRemoveAllButton->setText(text);
}

}  // namespace DA
