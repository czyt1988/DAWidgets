#include <QtTest/QtTest>

#include <DAFilePathEditWidget.h>

using namespace DA;

class DaFilePathEditWidgetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDefaultMode();
    void testModeRoundTrip();
    void testStartDirectory();
    void testDefaultSuffix();
    void testFilePathRoundTrip();
    void testNameFilters();
    void testPathChanged();
};

void DaFilePathEditWidgetTest::testDefaultMode()
{
    DAFilePathEditWidget w;
    QCOMPARE(w.mode(), DAFilePathEditWidget::Mode::OpenFile);
    QVERIFY(w.getFilePath().isEmpty());
}

void DaFilePathEditWidgetTest::testModeRoundTrip()
{
    DAFilePathEditWidget w;
    w.setMode(DAFilePathEditWidget::Mode::SaveFile);
    QCOMPARE(w.mode(), DAFilePathEditWidget::Mode::SaveFile);
    w.setMode(DAFilePathEditWidget::Mode::ExistingDirectory);
    QCOMPARE(w.mode(), DAFilePathEditWidget::Mode::ExistingDirectory);
    w.setMode(DAFilePathEditWidget::Mode::OpenFile);
    QCOMPARE(w.mode(), DAFilePathEditWidget::Mode::OpenFile);
}

void DaFilePathEditWidgetTest::testStartDirectory()
{
    DAFilePathEditWidget w;
    QVERIFY(w.startDirectory().isEmpty());
    w.setStartDirectory(QStringLiteral("C:/tmp"));
    QCOMPARE(w.startDirectory(), QStringLiteral("C:/tmp"));
    w.setStartDirectory(QString());
    QVERIFY(w.startDirectory().isEmpty());
}

void DaFilePathEditWidgetTest::testDefaultSuffix()
{
    DAFilePathEditWidget w;
    QVERIFY(w.defaultSuffix().isEmpty());
    w.setDefaultSuffix(QStringLiteral("csv"));
    QCOMPARE(w.defaultSuffix(), QStringLiteral("csv"));
}

void DaFilePathEditWidgetTest::testFilePathRoundTrip()
{
    DAFilePathEditWidget w;
    w.setFilePath(QStringLiteral("C:/tmp/data.csv"));
    QCOMPARE(w.getFilePath(), QStringLiteral("C:/tmp/data.csv"));
    w.setFilePath(QString());
    QVERIFY(w.getFilePath().isEmpty());
}

void DaFilePathEditWidgetTest::testNameFilters()
{
    DAFilePathEditWidget w;
    w.setNameFilter(QStringLiteral("CSV (*.csv)"));
    QCOMPARE(w.nameFilters(), QStringList { QStringLiteral("CSV (*.csv)") });
    w.setNameFilters({ QStringLiteral("CSV (*.csv)"), QStringLiteral("All (*.*)") });
    QCOMPARE(w.nameFilters().size(), 2);
    QCOMPARE(w.nameFilters().at(1), QStringLiteral("All (*.*)"));
}

void DaFilePathEditWidgetTest::testPathChanged()
{
    DAFilePathEditWidget w;
    QSignalSpy spy(&w, &DAFilePathEditWidget::pathChanged);
    w.setFilePath(QStringLiteral("C:/tmp/a.txt"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toString(), QStringLiteral("C:/tmp/a.txt"));
}

QTEST_MAIN(DaFilePathEditWidgetTest)
#include "tst_dafilepatheditwidget.moc"
