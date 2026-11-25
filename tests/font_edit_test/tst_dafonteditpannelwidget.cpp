#include <QAction>
#include <QAbstractItemDelegate>
#include <QAbstractItemView>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QImage>
#include <QMenu>
#include <QToolButton>
#include <QtTest/QtTest>

#include <DAFontEditPannelWidget.h>

using namespace DA;

class DaFontEditPannelWidgetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDefaultStateIsOff();
    void testPreviewDelegateRoundTrip();
    void testPreviewKeepsPanelHeight();
    void testPreviewLayoutPixelsUnchanged();
    void testPreviewRowHeightFollowsFontSize();
    void testPreviewRowHeightClamped();
    void testPreviewInkGrowsWithSize();
    void testPreviewSurvivesFontFilterChange();
    void testRecentFontsDedupAndCap();
    void testMaxRecentFontCountTruncates();
    void testRecentMenuUsesOwnFont();
    void testEmptyRecentListHidesButton();

private:
    static QFontComboBox* comboOf(DAFontEditPannelWidget& panel);
    static QToolButton* recentButtonOf(DAFontEditPannelWidget& panel);
    static bool hasFonts();
    static int darkPixels(const QImage& img);
};

QFontComboBox* DaFontEditPannelWidgetTest::comboOf(DAFontEditPannelWidget& panel)
{
    return panel.findChild< QFontComboBox* >();
}

QToolButton* DaFontEditPannelWidgetTest::recentButtonOf(DAFontEditPannelWidget& panel)
{
    return panel.findChild< QToolButton* >(QStringLiteral("recentFontButton"));
}

bool DaFontEditPannelWidgetTest::hasFonts()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return !QFontDatabase::families().isEmpty();
#else
    return !QFontDatabase().families().isEmpty();
#endif
}

int DaFontEditPannelWidgetTest::darkPixels(const QImage& img)
{
    int count = 0;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            const QColor c = img.pixelColor(x, y);
            if (c.red() + c.green() + c.blue() < 3 * 128)
                ++count;
        }
    }
    return count;
}

void DaFontEditPannelWidgetTest::testDefaultStateIsOff()
{
    DAFontEditPannelWidget panel;
    QVERIFY(!panel.isFontPreviewEnabled());
    QVERIFY(panel.recentFontFamilies().isEmpty());
    QToolButton* button = recentButtonOf(panel);
    QVERIFY(button);
    QVERIFY(button->isHidden());
    QCOMPARE(panel.maxRecentFontCount(), 5);
}

void DaFontEditPannelWidgetTest::testPreviewDelegateRoundTrip()
{
    DAFontEditPannelWidget panel;
    QFontComboBox* combo = comboOf(panel);
    QVERIFY(combo);
    QAbstractItemView* view = combo->view();
    QVERIFY(view);
    QAbstractItemDelegate* original = view->itemDelegate();
    QVERIFY(original);

    panel.setFontPreviewEnabled(true);
    QVERIFY(panel.isFontPreviewEnabled());
    QAbstractItemDelegate* preview = view->itemDelegate();
    QVERIFY(preview != original);

    panel.setFontPreviewEnabled(false);
    QVERIFY(!panel.isFontPreviewEnabled());
    QCOMPARE(view->itemDelegate(), original);
}

void DaFontEditPannelWidgetTest::testPreviewKeepsPanelHeight()
{
    DAFontEditPannelWidget panel;
    const QList< DAFontEditPannelWidget::LayoutMode > modes{ DAFontEditPannelWidget::TwoRowsLayout,
                                                             DAFontEditPannelWidget::SingleRowLayout };
    for (auto mode : modes) {
        panel.setLayoutMode(mode);
        const QSize hint = panel.sizeHint();
        const QSize minHint = panel.minimumSizeHint();
        panel.setFontPreviewEnabled(true);
        QCOMPARE(panel.sizeHint(), hint);
        QCOMPARE(panel.minimumSizeHint(), minHint);
        panel.setFontPreviewEnabled(false);
        QCOMPARE(panel.sizeHint(), hint);
        QCOMPARE(panel.minimumSizeHint(), minHint);
    }
}

void DaFontEditPannelWidgetTest::testPreviewLayoutPixelsUnchanged()
{
    DAFontEditPannelWidget panel;
    panel.resize(360, 80);
    const QImage before = panel.grab().toImage();
    panel.setFontPreviewEnabled(true);
    panel.setFontPreviewEnabled(false);
    const QImage after = panel.grab().toImage();
    QCOMPARE(after, before);
}

void DaFontEditPannelWidgetTest::testPreviewRowHeightFollowsFontSize()
{
    if (!hasFonts())
        QSKIP("no fonts available in this environment");
    DAFontEditPannelWidget panel;
    QFontComboBox* combo = comboOf(panel);
    QVERIFY(combo);
    QAbstractItemView* view = combo->view();
    QVERIFY(view);
    panel.setFontPreviewEnabled(true);

    panel.setFontWeight(6);
    const int small = view->sizeHintForRow(0);
    panel.setFontWeight(18);
    const int large = view->sizeHintForRow(0);
    QVERIFY2(large > small, qPrintable(QStringLiteral("small=%1 large=%2").arg(small).arg(large)));
}

void DaFontEditPannelWidgetTest::testPreviewRowHeightClamped()
{
    if (!hasFonts())
        QSKIP("no fonts available in this environment");
    DAFontEditPannelWidget panel;
    QFontComboBox* combo = comboOf(panel);
    QVERIFY(combo);
    QAbstractItemView* view = combo->view();
    QVERIFY(view);
    const int nativeRow = view->sizeHintForRow(0);

    panel.setFontPreviewEnabled(true);
    panel.setFontWeight(48);
    const int huge = view->sizeHintForRow(0);
    QVERIFY2(huge <= nativeRow * 2 + 2,
             qPrintable(QStringLiteral("huge=%1 native=%2").arg(huge).arg(nativeRow)));
}

void DaFontEditPannelWidgetTest::testPreviewInkGrowsWithSize()
{
    if (!hasFonts())
        QSKIP("no fonts available in this environment");
    DAFontEditPannelWidget panel;
    panel.resize(360, 80);
    panel.show();
    QVERIFY(QTest::qWaitForWindowExposed(&panel));
    QFontComboBox* combo = comboOf(panel);
    QVERIFY(combo);
    QAbstractItemView* view = combo->view();
    QVERIFY(view);

    panel.setFontWeight(18);
    combo->showPopup();
    QTest::qWait(120);
    const QImage off = view->viewport()->grab().toImage();
    combo->hidePopup();
    QTest::qWait(40);

    panel.setFontPreviewEnabled(true);
    combo->showPopup();
    QTest::qWait(120);
    const QImage on = view->viewport()->grab().toImage();
    combo->hidePopup();

    QVERIFY2(darkPixels(on) > darkPixels(off),
             qPrintable(QStringLiteral("off=%1 on=%2").arg(darkPixels(off)).arg(darkPixels(on))));
}

void DaFontEditPannelWidgetTest::testPreviewSurvivesFontFilterChange()
{
    DAFontEditPannelWidget panel;
    QFontComboBox* combo = comboOf(panel);
    QVERIFY(combo);
    QAbstractItemView* view = combo->view();
    QVERIFY(view);
    panel.setFontPreviewEnabled(true);
    QAbstractItemDelegate* mine = view->itemDelegate();
    combo->setFontFilters(QFontComboBox::MonospacedFonts);
    QCOMPARE(view->itemDelegate(), mine);
}

void DaFontEditPannelWidgetTest::testRecentFontsDedupAndCap()
{
    DAFontEditPannelWidget panel;
    panel.setRecentFontFamilies({ QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("A"),
                                  QString(), QStringLiteral("C"), QStringLiteral("D"),
                                  QStringLiteral("E"), QStringLiteral("F") });
    const QStringList expected{ QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("C"),
                                QStringLiteral("D"), QStringLiteral("E") };
    QCOMPARE(panel.recentFontFamilies(), expected);
    QToolButton* button = recentButtonOf(panel);
    QVERIFY(button);
    QVERIFY(button->menu());
    QCOMPARE(button->menu()->actions().size(), 5);
}

void DaFontEditPannelWidgetTest::testMaxRecentFontCountTruncates()
{
    DAFontEditPannelWidget panel;
    panel.setRecentFontFamilies({ QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("C"),
                                  QStringLiteral("D"), QStringLiteral("E") });
    QCOMPARE(panel.recentFontFamilies().size(), 5);

    panel.setMaxRecentFontCount(2);
    QCOMPARE(panel.maxRecentFontCount(), 2);
    QCOMPARE(panel.recentFontFamilies().size(), 2);
    QCOMPARE(recentButtonOf(panel)->menu()->actions().size(), 2);

    panel.setMaxRecentFontCount(0);
    QCOMPARE(panel.maxRecentFontCount(), 1);
    QCOMPARE(panel.recentFontFamilies().size(), 1);
}

void DaFontEditPannelWidgetTest::testRecentMenuUsesOwnFont()
{
    DAFontEditPannelWidget panel;
    panel.setRecentFontFamilies({ QStringLiteral("Arial"), QStringLiteral("Courier New") });
    QToolButton* button = recentButtonOf(panel);
    QVERIFY(button);
    QMenu* menu = button->menu();
    QVERIFY(menu);
    const auto actions = menu->actions();
    QCOMPARE(actions.size(), 2);
    QCOMPARE(actions.at(0)->font().family(), QStringLiteral("Arial"));
    QCOMPARE(actions.at(1)->font().family(), QStringLiteral("Courier New"));
    QCOMPARE(actions.at(0)->data().toString(), QStringLiteral("Arial"));
    QCOMPARE(actions.at(1)->data().toString(), QStringLiteral("Courier New"));
}

void DaFontEditPannelWidgetTest::testEmptyRecentListHidesButton()
{
    DAFontEditPannelWidget panel;
    const QSize initial = panel.sizeHint();
    panel.setRecentFontFamilies({ QStringLiteral("Arial"), QStringLiteral("Courier New") });
    QToolButton* button = recentButtonOf(panel);
    QVERIFY(button);
    QVERIFY(!button->isHidden());

    panel.setRecentFontFamilies({});
    QVERIFY(button->isHidden());
    QCOMPARE(button->menu()->actions().size(), 0);
    QCOMPARE(panel.sizeHint(), initial);
}

QTEST_MAIN(DaFontEditPannelWidgetTest)
#include "tst_dafonteditpannelwidget.moc"
