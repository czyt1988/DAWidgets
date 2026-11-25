#include <QAction>
#include <QFontDatabase>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QToolButton>
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
    void testLineNumbersOffByDefault();
    void testLineNumbersGrowWidth();
    void testLineNumbersToggleRestoresMargin();
    void testFindShortcutOpensBar();
    void testFindCountAndWrap();
    void testFindHighlightsAllMatches();
    void testFindCaseSensitive();
    void testSetTextClearsFind();
    void testReadOnlyFindStillWorks();
    void testFindBarHiddenByDefault();

private:
    static QPlainTextEdit* editOf(DATextEditWidget& w);
    static QLabel* findCountOf(DATextEditWidget& w);
    static void showWidget(DATextEditWidget& w);
};

void DaTextEditWidgetTest::showWidget(DATextEditWidget& w)
{
    w.resize(480, 320);
    w.show();
    QTest::qWaitForWindowExposed(&w);
    w.activateWindow();
    QTest::qWaitForWindowActive(&w);
    QTest::qWait(20);
}

QLabel* DaTextEditWidgetTest::findCountOf(DATextEditWidget& w)
{
    return w.findChild< QLabel* >(QStringLiteral("findCount"));
}

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

void DaTextEditWidgetTest::testLineNumbersOffByDefault()
{
    DATextEditWidget w;
    QVERIFY(!w.lineNumbersVisible());
    QWidget* area = w.findChild< QWidget* >(QStringLiteral("lineNumberArea"));
    QVERIFY(area);
    QVERIFY(area->isHidden());
}

void DaTextEditWidgetTest::testLineNumbersGrowWidth()
{
    DATextEditWidget w;
    showWidget(w);
    w.setLineNumbersVisible(true);
    QWidget* area = w.findChild< QWidget* >(QStringLiteral("lineNumberArea"));
    QVERIFY(area);
    QStringList lines;
    for (int i = 0; i < 100; ++i)
        lines << QStringLiteral("line %1").arg(i);
    w.setText(lines.join(QLatin1Char('\n')));
    QTest::qWait(20);
    const int width100 = area->width();
    QVERIFY(width100 > 0);

    QStringList many;
    for (int i = 0; i < 10000; ++i)
        many << QStringLiteral("line %1").arg(i);
    w.setText(many.join(QLatin1Char('\n')));
    QTest::qWait(20);
    QVERIFY(area->width() > width100);
}

void DaTextEditWidgetTest::testLineNumbersToggleRestoresMargin()
{
    DATextEditWidget w;
    showWidget(w);
    w.setText(QStringLiteral("a\nb\nc"));
    QWidget* area = w.findChild< QWidget* >(QStringLiteral("lineNumberArea"));
    QVERIFY(area);
    w.setLineNumbersVisible(true);
    QTest::qWait(20);
    QVERIFY(!area->isHidden());
    w.setLineNumbersVisible(false);
    QTest::qWait(20);
    QVERIFY(area->isHidden());
    QVERIFY(!w.lineNumbersVisible());
}

void DaTextEditWidgetTest::testFindShortcutOpensBar()
{
    DATextEditWidget w;
    showWidget(w);
    QPlainTextEdit* edit = editOf(w);
    QVERIFY(edit);
    edit->setFocus();
    QTest::keyClick(edit, Qt::Key_F, Qt::ControlModifier);
    QVERIFY(!w.findBarVisible());

    w.setFindEnabled(true);
    edit->setFocus();
    QTest::keyClick(edit, Qt::Key_F, Qt::ControlModifier);
    QVERIFY(w.findBarVisible());
}

void DaTextEditWidgetTest::testFindCountAndWrap()
{
    DATextEditWidget w;
    showWidget(w);
    w.setText(QStringLiteral("a\nb\na"));
    w.showFindBar(QStringLiteral("a"));
    QLabel* count = findCountOf(w);
    QVERIFY(count);
    QCOMPARE(count->text(), QStringLiteral("1 / 2"));

    QToolButton* next = w.findChild< QToolButton* >(QStringLiteral("findNext"));
    QVERIFY(next);
    next->click();
    QCOMPARE(count->text(), QStringLiteral("2 / 2"));
    next->click();
    QCOMPARE(count->text(), QStringLiteral("1 / 2"));
}

void DaTextEditWidgetTest::testFindHighlightsAllMatches()
{
    DATextEditWidget w;
    showWidget(w);
    w.setText(QStringLiteral("a b a c a"));
    w.showFindBar(QStringLiteral("a"));
    QCOMPARE(editOf(w)->extraSelections().size(), 3);
}

void DaTextEditWidgetTest::testFindCaseSensitive()
{
    DATextEditWidget w;
    showWidget(w);
    w.setText(QStringLiteral("ERROR error"));
    w.showFindBar(QStringLiteral("error"));
    QLabel* count = findCountOf(w);
    QVERIFY(count);
    QCOMPARE(count->text(), QStringLiteral("1 / 2"));

    w.setFindCaseSensitive(true);
    QCOMPARE(count->text(), QStringLiteral("1 / 1"));
    QVERIFY(w.findCaseSensitive());
    w.setFindCaseSensitive(false);
    QCOMPARE(count->text(), QStringLiteral("1 / 2"));
}

void DaTextEditWidgetTest::testSetTextClearsFind()
{
    DATextEditWidget w;
    showWidget(w);
    w.setText(QStringLiteral("aaa"));
    w.showFindBar(QStringLiteral("a"));
    QVERIFY(!editOf(w)->extraSelections().isEmpty());

    w.setText(QStringLiteral("other"));
    QVERIFY(editOf(w)->extraSelections().isEmpty());
    QCOMPARE(findCountOf(w)->text(), QStringLiteral("0 / 0"));
}

void DaTextEditWidgetTest::testReadOnlyFindStillWorks()
{
    DATextEditWidget w;
    w.setReadOnly(true);
    showWidget(w);
    w.setText(QStringLiteral("a\nb\na"));
    w.setFindEnabled(true);
    QPlainTextEdit* edit = editOf(w);
    edit->setFocus();
    QTest::keyClick(edit, Qt::Key_F, Qt::ControlModifier);
    QVERIFY(w.findBarVisible());

    QLineEdit* findEdit = w.findChild< QLineEdit* >(QStringLiteral("findEdit"));
    QVERIFY(findEdit);
    findEdit->setText(QStringLiteral("a"));
    QCOMPARE(findCountOf(w)->text(), QStringLiteral("1 / 2"));
    QToolButton* next = w.findChild< QToolButton* >(QStringLiteral("findNext"));
    QVERIFY(next);
    next->click();
    QCOMPARE(findCountOf(w)->text(), QStringLiteral("2 / 2"));
}

void DaTextEditWidgetTest::testFindBarHiddenByDefault()
{
    DATextEditWidget w;
    QVERIFY(!w.findBarVisible());
    QWidget* bar = w.findChild< QWidget* >(QStringLiteral("findBar"));
    QVERIFY(bar);
    QVERIFY(bar->isHidden());
}

QTEST_MAIN(DaTextEditWidgetTest)
#include "tst_datexteditwidget.moc"
