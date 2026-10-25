#include <QListWidget>
#include <QPushButton>
#include <QtTest/QtTest>

#include <DAStringListEditWidget.h>

using namespace DA;

class DaStringListEditWidgetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testInlineAdd();
    void testExternalAdd();
    void testAppendItem();
    void testReorder();
    void testEditable();
    void testMaxCount();
    void testRemove();
    void testSetItemsDoesNotEmit();

private:
    static QListWidget* listOf(DAStringListEditWidget& w);
    static QPushButton* buttonOf(DAStringListEditWidget& w, const QString& name);
};

QListWidget* DaStringListEditWidgetTest::listOf(DAStringListEditWidget& w)
{
    return w.findChild< QListWidget* >(QStringLiteral("stringList"));
}

QPushButton* DaStringListEditWidgetTest::buttonOf(DAStringListEditWidget& w, const QString& name)
{
    return w.findChild< QPushButton* >(name);
}

void DaStringListEditWidgetTest::testInlineAdd()
{
    DAStringListEditWidget w;
    w.setItems({});
    QSignalSpy spy(&w, &DAStringListEditWidget::itemsChanged);
    QPushButton* add = buttonOf(w, QStringLiteral("addButton"));
    QVERIFY(add);
    add->click();
    // Inline：追加空行（不插入 "New Item" 占位文本）
    QCOMPARE(w.items().size(), 1);
    QVERIFY(w.items().first().isEmpty());
    QCOMPARE(spy.count(), 1);
}

void DaStringListEditWidgetTest::testExternalAdd()
{
    DAStringListEditWidget w;
    w.setAddMode(DAStringListEditWidget::AddMode::External);
    QCOMPARE(w.addMode(), DAStringListEditWidget::AddMode::External);
    QSignalSpy addSpy(&w, &DAStringListEditWidget::addRequested);
    QSignalSpy changedSpy(&w, &DAStringListEditWidget::itemsChanged);
    buttonOf(w, QStringLiteral("addButton"))->click();
    // External：只发 addRequested，不动列表
    QCOMPARE(addSpy.count(), 1);
    QCOMPARE(changedSpy.count(), 0);
    QVERIFY(w.items().isEmpty());
}

void DaStringListEditWidgetTest::testAppendItem()
{
    DAStringListEditWidget w;
    QSignalSpy spy(&w, &DAStringListEditWidget::itemsChanged);
    w.appendItem(QStringLiteral("x"));
    QCOMPARE(w.items(), QStringList { QStringLiteral("x") });
    QCOMPARE(spy.count(), 1);
}

void DaStringListEditWidgetTest::testReorder()
{
    DAStringListEditWidget w;
    w.setReorderEnabled(true);
    QVERIFY(w.isReorderEnabled());
    w.setItems({ QStringLiteral("a"), QStringLiteral("b") });
    listOf(w)->setCurrentRow(1);
    QPushButton* up = buttonOf(w, QStringLiteral("upButton"));
    QVERIFY(up);
    up->click();
    QCOMPARE(w.items(), QStringList({ QStringLiteral("b"), QStringLiteral("a") }));
    QCOMPARE(listOf(w)->currentRow(), 0);
    QPushButton* down = buttonOf(w, QStringLiteral("downButton"));
    down->click();
    QCOMPARE(w.items(), QStringList({ QStringLiteral("a"), QStringLiteral("b") }));
}

void DaStringListEditWidgetTest::testEditable()
{
    DAStringListEditWidget w;
    w.setItems({ QStringLiteral("a") });
    QVERIFY(w.isEditable());
    QVERIFY(listOf(w)->item(0)->flags().testFlag(Qt::ItemIsEditable));
    w.setEditable(false);
    QVERIFY(!w.isEditable());
    QVERIFY(!listOf(w)->item(0)->flags().testFlag(Qt::ItemIsEditable));
    w.setEditable(true);
    QVERIFY(listOf(w)->item(0)->flags().testFlag(Qt::ItemIsEditable));
}

void DaStringListEditWidgetTest::testMaxCount()
{
    DAStringListEditWidget w;
    w.setMaxCount(2);
    QCOMPARE(w.maxCount(), 2);
    QPushButton* add = buttonOf(w, QStringLiteral("addButton"));
    w.setItems({ QStringLiteral("a") });
    QVERIFY(add->isEnabled());
    w.appendItem(QStringLiteral("b"));
    QVERIFY(!add->isEnabled());
    // 超限追加被忽略
    w.appendItem(QStringLiteral("c"));
    QCOMPARE(w.items().size(), 2);
}

void DaStringListEditWidgetTest::testRemove()
{
    DAStringListEditWidget w;
    w.setItems({ QStringLiteral("a"), QStringLiteral("b") });
    QSignalSpy spy(&w, &DAStringListEditWidget::itemsChanged);
    listOf(w)->setCurrentRow(0);
    buttonOf(w, QStringLiteral("removeButton"))->click();
    QCOMPARE(w.items(), QStringList { QStringLiteral("b") });
    QCOMPARE(spy.count(), 1);
}

void DaStringListEditWidgetTest::testSetItemsDoesNotEmit()
{
    DAStringListEditWidget w;
    QSignalSpy spy(&w, &DAStringListEditWidget::itemsChanged);
    w.setItems({ QStringLiteral("a"), QStringLiteral("b") });
    QCOMPARE(spy.count(), 0);
    QCOMPARE(w.items().size(), 2);
}

QTEST_MAIN(DaStringListEditWidgetTest)
#include "tst_dastringlistedwidget.moc"
