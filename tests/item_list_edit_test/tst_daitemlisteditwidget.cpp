#include <QListWidget>
#include <QPushButton>
#include <QtTest/QtTest>

#include <DAItemListEditWidget.h>

using namespace DA;

class DaItemListEditWidgetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testSelectedItemsRoundTrip();
    void testSetSelectedItemsDoesNotEmit();
    void testReorderButtonsFollowFlag();
    void testMoveUpDown();
    void testReorderEmits();

private:
    static QVector< DAItemEntry > makeEntries(const QStringList& texts);
    static QStringList textsOf(const DAItemListEditWidget& w);
    static QListWidget* listOf(DAItemListEditWidget& w);
};

QVector< DAItemEntry > DaItemListEditWidgetTest::makeEntries(const QStringList& texts)
{
    QVector< DAItemEntry > res;
    for (const QString& t : texts) {
        res.append(DAItemEntry { t, QVariant() });
    }
    return res;
}

QStringList DaItemListEditWidgetTest::textsOf(const DAItemListEditWidget& w)
{
    QStringList res;
    const QVector< DAItemEntry > items = w.selectedItems();
    for (const DAItemEntry& e : items) {
        res << e.text;
    }
    return res;
}

QListWidget* DaItemListEditWidgetTest::listOf(DAItemListEditWidget& w)
{
    return w.findChild< QListWidget* >(QStringLiteral("itemList"));
}

void DaItemListEditWidgetTest::testSelectedItemsRoundTrip()
{
    DAItemListEditWidget w;
    w.setSelectedItems(makeEntries({ "a", "b", "c" }));
    QCOMPARE(w.selectedItems().size(), 3);
    QCOMPARE(textsOf(w), (QStringList { "a", "b", "c" }));
}

void DaItemListEditWidgetTest::testSetSelectedItemsDoesNotEmit()
{
    DAItemListEditWidget w;
    QSignalSpy spy(&w, &DAItemListEditWidget::selectedItemsChanged);
    w.setSelectedItems(makeEntries({ "a", "b" }));
    QCOMPARE(spy.count(), 0);
}

void DaItemListEditWidgetTest::testReorderButtonsFollowFlag()
{
    DAItemListEditWidget w;
    QPushButton* up = w.findChild< QPushButton* >(QStringLiteral("upButton"));
    QPushButton* down = w.findChild< QPushButton* >(QStringLiteral("downButton"));
    QVERIFY(up);
    QVERIFY(down);
    QVERIFY(!w.isReorderEnabled());
    QVERIFY(up->isHidden());
    QVERIFY(down->isHidden());

    w.setReorderEnabled(true);
    QVERIFY(w.isReorderEnabled());
    QVERIFY(!up->isHidden());
    QVERIFY(!down->isHidden());

    w.setSelectedItems(makeEntries({ "a", "b" }));
    listOf(w)->setCurrentRow(0);
    QVERIFY(!up->isEnabled());    // 第一行不能上移
    QVERIFY(down->isEnabled());
    listOf(w)->setCurrentRow(1);
    QVERIFY(up->isEnabled());
    QVERIFY(!down->isEnabled());  // 最后一行不能下移
}

void DaItemListEditWidgetTest::testMoveUpDown()
{
    DAItemListEditWidget w;
    w.setReorderEnabled(true);
    w.setSelectedItems(makeEntries({ "a", "b", "c" }));
    QListWidget* list = listOf(w);
    list->setCurrentRow(1);
    w.findChild< QPushButton* >(QStringLiteral("upButton"))->click();
    QCOMPARE(textsOf(w), (QStringList { "b", "a", "c" }));
    QCOMPARE(list->currentRow(), 0);
    w.findChild< QPushButton* >(QStringLiteral("downButton"))->click();
    QCOMPARE(textsOf(w), (QStringList { "a", "b", "c" }));
    QCOMPARE(list->currentRow(), 1);
}

void DaItemListEditWidgetTest::testReorderEmits()
{
    DAItemListEditWidget w;
    w.setReorderEnabled(true);
    w.setSelectedItems(makeEntries({ "a", "b" }));
    QSignalSpy spy(&w, &DAItemListEditWidget::selectedItemsChanged);
    listOf(w)->setCurrentRow(1);
    w.findChild< QPushButton* >(QStringLiteral("upButton"))->click();
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(DaItemListEditWidgetTest)
#include "tst_daitemlisteditwidget.moc"
