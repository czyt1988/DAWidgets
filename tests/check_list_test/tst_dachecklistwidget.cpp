#include <QAbstractItemModel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QtTest/QtTest>

#include <DACheckListWidget.h>

using namespace DA;

class DaCheckListWidgetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testForcedCannotBeUnchecked();
    void testSelectAllSkipsDisabledAndForced();
    void testInvertSkipsForced();
    void testFilterKeepsChecked();
    void testSetItemsDoesNotEmit();
    void testUserChangeEmitsOnce();
    void testUncheckableIsNeverChecked();
    void testKeyboardSelectionDisabledByDefault();
    void testShiftRangeChecksInclusive();
    void testShiftRangeUnchecksWhenAnchorUnchecked();
    void testShiftRangeSkipsForcedDisabledUncheckable();
    void testCtrlASelectsVisibleOnly();
    void testAnchorResetAfterFilterChange();
    void testRangeEmitsOnce();

private:
    static QVector< DAItemEntry > makeEntries(const QStringList& texts);
    static QListView* listOf(DACheckListWidget& w);
    static QAbstractItemModel* modelOf(DACheckListWidget& w);
    static QPushButton* buttonOf(DACheckListWidget& w, const QString& name);
    static Qt::CheckState checkStateOf(QAbstractItemModel* m, int row);
    static void setCheckStateOf(QAbstractItemModel* m, int row, Qt::CheckState state);
    static void clickRow(DACheckListWidget& w, int proxyRow, Qt::KeyboardModifiers mods = Qt::NoModifier);
    static QPoint checkPoint(QListView* list, const QModelIndex& index);
    static void showWidget(DACheckListWidget& w);
};

QVector< DAItemEntry > DaCheckListWidgetTest::makeEntries(const QStringList& texts)
{
    QVector< DAItemEntry > res;
    res.reserve(texts.size());
    for (const QString& t : texts) {
        res.append(DAItemEntry { t, QVariant() });
    }
    return res;
}

QListView* DaCheckListWidgetTest::listOf(DACheckListWidget& w)
{
    return w.findChild< QListView* >(QStringLiteral("checkList"));
}

QAbstractItemModel* DaCheckListWidgetTest::modelOf(DACheckListWidget& w)
{
    QListView* view = listOf(w);
    return view ? view->model() : nullptr;
}

QPushButton* DaCheckListWidgetTest::buttonOf(DACheckListWidget& w, const QString& name)
{
    return w.findChild< QPushButton* >(name);
}

Qt::CheckState DaCheckListWidgetTest::checkStateOf(QAbstractItemModel* m, int row)
{
    return static_cast< Qt::CheckState >(m->index(row, 0).data(Qt::CheckStateRole).toInt());
}

void DaCheckListWidgetTest::setCheckStateOf(QAbstractItemModel* m, int row, Qt::CheckState state)
{
    m->setData(m->index(row, 0), state, Qt::CheckStateRole);
}

QPoint DaCheckListWidgetTest::checkPoint(QListView* list, const QModelIndex& index)
{
    QStyleOptionViewItem opt;
    opt.initFrom(list);
    opt.rect = list->visualRect(index);
    opt.features = QStyleOptionViewItem::HasCheckIndicator;
    opt.checkState = static_cast< Qt::CheckState >(index.data(Qt::CheckStateRole).toInt());
    opt.text = index.data(Qt::DisplayRole).toString();
    opt.widget = list;
    const QRect r = list->style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt, list);
    if (r.isValid() && !r.isEmpty())
        return r.center();
    return opt.rect.center();
}

void DaCheckListWidgetTest::clickRow(DACheckListWidget& w, int proxyRow, Qt::KeyboardModifiers mods)
{
    QListView* list = listOf(w);
    QVERIFY(list);
    QAbstractItemModel* m = list->model();
    QVERIFY(m);
    const QModelIndex idx = m->index(proxyRow, 0);
    QVERIFY(idx.isValid());
    QTest::mouseClick(list->viewport(), Qt::LeftButton, mods, checkPoint(list, idx));
}

void DaCheckListWidgetTest::showWidget(DACheckListWidget& w)
{
    w.resize(320, 320);
    w.show();
    QTest::qWaitForWindowExposed(&w);
    w.activateWindow();
    QTest::qWaitForWindowActive(&w);
    QTest::qWait(20);
}

void DaCheckListWidgetTest::testForcedCannotBeUnchecked()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "a", "b", "c" }));
    const DAItemEntry b { "b", QVariant() };
    w.setEntryForcedChecked(b, true);
    QCOMPARE(w.checkedCount(), 1);
    QVERIFY(w.checkedEntries().first() == b);

    // 模拟用户取消 forced 条目：控件应立刻恢复勾选
    QAbstractItemModel* m = modelOf(w);
    QVERIFY(m);
    setCheckStateOf(m, 1, Qt::Unchecked);
    QCOMPARE(checkStateOf(m, 1), Qt::Checked);
    QCOMPARE(w.checkedCount(), 1);
}

void DaCheckListWidgetTest::testSelectAllSkipsDisabledAndForced()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "a", "b", "c" }));
    const DAItemEntry a { "a", QVariant() };
    const DAItemEntry c { "c", QVariant() };
    w.setEntryForcedChecked(c, true);
    w.setEntryEnabled(a, false);

    QPushButton* selectAll = buttonOf(w, QStringLiteral("selectAllButton"));
    QVERIFY(selectAll);
    selectAll->click();
    // a 禁用、c forced：只有 b 被全选勾上；c 始终勾选
    QCOMPARE(w.checkedCount(), 2);
    const QVector< DAItemEntry > checked = w.checkedEntries();
    QVERIFY(checked.contains(DAItemEntry { "b", QVariant() }));
    QVERIFY(checked.contains(c));
    QVERIFY(!checked.contains(a));
}

void DaCheckListWidgetTest::testInvertSkipsForced()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "a", "b" }));
    const DAItemEntry a { "a", QVariant() };
    const DAItemEntry b { "b", QVariant() };
    w.setEntryForcedChecked(a, true);
    w.setCheckedEntries({ b });

    QPushButton* invert = buttonOf(w, QStringLiteral("invertButton"));
    QVERIFY(invert);
    invert->click();
    // b 被反选取消，a（forced）保持勾选
    QCOMPARE(w.checkedCount(), 1);
    QVERIFY(w.checkedEntries().first() == a);
}

void DaCheckListWidgetTest::testFilterKeepsChecked()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "Apple", "Banana", "Cherry" }));
    const DAItemEntry apple { "Apple", QVariant() };
    const DAItemEntry cherry { "Cherry", QVariant() };
    w.setCheckedEntries({ apple, cherry });
    QCOMPARE(w.checkedCount(), 2);

    QLineEdit* search = w.findChild< QLineEdit* >(QStringLiteral("searchEdit"));
    QVERIFY(search);
    search->setText(QStringLiteral("ban"));
    // 过滤只影响可见行，勾选状态保留
    QCOMPARE(w.checkedCount(), 2);
    QAbstractItemModel* m = modelOf(w);
    QCOMPARE(m->rowCount(), 1);
    QCOMPARE(m->index(0, 0).data(Qt::DisplayRole).toString(), QStringLiteral("Banana"));

    search->clear();
    QCOMPARE(w.checkedCount(), 2);
    QCOMPARE(modelOf(w)->rowCount(), 3);
}

void DaCheckListWidgetTest::testSetItemsDoesNotEmit()
{
    DACheckListWidget w;
    QSignalSpy spy(&w, &DACheckListWidget::checkedEntriesChanged);
    w.setItems(makeEntries({ "a", "b" }));
    w.setCheckedEntries({ DAItemEntry { "a", QVariant() } });
    QCOMPARE(spy.count(), 0);
}

void DaCheckListWidgetTest::testUserChangeEmitsOnce()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "a", "b" }));
    QSignalSpy spy(&w, &DACheckListWidget::checkedEntriesChanged);
    setCheckStateOf(modelOf(w), 0, Qt::Checked);
    QCOMPARE(spy.count(), 1);
    // 全选合并为一次信号
    QPushButton* selectAll = buttonOf(w, QStringLiteral("selectAllButton"));
    selectAll->click();
    QCOMPARE(spy.count(), 2);
}

void DaCheckListWidgetTest::testUncheckableIsNeverChecked()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "a", "b" }));
    const DAItemEntry a { "a", QVariant() };
    w.setEntryCheckable(a, false);
    w.setCheckedEntries({ a });
    QCOMPARE(w.checkedCount(), 0);
    QAbstractItemModel* m = modelOf(w);
    QVERIFY(!(m->flags(m->index(0, 0)) & Qt::ItemIsUserCheckable));
}

void DaCheckListWidgetTest::testKeyboardSelectionDisabledByDefault()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "a", "b", "c", "d" }));
    showWidget(w);
    QVERIFY(!w.keyboardSelectionEnabled());
    clickRow(w, 2);
    QCOMPARE(w.checkedEntries().size(), 1);
    QCOMPARE(w.checkedEntries().first().text, QStringLiteral("c"));
}

void DaCheckListWidgetTest::testShiftRangeChecksInclusive()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "a", "b", "c", "d" }));
    showWidget(w);
    w.setKeyboardSelectionEnabled(true);
    QSignalSpy spy(&w, &DACheckListWidget::checkedEntriesChanged);

    setCheckStateOf(modelOf(w), 0, Qt::Checked);
    QCOMPARE(spy.count(), 1);
    clickRow(w, 3, Qt::ShiftModifier);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(w.checkedEntries().size(), 4);
    for (int i = 0; i < 4; ++i)
        QCOMPARE(checkStateOf(modelOf(w), i), Qt::Checked);
}

void DaCheckListWidgetTest::testShiftRangeUnchecksWhenAnchorUnchecked()
{
    DACheckListWidget w;
    const QVector< DAItemEntry > items = makeEntries({ "a", "b", "c", "d", "e" });
    w.setItems(items);
    w.setCheckedEntries(items);
    showWidget(w);
    w.setKeyboardSelectionEnabled(true);
    QSignalSpy spy(&w, &DACheckListWidget::checkedEntriesChanged);

    setCheckStateOf(modelOf(w), 1, Qt::Unchecked);
    QCOMPARE(spy.count(), 1);
    clickRow(w, 4, Qt::ShiftModifier);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(w.checkedEntries().size(), 1);
    QCOMPARE(w.checkedEntries().first().text, QStringLiteral("a"));
    for (int i = 1; i < 5; ++i)
        QCOMPARE(checkStateOf(modelOf(w), i), Qt::Unchecked);
}

void DaCheckListWidgetTest::testShiftRangeSkipsForcedDisabledUncheckable()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "a", "b", "c", "d", "e" }));
    w.setEntryForcedChecked(DAItemEntry { "b", QVariant() }, true);
    w.setEntryEnabled(DAItemEntry { "c", QVariant() }, false);
    w.setEntryCheckable(DAItemEntry { "d", QVariant() }, false);
    showWidget(w);
    w.setKeyboardSelectionEnabled(true);

    setCheckStateOf(modelOf(w), 0, Qt::Checked);
    clickRow(w, 4, Qt::ShiftModifier);

    const QVector< DAItemEntry > checked = w.checkedEntries();
    QCOMPARE(checked.size(), 3);
    QVERIFY(checked.contains(DAItemEntry { "a", QVariant() }));
    QVERIFY(checked.contains(DAItemEntry { "b", QVariant() }));
    QVERIFY(checked.contains(DAItemEntry { "e", QVariant() }));
    QCOMPARE(checkStateOf(modelOf(w), 2), Qt::Unchecked);
    QCOMPARE(checkStateOf(modelOf(w), 3), Qt::Unchecked);
}

void DaCheckListWidgetTest::testCtrlASelectsVisibleOnly()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "Apple", "Banana", "Date", "Cherry", "Elder" }));
    showWidget(w);
    w.setKeyboardSelectionEnabled(true);
    QLineEdit* search = w.findChild< QLineEdit* >(QStringLiteral("searchEdit"));
    QVERIFY(search);
    search->setText(QStringLiteral("a"));
    QCOMPARE(modelOf(w)->rowCount(), 3);

    QListView* list = listOf(w);
    QVERIFY(list);
    list->setFocus();
    QTest::keyClick(list->viewport(), Qt::Key_A, Qt::ControlModifier);
    QCOMPARE(w.checkedCount(), 3);

    search->clear();
    QCOMPARE(w.checkedCount(), 3);
}

void DaCheckListWidgetTest::testAnchorResetAfterFilterChange()
{
    DACheckListWidget w;
    w.setItems(makeEntries({ "alpha", "beta", "gamma" }));
    showWidget(w);
    w.setKeyboardSelectionEnabled(true);
    QAbstractItemModel* m = modelOf(w);
    setCheckStateOf(m, 0, Qt::Checked);
    setCheckStateOf(m, 0, Qt::Unchecked);

    QLineEdit* search = w.findChild< QLineEdit* >(QStringLiteral("searchEdit"));
    QVERIFY(search);
    search->setText(QStringLiteral("beta"));
    QCOMPARE(m->rowCount(), 1);

    clickRow(w, 0, Qt::ShiftModifier);
    QCOMPARE(m->rowCount(), 1);
    QCOMPARE(checkStateOf(m, 0), Qt::Checked);
}

void DaCheckListWidgetTest::testRangeEmitsOnce()
{
    DACheckListWidget w;
    QStringList texts;
    for (int i = 0; i < 10; ++i)
        texts << QStringLiteral("item%1").arg(i);
    w.setItems(makeEntries(texts));
    showWidget(w);
    w.setKeyboardSelectionEnabled(true);
    setCheckStateOf(modelOf(w), 0, Qt::Checked);

    QSignalSpy spy(&w, &DACheckListWidget::checkedEntriesChanged);
    clickRow(w, 9, Qt::ShiftModifier);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(w.checkedCount(), 10);
}

QTEST_MAIN(DaCheckListWidgetTest)
#include "tst_dachecklistwidget.moc"
