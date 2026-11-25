#include <QtTest/QtTest>

#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMimeData>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QToolButton>
#include <QUrl>

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
    void testDefaultLayoutUnchanged();
    void testValidationDisabledByDefault();
    void testValidationInvalidPathEmitsOnce();
    void testValidationHiddenOnEmptyThenReemit();
    void testValidationOpenFileAndDirectory();
    void testValidationSaveFileAllowsMissingPath();
    void testValidateNowReturnsBool();
    void testValidationStatusRowTextAndProperty();
    void testInvalidBorderPixels();
    void testDropDisabledByDefault();
    void testDropFillsPathAndValidates();
    void testDropIgnoresRemoteUrl();
    void testRecentPathsDedupeAndTruncate();
    void testRecentPathActivated();
    void testRecentButtonHiddenWithoutPaths();

private:
    static QLineEdit* lineEditOf(DAFilePathEditWidget& w);
    static QWidget* statusOf(DAFilePathEditWidget& w);
    static QLabel* dropHintOf(DAFilePathEditWidget& w);
    static QToolButton* recentButtonOf(DAFilePathEditWidget& w);
    static QString makeFile(const QTemporaryDir& dir, const QString& name);
};

QLineEdit* DaFilePathEditWidgetTest::lineEditOf(DAFilePathEditWidget& w)
{
    return w.findChild< QLineEdit* >(QStringLiteral("lineEdit"));
}

QWidget* DaFilePathEditWidgetTest::statusOf(DAFilePathEditWidget& w)
{
    return w.findChild< QWidget* >(QStringLiteral("widgetStatus"));
}

QLabel* DaFilePathEditWidgetTest::dropHintOf(DAFilePathEditWidget& w)
{
    return w.findChild< QLabel* >(QStringLiteral("labelDropHint"));
}

QToolButton* DaFilePathEditWidgetTest::recentButtonOf(DAFilePathEditWidget& w)
{
    return w.findChild< QToolButton* >(QStringLiteral("toolButtonRecent"));
}

QString DaFilePathEditWidgetTest::makeFile(const QTemporaryDir& dir, const QString& name)
{
    const QString path = dir.filePath(name);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return QString();
    f.write("x");
    f.close();
    return path;
}

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

void DaFilePathEditWidgetTest::testDefaultLayoutUnchanged()
{
    DAFilePathEditWidget w;
    QVERIFY(!w.isDropEnabled());
    QVERIFY(!w.isValidationEnabled());
    QVERIFY(w.recentPaths().isEmpty());
    QVERIFY(statusOf(w)->isHidden());
    QVERIFY(dropHintOf(w)->isHidden());
    QVERIFY(recentButtonOf(w)->isHidden());
    QVERIFY(w.sizeHint().height() < 40);
}

void DaFilePathEditWidgetTest::testValidationDisabledByDefault()
{
    DAFilePathEditWidget w;
    QLineEdit* le = lineEditOf(w);
    QVERIFY(le);
    QSignalSpy spy(&w, &DAFilePathEditWidget::validationChanged);
    w.setFilePath(QStringLiteral("Z:/missing/file.csv"));
    QTest::keyClick(le, Qt::Key_Return);
    QCOMPARE(spy.count(), 0);
    QVERIFY(statusOf(w)->isHidden());
}

void DaFilePathEditWidgetTest::testValidationInvalidPathEmitsOnce()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    DAFilePathEditWidget w;
    w.setValidationEnabled(true);
    QSignalSpy spy(&w, &DAFilePathEditWidget::validationChanged);
    w.setFilePath(dir.filePath(QStringLiteral("missing.csv")));
    QVERIFY(!w.validateNow());
    QVERIFY(!w.validateNow());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
}

void DaFilePathEditWidgetTest::testValidationHiddenOnEmptyThenReemit()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    DAFilePathEditWidget w;
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    w.setValidationEnabled(true);
    QSignalSpy spy(&w, &DAFilePathEditWidget::validationChanged);
    w.setFilePath(dir.filePath(QStringLiteral("missing.csv")));
    w.validateNow();
    QCOMPARE(spy.count(), 1);
    QVERIFY(statusOf(w)->isVisible());

    w.setFilePath(QString());
    w.validateNow();
    QVERIFY(statusOf(w)->isHidden());
    QCOMPARE(spy.count(), 1);

    w.setFilePath(dir.filePath(QStringLiteral("missing.csv")));
    w.validateNow();
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void DaFilePathEditWidgetTest::testValidationOpenFileAndDirectory()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = makeFile(dir, QStringLiteral("a.txt"));
    QVERIFY(!file.isEmpty());

    DAFilePathEditWidget w;
    w.setValidationEnabled(true);
    w.setMode(DAFilePathEditWidget::Mode::OpenFile);
    w.setFilePath(file);
    QVERIFY(w.validateNow());
    w.setFilePath(dir.filePath(QStringLiteral("missing.txt")));
    QVERIFY(!w.validateNow());

    w.setMode(DAFilePathEditWidget::Mode::ExistingDirectory);
    w.setFilePath(dir.path());
    QVERIFY(w.validateNow());
    w.setFilePath(file);
    QVERIFY(!w.validateNow());
}

void DaFilePathEditWidgetTest::testValidationSaveFileAllowsMissingPath()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    DAFilePathEditWidget w;
    w.setValidationEnabled(true);
    w.setMode(DAFilePathEditWidget::Mode::SaveFile);
    w.setFilePath(dir.filePath(QStringLiteral("new.csv")));
    QVERIFY(w.validateNow());
    w.setFilePath(dir.filePath(QStringLiteral("no_such_dir/new.csv")));
    QVERIFY(!w.validateNow());
}

void DaFilePathEditWidgetTest::testValidateNowReturnsBool()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = makeFile(dir, QStringLiteral("a.txt"));
    QVERIFY(!file.isEmpty());
    DAFilePathEditWidget w;
    w.setValidationEnabled(true);
    QSignalSpy spy(&w, &DAFilePathEditWidget::validationChanged);

    w.setFilePath(file);
    QVERIFY(w.validateNow());
    QCOMPARE(spy.count(), 1);
    QVERIFY(w.validateNow());
    QCOMPARE(spy.count(), 1);

    w.setFilePath(QString());
    QVERIFY(!w.validateNow());
    QCOMPARE(spy.count(), 1);
}

void DaFilePathEditWidgetTest::testValidationStatusRowTextAndProperty()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = makeFile(dir, QStringLiteral("a.txt"));
    QVERIFY(!file.isEmpty());
    DAFilePathEditWidget w;
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    w.setValidationEnabled(true);
    QLineEdit* le = lineEditOf(w);
    auto* text = w.findChild< QLabel* >(QStringLiteral("labelStatusText"));
    QVERIFY(le && text);

    w.setFilePath(file);
    w.validateNow();
    QVERIFY(!text->text().isEmpty());
    QCOMPARE(le->property("invalid").toBool(), false);
    QVERIFY(statusOf(w)->isVisible());

    w.setFilePath(dir.filePath(QStringLiteral("missing.csv")));
    w.validateNow();
    QVERIFY(!text->text().isEmpty());
    QCOMPARE(le->property("invalid").toBool(), true);
    QVERIFY(statusOf(w)->isVisible());
}

void DaFilePathEditWidgetTest::testInvalidBorderPixels()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString missing = dir.filePath(QStringLiteral("missing.csv"));
    DAFilePathEditWidget w;
    w.setStyleSheet(QStringLiteral("QLineEdit[invalid=\"true\"]{border:1px solid #F56C6C;}"));
    w.setValidationEnabled(true);
    w.resize(360, 120);
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    QTest::qWait(30);
    QLineEdit* le = lineEditOf(w);
    QVERIFY(le);

    w.setFilePath(missing);
    w.validateNow();
    QTest::qWait(30);
    QImage img = le->grab().toImage();
    QVERIFY(img.rect().contains(QPoint(0, img.height() / 2)));
    const QColor invalidPixel = img.pixelColor(0, img.height() / 2);
    QVERIFY2(invalidPixel.red() > 200 && invalidPixel.green() < 140,
             qPrintable(QStringLiteral("invalid border pixel=%1").arg(invalidPixel.name())));

    const QString file = makeFile(dir, QStringLiteral("a.txt"));
    QVERIFY(!file.isEmpty());
    w.setFilePath(file);
    w.validateNow();
    QTest::qWait(30);
    img = le->grab().toImage();
    const QColor validPixel = img.pixelColor(0, img.height() / 2);
    QVERIFY2(!(validPixel.red() > 200 && validPixel.green() < 140),
             qPrintable(QStringLiteral("valid border pixel=%1").arg(validPixel.name())));
}

void DaFilePathEditWidgetTest::testDropDisabledByDefault()
{
    DAFilePathEditWidget w;
    QVERIFY(!w.acceptDrops());
    QMimeData mime;
    mime.setUrls({ QUrl::fromLocalFile(QDir::tempPath()) });
    QDragEnterEvent enter(QPoint(5, 5), Qt::CopyAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &enter);
    QVERIFY(!enter.isAccepted());
}

void DaFilePathEditWidgetTest::testDropFillsPathAndValidates()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = makeFile(dir, QStringLiteral("drop.txt"));
    QVERIFY(!file.isEmpty());

    DAFilePathEditWidget w;
    w.setValidationEnabled(true);
    w.setDropEnabled(true);
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    QLabel* hint = dropHintOf(w);
    QVERIFY(hint);
    QVERIFY(hint->isVisible());

    QMimeData mime;
    mime.setUrls({ QUrl::fromLocalFile(file) });
    QDragEnterEvent enter(QPoint(10, 10), Qt::CopyAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &enter);
    QVERIFY(enter.isAccepted());
    QCOMPARE(hint->property("dragActive").toBool(), true);

    QSignalSpy valSpy(&w, &DAFilePathEditWidget::validationChanged);
    QSignalSpy selSpy(&w, &DAFilePathEditWidget::selectedPath);
    QDropEvent drop(QPointF(10, 10), Qt::CopyAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &drop);
    QCOMPARE(w.getFilePath(), file);
    QCOMPARE(valSpy.count(), 1);
    QCOMPARE(valSpy.at(0).at(0).toBool(), true);
    QCOMPARE(selSpy.count(), 1);
    QCOMPARE(selSpy.at(0).at(0).toString(), file);
    QCOMPARE(hint->property("dragActive").toBool(), false);
}

void DaFilePathEditWidgetTest::testDropIgnoresRemoteUrl()
{
    DAFilePathEditWidget w;
    w.setDropEnabled(true);
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    QMimeData mime;
    mime.setUrls({ QUrl(QStringLiteral("https://example.com/a.csv")) });
    QSignalSpy selSpy(&w, &DAFilePathEditWidget::selectedPath);
    QDropEvent drop(QPointF(10, 10), Qt::CopyAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &drop);
    QVERIFY(w.getFilePath().isEmpty());
    QCOMPARE(selSpy.count(), 0);
}

void DaFilePathEditWidgetTest::testRecentPathsDedupeAndTruncate()
{
    DAFilePathEditWidget w;
    w.setRecentPaths({ QStringLiteral("C:/a"), QString(), QStringLiteral("C:/b"),
                       QStringLiteral("C:/a"), QStringLiteral("C:/c"), QStringLiteral("C:/d"),
                       QStringLiteral("C:/e"), QStringLiteral("C:/f") });
    const QStringList recent = w.recentPaths();
    QCOMPARE(recent.size(), 5);
    QCOMPARE(recent.at(0), QStringLiteral("C:/a"));
    for (int i = 1; i < recent.size(); ++i) {
        for (int j = 0; j < i; ++j)
            QVERIFY(recent.at(i) != recent.at(j));
    }
}

void DaFilePathEditWidgetTest::testRecentPathActivated()
{
    DAFilePathEditWidget w;
    w.setRecentPaths({ QStringLiteral("C:/a"), QStringLiteral("C:/b") });
    QToolButton* button = recentButtonOf(w);
    QVERIFY(button);
    QMenu* menu = button->menu();
    QVERIFY(menu);
    QCOMPARE(menu->actions().size(), 2);

    QSignalSpy recentSpy(&w, &DAFilePathEditWidget::recentPathActivated);
    QSignalSpy selSpy(&w, &DAFilePathEditWidget::selectedPath);
    menu->actions().first()->trigger();
    QCOMPARE(recentSpy.count(), 1);
    QCOMPARE(recentSpy.at(0).at(0).toString(), QStringLiteral("C:/a"));
    QCOMPARE(w.getFilePath(), QStringLiteral("C:/a"));
    QCOMPARE(selSpy.count(), 1);
}

void DaFilePathEditWidgetTest::testRecentButtonHiddenWithoutPaths()
{
    DAFilePathEditWidget w;
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    QTest::qWait(20);
    QToolButton* button = recentButtonOf(w);
    QVERIFY(button);
    w.setRecentPaths({});
    QVERIFY(button->isHidden());
    w.setRecentPaths({ QStringLiteral("C:/a") });
    QVERIFY(!button->isHidden());
    w.setRecentPaths({});
    QVERIFY(button->isHidden());
}

QTEST_MAIN(DaFilePathEditWidgetTest)
#include "tst_dafilepatheditwidget.moc"
