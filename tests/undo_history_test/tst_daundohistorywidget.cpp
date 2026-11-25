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
    void testTimestampsOffByDefault();
    void testTimestampRecordedOnPush();
    void testTimestampSurvivesUndoRedo();
    void testBranchTruncationDropsTimestamp();
    void testEmptyCommandFilterHidesRow();
    void testFilterRefreshesOnNewCommand();
    void testNoGroupIsSafe();
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
    group.setActiveStack(&stack);
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

void DaUndoHistoryWidgetTest::testTimestampsOffByDefault()
{
    DAUndoHistoryWidget w;
    QUndoGroup group;
    QUndoStack stack;
    group.addStack(&stack);
    group.setActiveStack(&stack);
    w.setUndoGroup(&group);
    int value = 0;
    stack.push(new AddCommand(&value, 1));

    QVERIFY(!w.timestampsVisible());
    QVERIFY(!w.iconsVisible());
    QVERIFY(!w.isEmptyCommandFilterEnabled());
    // 记录不受显示开关影响
    QVERIFY(w.commandTimestamp(0).isValid());
}

void DaUndoHistoryWidgetTest::testTimestampRecordedOnPush()
{
    DAUndoHistoryWidget w;
    QUndoGroup group;
    QUndoStack stack;
    group.addStack(&stack);
    group.setActiveStack(&stack);
    w.setUndoGroup(&group);
    int value = 0;
    stack.push(new AddCommand(&value, 1));
    stack.push(new AddCommand(&value, 2));

    const QDateTime t0 = w.commandTimestamp(0);
    const QDateTime t1 = w.commandTimestamp(1);
    QVERIFY(t0.isValid());
    QVERIFY(t1.isValid());
    QVERIFY(t1 >= t0);
    QVERIFY(!w.commandTimestamp(2).isValid());
}

void DaUndoHistoryWidgetTest::testTimestampSurvivesUndoRedo()
{
    DAUndoHistoryWidget w;
    QUndoGroup group;
    QUndoStack stack;
    group.addStack(&stack);
    group.setActiveStack(&stack);
    w.setUndoGroup(&group);
    int value = 0;
    stack.push(new AddCommand(&value, 1));
    stack.push(new AddCommand(&value, 2));
    const QDateTime t1 = w.commandTimestamp(1);

    stack.undo();
    stack.redo();
    QCOMPARE(w.commandTimestamp(1), t1);
}

void DaUndoHistoryWidgetTest::testBranchTruncationDropsTimestamp()
{
    DAUndoHistoryWidget w;
    QUndoGroup group;
    QUndoStack stack;
    group.addStack(&stack);
    group.setActiveStack(&stack);
    w.setUndoGroup(&group);
    int value = 0;
    stack.push(new AddCommand(&value, 1));
    stack.push(new AddCommand(&value, 2));
    stack.undo();
    stack.push(new AddCommand(&value, 3));
    QCOMPARE(stack.count(), 2);
    QVERIFY(w.commandTimestamp(1).isValid());
    QVERIFY(w.commandTimestamp(1) >= w.commandTimestamp(0));
}

void DaUndoHistoryWidgetTest::testEmptyCommandFilterHidesRow()
{
    DAUndoHistoryWidget w;
    QUndoGroup group;
    QUndoStack stack;
    group.addStack(&stack);
    group.setActiveStack(&stack);
    w.setUndoGroup(&group);
    int value = 0;
    auto* cmd = new AddCommand(&value, 1);
    cmd->setText(QString());
    stack.push(cmd);

    QUndoView* view = w.findChild< QUndoView* >(QStringLiteral("undoHistoryView"));
    QVERIFY(view);
    QAbstractItemModel* model = view->model();
    QVERIFY(model);
    QCOMPARE(model->rowCount(), 2);

    w.setEmptyCommandFilterEnabled(true);
    QVERIFY(view->isRowHidden(1));
    w.setEmptyCommandFilterEnabled(false);
    QVERIFY(!view->isRowHidden(1));
}

void DaUndoHistoryWidgetTest::testFilterRefreshesOnNewCommand()
{
    DAUndoHistoryWidget w;
    QUndoGroup group;
    QUndoStack stack;
    group.addStack(&stack);
    group.setActiveStack(&stack);
    w.setUndoGroup(&group);
    w.setEmptyCommandFilterEnabled(true);

    QUndoView* view = w.findChild< QUndoView* >(QStringLiteral("undoHistoryView"));
    QVERIFY(view);
    QAbstractItemModel* model = view->model();
    QVERIFY(model);

    int value = 0;
    stack.push(new AddCommand(&value, 1));  // 有文本
    QCOMPARE(model->rowCount(), 2);
    QVERIFY(!view->isRowHidden(1));

    auto* empty = new AddCommand(&value, 1);
    empty->setText(QString());
    stack.push(empty);
    QCOMPARE(model->rowCount(), 3);
    QVERIFY(view->isRowHidden(2));
}

void DaUndoHistoryWidgetTest::testNoGroupIsSafe()
{
    DAUndoHistoryWidget w;
    QVERIFY(!w.commandTimestamp(0).isValid());
    w.setTimestampsVisible(true);
    w.setIconsVisible(true);
    w.setEmptyCommandFilterEnabled(true);
    QVERIFY(!w.grab().isNull());
    w.setUndoGroup(nullptr);
    QVERIFY(!w.commandTimestamp(0).isValid());
}

QTEST_MAIN(DaUndoHistoryWidgetTest)
#include "tst_daundohistorywidget.moc"

