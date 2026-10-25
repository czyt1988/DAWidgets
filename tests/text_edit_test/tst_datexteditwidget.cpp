#include <QAction>
#include <QFontDatabase>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QtTest/QtTest>

#include <DATextEditWidget.h>

using namespace DA;

class DaTextEditWidgetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDefaults();
    void testTextRoundTrip();
    void testReadOnly();
    void testMonospace();
    void testLineWrap();
    void testToolbarAndAction();
    void testPlaceholder();

private:
    static QPlainTextEdit* editOf(DATextEditWidget& w);
};

QPlainTextEdit* DaTextEditWidgetTest::editOf(DATextEditWidget& w)
{
    return w.findChild< QPlainTextEdit* >(QStringLiteral("textEdit"));
}

void DaTextEditWidgetTest::testDefaults()
{
    DATextEditWidget w;
    QVERIFY(!w.isReadOnly());
    QVERIFY(!w.isMonospace());
    QVERIFY(w.isLineWrapEnabled());
    QVERIFY(!w.isToolbarVisible());
    QVERIFY(w.text().isEmpty());
}

void DaTextEditWidgetTest::testTextRoundTrip()
{
    DATextEditWidget w;
    QSignalSpy spy(&w, &DATextEditWidget::textChanged);
    w.setText(QStringLiteral("line1\nline2"));
    QCOMPARE(w.text(), QStringLiteral("line1\nline2"));
    // setText 是程序化写入，不发 textChanged
    QCOMPARE(spy.count(), 0);
}

void DaTextEditWidgetTest::testReadOnly()
{
    DATextEditWidget w;
    w.setReadOnly(true);
    QVERIFY(w.isReadOnly());
    QVERIFY(editOf(w)->isReadOnly());
    w.setReadOnly(false);
    QVERIFY(!w.isReadOnly());
}

void DaTextEditWidgetTest::testMonospace()
{
    DATextEditWidget w;
    w.setMonospace(true);
    QVERIFY(w.isMonospace());
    const QFont fixed = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    QCOMPARE(editOf(w)->font().family(), fixed.family());
    w.setMonospace(false);
    QVERIFY(!w.isMonospace());
}

void DaTextEditWidgetTest::testLineWrap()
{
    DATextEditWidget w;
    w.setLineWrapEnabled(false);
    QVERIFY(!w.isLineWrapEnabled());
    QCOMPARE(editOf(w)->lineWrapMode(), QPlainTextEdit::NoWrap);
    w.setLineWrapEnabled(true);
    QVERIFY(w.isLineWrapEnabled());
}

void DaTextEditWidgetTest::testToolbarAndAction()
{
    DATextEditWidget w;
    QAction act(QStringLiteral("Save"));
    w.addToolAction(&act);
    w.setToolbarVisible(true);
    QVERIFY(w.isToolbarVisible());
    QToolBar* bar = w.findChild< QToolBar* >(QStringLiteral("textToolbar"));
    QVERIFY(bar);
    QCOMPARE(bar->actions().size(), 1);
    QSignalSpy spy(&act, &QAction::triggered);
    act.trigger();
    QCOMPARE(spy.count(), 1);
}

void DaTextEditWidgetTest::testPlaceholder()
{
    DATextEditWidget w;
    w.setPlaceholderText(QStringLiteral("Type here"));
    QCOMPARE(editOf(w)->placeholderText(), QStringLiteral("Type here"));
}

QTEST_MAIN(DaTextEditWidgetTest)
#include "tst_datexteditwidget.moc"
