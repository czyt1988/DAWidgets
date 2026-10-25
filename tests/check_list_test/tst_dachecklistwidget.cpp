#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
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

private:
    static QVector< DAItemEntry > makeEntries(const QStringList& texts);
    static QListWidget* listOf(DACheckListWidget& w);
    static QPushButton* buttonOf(DACheckListWidget& w, const QString& name);
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

QListWidget* DaCheckListWidgetTest::listOf(DACheckListWidget& w)
{
    return w.findChild< QListWidget* >(QStringLiteral("checkList"));
}

QPushButton* DaCheckListWidgetTest::buttonOf(DACheckListWidget& w, const QString& name)
{
    return w.findChild< QPushButton* >(name);
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
    QListWidget* list = listOf(w);
    QVERIFY(list);
    list->item(1)->setCheckState(Qt::Unchecked);
    QCOMPARE(list->item(1)->checkState(), Qt::Checked);
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
    // 过滤只隐藏行，勾选状态保留
    QCOMPARE(w.checkedCount(), 2);
    QVERIFY(listOf(w)->isRowHidden(0));
    QVERIFY(!listOf(w)->isRowHidden(1));
    QVERIFY(listOf(w)->isRowHidden(2));

    search->clear();
    QCOMPARE(w.checkedCount(), 2);
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
    listOf(w)->item(0)->setCheckState(Qt::Checked);
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
    QVERIFY(!listOf(w)->item(0)->flags().testFlag(Qt::ItemIsUserCheckable));
}

QTEST_MAIN(DaCheckListWidgetTest)
#include "tst_dachecklistwidget.moc"
