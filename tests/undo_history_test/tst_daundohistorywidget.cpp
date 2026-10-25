#include <QAbstractItemModel>
#include <QSpinBox>
#include <QUndoCommand>
#include <QUndoGroup>
#include <QUndoStack>
#include <QUndoView>
#include <QtTest/QtTest>

#include <DAUndoHistoryWidget.h>

using namespace DA;

namespace
{
class AddCommand : public QUndoCommand
{
public:
    AddCommand(int* value, int delta) : m_value(value), m_delta(delta)
    {
        setText(QStringLiteral("Add"));
    }
    void undo() override
    {
        *m_value -= m_delta;
    }
    void redo() override
    {
        *m_value += m_delta;
    }

private:
    int* m_value { nullptr };
    int m_delta { 0 };
};
}  // namespace

class DaUndoHistoryWidgetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testConstruction();
    void testSetUndoLimit();
    void testUserLimitEmits();
    void testUndoGroupFollowsStack();
};

void DaUndoHistoryWidgetTest::testConstruction()
{
    DAUndoHistoryWidget w;
    QCOMPARE(w.undoLimit(), 20);
    QVERIFY(w.findChild< QSpinBox* >(QStringLiteral("undoLimitSpin")));
    QVERIFY(w.findChild< QUndoView* >(QStringLiteral("undoHistoryView")));
    QVERIFY(w.undoGroup() == nullptr);
}

void DaUndoHistoryWidgetTest::testSetUndoLimit()
{
    DAUndoHistoryWidget w;
    QSignalSpy spy(&w, &DAUndoHistoryWidget::undoLimitChangeRequested);
    w.setUndoLimit(35);
    QCOMPARE(w.undoLimit(), 35);
    // 程序化设置不应回环发信号
    QCOMPARE(spy.count(), 0);
}

void DaUndoHistoryWidgetTest::testUserLimitEmits()
{
    DAUndoHistoryWidget w;
    QSignalSpy spy(&w, &DAUndoHistoryWidget::undoLimitChangeRequested);
    QSpinBox* spin = w.findChild< QSpinBox* >(QStringLiteral("undoLimitSpin"));
    QVERIFY(spin);
    spin->setValue(42);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 42);
}

void DaUndoHistoryWidgetTest::testUndoGroupFollowsStack()
{
    DAUndoHistoryWidget w;
    QUndoGroup group;
    QUndoStack stack;
    group.addStack(&stack);
    w.setUndoGroup(&group);
    QCOMPARE(w.undoGroup(), &group);

    int value = 0;
    stack.push(new AddCommand(&value, 1));
    stack.push(new AddCommand(&value, 2));
    QCOMPARE(value, 3);
    QCOMPARE(stack.count(), 2);

    QUndoView* view = w.findChild< QUndoView* >(QStringLiteral("undoHistoryView"));
    QVERIFY(view);
    QCOMPARE(view->group(), &group);

    stack.undo();
    QCOMPARE(value, 1);  // 撤销最后一条 +2
    QCOMPARE(stack.index(), 1);
    stack.redo();
    QCOMPARE(value, 3);
    QCOMPARE(stack.index(), 2);
}

QTEST_MAIN(DaUndoHistoryWidgetTest)
#include "tst_daundohistorywidget.moc"
