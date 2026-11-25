#include <QApplication>
#include <QDragEnterEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMimeData>
#include <QDropEvent>
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
    void testDragReorderDefaultsOff();
    void testRowHandlePixels();
    void testDropEventMovesRow();
    void testDropSamePositionNoSignal();
    void testDropRejectsExternalMime();
    void testDropKeepsData();
    void testUpDownEmitsOrderChanged();
    void testFilterHidesRowsKeepsSelection();
    void testFilterMatchesLikeCheckList();
    void testEmptyStateVisibility();
    void testConfirmClearArming();

private:
    static QVector< DAItemEntry > makeEntries(const QStringList& texts);
    static QStringList textsOf(const DAItemListEditWidget& w);
    static QListWidget* listOf(DAItemListEditWidget& w);
    static QLineEdit* filterOf(DAItemListEditWidget& w);
    static QLabel* emptyOf(DAItemListEditWidget& w);
    static void showWidget(DAItemListEditWidget& w);
    static void dropRow(DAItemListEditWidget& w, int srcRow, int targetRow, bool after);
};

namespace {
const char kRowMime[] = "application/x-dawidgets-itemlist-row";
}

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

QLineEdit* DaItemListEditWidgetTest::filterOf(DAItemListEditWidget& w)
{
    return w.findChild< QLineEdit* >(QStringLiteral("filterEdit"));
}

QLabel* DaItemListEditWidgetTest::emptyOf(DAItemListEditWidget& w)
{
    return w.findChild< QLabel* >(QStringLiteral("emptyLabel"));
}

void DaItemListEditWidgetTest::showWidget(DAItemListEditWidget& w)
{
    w.resize(320, 260);
    w.show();
    QTest::qWaitForWindowExposed(&w);
    QTest::qWait(20);
}

void DaItemListEditWidgetTest::dropRow(DAItemListEditWidget& w, int srcRow, int targetRow, bool after)
{
    QListWidget* list = listOf(w);
    QVERIFY(list);
    const QRect r = list->visualRect(list->model()->index(targetRow, 0));
    const QPoint vp(r.center().x(), after ? r.bottom() - 1 : r.top() + 1);
    QMimeData mime;
    mime.setData(QString::fromLatin1(kRowMime), QByteArray::number(srcRow));
    // QApplication::notify 会把 Drop 路由到 QDragManager::currentTarget；
    // 先发 DragEnter 建立 currentTarget，Drop 才会沿 viewport → dropEvent 送达
    QDragEnterEvent enterEv(vp, Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(list->viewport(), &enterEv);
    QDropEvent ev(QPointF(vp), Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(list->viewport(), &ev);
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

void DaItemListEditWidgetTest::testDragReorderDefaultsOff()
{
    DAItemListEditWidget w;
    QVERIFY(!w.isDragReorderEnabled());
    QVERIFY(!w.isFilterVisible());
    QVERIFY(!w.isConfirmClearEnabled());
    QCOMPARE(w.confirmClearInterval(), 2500);

    w.setReorderEnabled(true);
    QVERIFY(!w.isDragReorderEnabled());  // 两个入口互不依赖

    w.setSelectedItems(makeEntries({ "a", "b" }));
    QListWidget* list = listOf(w);
    QVERIFY(list);
    for (int i = 0; i < list->count(); ++i) {
        QWidget* row = list->itemWidget(list->item(i));
        QVERIFY(row);
        QVERIFY(!row->findChild< QWidget* >(QStringLiteral("rowHandle")));
    }
}

void DaItemListEditWidgetTest::testRowHandlePixels()
{
    DAItemListEditWidget w;
    w.setSelectedItems(makeEntries({ "a", "b" }));
    showWidget(w);
    QListWidget* list = listOf(w);
    QVERIFY(list);
    QVERIFY(!list->itemWidget(list->item(0))->findChild< QWidget* >(QStringLiteral("rowHandle")));

    w.setDragReorderEnabled(true);
    QTest::qWait(20);
    for (int i = 0; i < list->count(); ++i) {
        QWidget* row = list->itemWidget(list->item(i));
        QVERIFY(row);
        QWidget* handle = row->findChild< QWidget* >(QStringLiteral("rowHandle"));
        QVERIFY(handle);
        QCOMPARE(handle->width(), 14);

        // 手柄底色不可控，先铺白再 render，只数手柄自己画的圆点
        QPixmap pm(handle->size());
        pm.fill(Qt::white);
        handle->render(&pm);
        const QImage img = pm.toImage();
        int dark = 0;
        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                const QRgb px = img.pixel(x, y);
                if (qRed(px) < 200 || qGreen(px) < 200 || qBlue(px) < 200)
                    ++dark;
            }
        }
        QVERIFY2(dark >= 6, qPrintable(QStringLiteral("row handle dark pixels=%1").arg(dark)));
    }
}

void DaItemListEditWidgetTest::testDropEventMovesRow()
{
    DAItemListEditWidget w;
    w.setSelectedItems(makeEntries({ "a", "b", "c" }));
    w.setDragReorderEnabled(true);
    showWidget(w);

    QSignalSpy orderSpy(&w, &DAItemListEditWidget::orderChanged);
    QSignalSpy selectionSpy(&w, &DAItemListEditWidget::selectedItemsChanged);
    dropRow(w, 2, 0, false);  // 第 3 行拖到第 1 行上缘

    QCOMPARE(textsOf(w), (QStringList { "c", "a", "b" }));
    QCOMPARE(orderSpy.count(), 1);
    QCOMPARE(selectionSpy.count(), 0);
}

void DaItemListEditWidgetTest::testDropSamePositionNoSignal()
{
    DAItemListEditWidget w;
    w.setSelectedItems(makeEntries({ "a", "b", "c" }));
    w.setDragReorderEnabled(true);
    showWidget(w);

    QSignalSpy orderSpy(&w, &DAItemListEditWidget::orderChanged);
    QSignalSpy selectionSpy(&w, &DAItemListEditWidget::selectedItemsChanged);
    dropRow(w, 1, 1, false);  // 落回原位

    QCOMPARE(textsOf(w), (QStringList { "a", "b", "c" }));
    QCOMPARE(orderSpy.count(), 0);
    QCOMPARE(selectionSpy.count(), 0);
}

void DaItemListEditWidgetTest::testDropRejectsExternalMime()
{
    DAItemListEditWidget w;
    w.setSelectedItems(makeEntries({ "a", "b" }));
    w.setDragReorderEnabled(true);
    showWidget(w);

    QListWidget* list = listOf(w);
    QMimeData mime;
    mime.setData(QStringLiteral("text/uri-list"), QByteArray("file:///tmp/a.txt"));
    QDragEnterEvent enterEv(QPoint(5, 5), Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(list->viewport(), &enterEv);
    QDropEvent ev(QPointF(5, 5), Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(list->viewport(), &ev);

    QVERIFY(!ev.isAccepted());
    QCOMPARE(textsOf(w), (QStringList { "a", "b" }));
}

void DaItemListEditWidgetTest::testDropKeepsData()
{
    DAItemListEditWidget w;
    QVector< DAItemEntry > entries;
    entries << DAItemEntry { QStringLiteral("a"), QVariant(7) };
    entries << DAItemEntry { QStringLiteral("b"), QVariant(9) };
    entries << DAItemEntry { QStringLiteral("c"), QVariant(11) };
    w.setSelectedItems(entries);
    w.setDragReorderEnabled(true);
    showWidget(w);

    dropRow(w, 2, 0, false);
    const QVector< DAItemEntry > sel = w.selectedItems();
    QCOMPARE(sel.size(), 3);
    QCOMPARE(sel.at(0).text, QStringLiteral("c"));
    QCOMPARE(sel.at(0).data.toInt(), 11);
    QCOMPARE(sel.at(1).data.toInt(), 7);
    QCOMPARE(sel.at(2).data.toInt(), 9);
}

void DaItemListEditWidgetTest::testUpDownEmitsOrderChanged()
{
    DAItemListEditWidget w;
    w.setReorderEnabled(true);
    w.setSelectedItems(makeEntries({ "a", "b", "c" }));
    QListWidget* list = listOf(w);
    list->setCurrentRow(1);

    QSignalSpy orderSpy(&w, &DAItemListEditWidget::orderChanged);
    QSignalSpy selectionSpy(&w, &DAItemListEditWidget::selectedItemsChanged);
    w.findChild< QPushButton* >(QStringLiteral("upButton"))->click();

    QCOMPARE(textsOf(w), (QStringList { "b", "a", "c" }));
    QCOMPARE(selectionSpy.count(), 1);
    QCOMPARE(orderSpy.count(), 1);

    QVector< DAItemEntry > args = qvariant_cast< QVector< DAItemEntry > >(orderSpy.at(0).at(0));
    QStringList argTexts;
    for (const DAItemEntry& e : args)
        argTexts << e.text;
    QCOMPARE(argTexts, textsOf(w));
}

void DaItemListEditWidgetTest::testFilterHidesRowsKeepsSelection()
{
    DAItemListEditWidget w;
    w.setSelectedItems(makeEntries({ "a", "b", "c" }));
    w.setFilterVisible(true);
    showWidget(w);

    QLineEdit* filter = filterOf(w);
    QVERIFY(filter);
    QSignalSpy orderSpy(&w, &DAItemListEditWidget::orderChanged);
    QSignalSpy selectionSpy(&w, &DAItemListEditWidget::selectedItemsChanged);
    filter->setText(QStringLiteral("a"));
    QTest::qWait(10);

    QListWidget* list = listOf(w);
    QCOMPARE(list->isRowHidden(0), false);
    QCOMPARE(list->isRowHidden(1), true);
    QCOMPARE(list->isRowHidden(2), true);
    QCOMPARE(w.selectedItems().size(), 3);
    QCOMPARE(orderSpy.count(), 0);
    QCOMPARE(selectionSpy.count(), 0);
}

void DaItemListEditWidgetTest::testFilterMatchesLikeCheckList()
{
    DAItemListEditWidget w;
    w.setSelectedItems(makeEntries({ QStringLiteral("Alpha"), QStringLiteral("Beta") }));
    w.setFilterVisible(true);
    QLineEdit* filter = filterOf(w);
    QListWidget* list   = listOf(w);

    filter->setText(QStringLiteral("alpha"));
    QCOMPARE(list->isRowHidden(0), false);
    QCOMPARE(list->isRowHidden(1), true);

    filter->setText(QStringLiteral("  alp  "));
    QCOMPARE(list->isRowHidden(0), false);

    filter->setText(QStringLiteral("zz"));
    QCOMPARE(list->isRowHidden(0), true);
    QCOMPARE(list->isRowHidden(1), true);
    QVERIFY(!emptyOf(w)->isHidden());
}

void DaItemListEditWidgetTest::testEmptyStateVisibility()
{
    DAItemListEditWidget w;
    QLabel* empty       = emptyOf(w);
    QListWidget* list   = listOf(w);
    QVERIFY(empty);
    QVERIFY(list);
    QVERIFY(!empty->isHidden());  // 0 条已选
    QVERIFY(list->isHidden());

    w.setSelectedItems(makeEntries({ "a", "b" }));
    QVERIFY(empty->isHidden());
    QVERIFY(!list->isHidden());

    w.setFilterVisible(true);
    filterOf(w)->setText(QStringLiteral("zz"));
    QVERIFY(!empty->isHidden());
    QVERIFY(list->isHidden());
    QCOMPARE(w.selectedItems().size(), 2);

    filterOf(w)->setText(QString());
    QVERIFY(empty->isHidden());
    QVERIFY(!list->isHidden());
}

void DaItemListEditWidgetTest::testConfirmClearArming()
{
    DAItemListEditWidget w;
    w.setSelectedItems(makeEntries({ "a", "b" }));
    w.setConfirmClearEnabled(true);
    w.setConfirmClearInterval(100);
    w.setConfirmClearText(QStringLiteral("Armed"));
    auto* button = w.findChild< QPushButton* >(QStringLiteral("removeAllButton"));
    QVERIFY(button);
    const QString baseText = button->text();
    QSignalSpy selectionSpy(&w, &DAItemListEditWidget::selectedItemsChanged);

    button->click();  // 第一次：只武装
    QCOMPARE(selectionSpy.count(), 0);
    QCOMPARE(button->text(), QStringLiteral("Armed"));
    QCOMPARE(w.selectedItems().size(), 2);

    button->click();  // 第二次：真清空
    QCOMPARE(selectionSpy.count(), 1);
    QVERIFY(w.selectedItems().isEmpty());
    QCOMPARE(button->text(), baseText);

    // 超时自动还原
    DAItemListEditWidget w2;
    w2.setSelectedItems(makeEntries({ "a" }));
    w2.setConfirmClearEnabled(true);
    w2.setConfirmClearInterval(100);
    w2.setConfirmClearText(QStringLiteral("Armed"));
    auto* button2 = w2.findChild< QPushButton* >(QStringLiteral("removeAllButton"));
    const QString base2 = button2->text();
    QSignalSpy spy2(&w2, &DAItemListEditWidget::selectedItemsChanged);
    button2->click();
    QCOMPARE(button2->text(), QStringLiteral("Armed"));
    QTest::qWait(250);
    QCOMPARE(button2->text(), base2);
    QCOMPARE(spy2.count(), 0);

    // 程序化替换已选集合也要复位文案
    DAItemListEditWidget w3;
    w3.setSelectedItems(makeEntries({ "a" }));
    w3.setConfirmClearEnabled(true);
    w3.setConfirmClearText(QStringLiteral("Armed"));
    auto* button3 = w3.findChild< QPushButton* >(QStringLiteral("removeAllButton"));
    const QString base3 = button3->text();
    button3->click();
    QCOMPARE(button3->text(), QStringLiteral("Armed"));
    w3.setSelectedItems(makeEntries({ "x" }));
    QCOMPARE(button3->text(), base3);
}

QTEST_MAIN(DaItemListEditWidgetTest)
#include "tst_daitemlisteditwidget.moc"
