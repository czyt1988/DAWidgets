#include <QAction>
#include <QLabel>
#include <QSlider>
#include <QSignalSpy>
#include <QToolButton>
#include <QtTest/QtTest>

#include <DAColorGridWidget.h>
#include <DAColorMenu.h>
#include <DAColorPaletteGridWidget.h>

using namespace DA;

class DaColorMenuTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDefaultStateUnchanged();
    void testAlphaEnabledBuildsRow();
    void testAlphaBoundaries();
    void testSetAlphaClampsAndEmits();
    void testAlphaSliderWiring();
    void testAlphaSliderDragInMenu();
    void testNoneColorStillInvalid();
    void testRecentColorsDedupeAndTruncate();
    void testRecentColorsRecordedOnPick();
    void testRecentGridShowsRecentAfterShow();
    void testRestoreDefaultColor();
    void testDefaultColorActionAbsentByDefault();
    void testArrowKeyNavigation();
    void testEnterActivatesColor();
    void testAlphaBackgroundPixels();

private:
    static QToolButton* firstThemeButton(DAColorMenu& menu);
    static QAction* findAction(DAColorMenu& menu, const QString& text);
    static QList<QColor> makeColors(int n);
};

QToolButton* DaColorMenuTest::firstThemeButton(DAColorMenu& menu)
{
    auto* palette = menu.colorPaletteGridWidget();
    if (!palette)
        return nullptr;
    const auto buttons = palette->findChildren< QToolButton* >();
    return buttons.isEmpty() ? nullptr : buttons.first();
}

QAction* DaColorMenuTest::findAction(DAColorMenu& menu, const QString& text)
{
    const auto actions = menu.actions();
    for (QAction* a : actions) {
        if (a->text() == text)
            return a;
    }
    return nullptr;
}

QList<QColor> DaColorMenuTest::makeColors(int n)
{
    QList<QColor> out;
    for (int i = 0; i < n; ++i)
        out << QColor(20 + i * 25, 40 + i * 15, 60 + i * 10);
    return out;
}

void DaColorMenuTest::testDefaultStateUnchanged()
{
    DAColorMenu menu;
    QVERIFY(!menu.isAlphaEnabled());
    QCOMPARE(menu.alpha(), 255);
    QVERIFY(!menu.findChild< QSlider* >(QStringLiteral("DAAlphaSlider")));

    QToolButton* btn = firstThemeButton(menu);
    QVERIFY(btn);
    const QColor expected = btn->property("color").value< QColor >();
    QSignalSpy spy(&menu, &DAColorMenu::selectedColor);
    btn->click();

    QCOMPARE(spy.count(), 1);
    const QColor got = spy.at(0).at(0).value< QColor >();
    QVERIFY(got.isValid());
    QCOMPARE(got.alpha(), 255);
    QCOMPARE(got.name(), expected.name());
}

void DaColorMenuTest::testAlphaEnabledBuildsRow()
{
    DAColorMenu menu;
    menu.setAlphaEnabled(true);
    QVERIFY(menu.isAlphaEnabled());
    auto* slider = menu.findChild< QSlider* >(QStringLiteral("DAAlphaSlider"));
    QVERIFY(slider);
    QCOMPARE(slider->value(), 255);
    auto* value = menu.findChild< QLabel* >(QStringLiteral("DAAlphaValueLabel"));
    QVERIFY(value);
    QCOMPARE(value->text(), QStringLiteral("100%"));
}

void DaColorMenuTest::testAlphaBoundaries()
{
    DAColorMenu menu;
    menu.setAlphaEnabled(true);
    QSignalSpy spy(&menu, &DAColorMenu::selectedColor);
    const QList<int> alphas{ 0, 128, 255 };
    for (int a : alphas) {
        menu.setAlpha(a);
        menu.emitSelectedColor(QColor(64, 158, 255));
        QCOMPARE(spy.count(), 1);
        const QColor got = spy.takeFirst().at(0).value< QColor >();
        QCOMPARE(got.alpha(), a);
        QCOMPARE(got.red(), 64);
        QCOMPARE(got.green(), 158);
        QCOMPARE(got.blue(), 255);
    }
}

void DaColorMenuTest::testSetAlphaClampsAndEmits()
{
    DAColorMenu menu;
    QSignalSpy spy(&menu, &DAColorMenu::alphaChanged);

    menu.setAlpha(-5);
    QCOMPARE(menu.alpha(), 0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);

    menu.setAlpha(-5);  // 值不变不重复发
    QCOMPARE(spy.count(), 1);

    menu.setAlpha(999);
    QCOMPARE(menu.alpha(), 255);
    QCOMPARE(spy.count(), 2);

    menu.setAlpha(255);
    QCOMPARE(spy.count(), 2);
}

void DaColorMenuTest::testAlphaSliderWiring()
{
    DAColorMenu menu;
    menu.setAlphaEnabled(true);
    auto* slider = menu.findChild< QSlider* >(QStringLiteral("DAAlphaSlider"));
    QVERIFY(slider);
    QSignalSpy spy(&menu, &DAColorMenu::alphaChanged);

    slider->setValue(64);
    QCOMPARE(menu.alpha(), 64);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 64);
    auto* value = menu.findChild< QLabel* >(QStringLiteral("DAAlphaValueLabel"));
    QVERIFY(value);
    QCOMPARE(value->text(), QStringLiteral("25%"));
}

void DaColorMenuTest::testAlphaSliderDragInMenu()
{
    DAColorMenu menu;
    menu.setAlphaEnabled(true);
    menu.show();
    QVERIFY(QTest::qWaitForWindowExposed(&menu));
    auto* slider = menu.findChild< QSlider* >(QStringLiteral("DAAlphaSlider"));
    QVERIFY(slider);
    QVERIFY(slider->isVisible());

    QSignalSpy spy(&menu, &DAColorMenu::alphaChanged);
    const QPoint start(slider->width() - 2, slider->height() / 2);
    const QPoint end(slider->width() / 2, slider->height() / 2);
    QTest::mousePress(slider, Qt::LeftButton, Qt::NoModifier, start);
    QTest::mouseMove(slider, end, 10);
    QTest::mouseRelease(slider, Qt::LeftButton, Qt::NoModifier, end);

    QVERIFY2(spy.count() > 0, "dragging the slider must emit alphaChanged");
    QVERIFY(menu.alpha() < 255);
    menu.close();
}

void DaColorMenuTest::testNoneColorStillInvalid()
{
    DAColorMenu menu;
    menu.setAlphaEnabled(true);
    menu.setAlpha(64);
    menu.enableNoneColorAction(true);
    QVERIFY(menu.noneColorAction());

    QSignalSpy spy(&menu, &DAColorMenu::selectedColor);
    menu.noneColorAction()->trigger();
    QCOMPARE(spy.count(), 1);
    QVERIFY(!spy.at(0).at(0).value< QColor >().isValid());
}

void DaColorMenuTest::testRecentColorsDedupeAndTruncate()
{
    DAColorMenu menu;
    const QList<QColor> base = makeColors(7);
    QList<QColor> input;
    input << base.at(0) << base.at(1) << base.at(0) << base.at(2) << base.at(3) << base.at(4)
          << base.at(5) << base.at(1);

    menu.setRecentColors(input);
    const QList<QColor> recent = menu.recentColors();
    QCOMPARE(recent.size(), 5);
    QCOMPARE(recent.first().toRgb().rgba(), base.at(0).toRgb().rgba());
    for (int i = 1; i < recent.size(); ++i) {
        for (int j = 0; j < i; ++j)
            QVERIFY(recent.at(i).toRgb().rgba() != recent.at(j).toRgb().rgba());
    }
}

void DaColorMenuTest::testRecentColorsRecordedOnPick()
{
    DAColorMenu menu;
    menu.setRecentColors({});
    const QColor a(64, 158, 255);
    const QColor b(103, 194, 58);

    menu.emitSelectedColor(a);
    menu.emitSelectedColor(b);
    menu.emitSelectedColor(a);

    const QList<QColor> recent = menu.recentColors();
    QCOMPARE(recent.size(), 2);
    QCOMPARE(recent.at(0).toRgb().rgba(), a.toRgb().rgba());
    QCOMPARE(recent.at(1).toRgb().rgba(), b.toRgb().rgba());
}

void DaColorMenuTest::testRecentGridShowsRecentAfterShow()
{
    DAColorMenu menu;
    const QList<QColor> colors = makeColors(3);
    menu.setRecentColors(colors);
    QCOMPARE(menu.recentColors().size(), 3);
    const QList<QColor> grid = menu.customColorsWidget()->getColorList();
    QCOMPARE(grid.size(), 3);
    for (int i = 0; i < 3; ++i)
        QCOMPARE(grid.at(i).toRgb().rgba(), colors.at(i).toRgb().rgba());
}

void DaColorMenuTest::testRestoreDefaultColor()
{
    DAColorMenu menu;
    menu.setAlphaEnabled(true);
    menu.setAlpha(80);
    const QColor def(64, 158, 255);
    menu.setRecentColors({ QColor(1, 2, 3) });
    menu.setDefaultColor(def);
    QAction* action = findAction(menu, QStringLiteral("Restore Default"));
    QVERIFY(action);

    QSignalSpy spy(&menu, &DAColorMenu::selectedColor);
    const int recentBefore = menu.recentColors().size();
    action->trigger();

    QCOMPARE(spy.count(), 1);
    const QColor got = spy.at(0).at(0).value< QColor >();
    QCOMPARE(got.name(), def.name());
    QCOMPARE(got.alpha(), 255);  // 恢复默认不套用滑杆 alpha
    QCOMPARE(menu.recentColors().size(), recentBefore);
}

void DaColorMenuTest::testDefaultColorActionAbsentByDefault()
{
    DAColorMenu menu;
    QVERIFY(!findAction(menu, QStringLiteral("Restore Default")));
    menu.setDefaultColor(QColor(64, 158, 255));
    QVERIFY(findAction(menu, QStringLiteral("Restore Default")));
    menu.setDefaultColor(QColor());
    QVERIFY(!findAction(menu, QStringLiteral("Restore Default")));
}

void DaColorMenuTest::testArrowKeyNavigation()
{
    DAColorGridWidget grid;
    grid.setColorList(makeColors(6));
    grid.setColumnCount(4);
    grid.setKeyboardNavigationEnabled(true);
    grid.resize(300, 300);
    grid.show();
    QVERIFY(QTest::qWaitForWindowExposed(&grid));
    grid.activateWindow();
    QTest::qWaitForWindowActive(&grid);
    QTest::qWait(20);

    QToolButton* b0 = grid.colorButton(0);
    QToolButton* b1 = grid.colorButton(1);
    QToolButton* b4 = grid.colorButton(4);
    QToolButton* b5 = grid.colorButton(5);
    QVERIFY(b0 && b1 && b4 && b5);

    b0->setFocus(Qt::TabFocusReason);
    QVERIFY(b0->hasFocus());
    QTest::keyClick(b0, Qt::Key_Right);
    QVERIFY(b1->hasFocus());
    QTest::keyClick(b1, Qt::Key_Down);
    QVERIFY(b5->hasFocus());
    QTest::keyClick(b5, Qt::Key_Down);  // 越界不动
    QVERIFY(b5->hasFocus());
    QTest::keyClick(b5, Qt::Key_Left);
    QVERIFY(b4->hasFocus());
    QTest::keyClick(b4, Qt::Key_Left);  // 第一列不动
    QVERIFY(b4->hasFocus());
}

void DaColorMenuTest::testEnterActivatesColor()
{
    DAColorGridWidget grid;
    const QList<QColor> colors = makeColors(3);
    grid.setColorList(colors);
    grid.setColumnCount(3);
    grid.setKeyboardNavigationEnabled(true);
    grid.resize(300, 200);
    grid.show();
    QVERIFY(QTest::qWaitForWindowExposed(&grid));
    grid.activateWindow();
    QTest::qWaitForWindowActive(&grid);
    QTest::qWait(20);

    QToolButton* b0 = grid.colorButton(0);
    QVERIFY(b0);
    b0->setFocus(Qt::TabFocusReason);
    QSignalSpy spy(&grid, &DAColorGridWidget::colorClicked);
    QTest::keyClick(b0, Qt::Key_Return);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value< QColor >().toRgb().rgba(), colors.at(0).toRgb().rgba());
}

void DaColorMenuTest::testAlphaBackgroundPixels()
{
    DAColorGridWidget grid;
    grid.setColorList({ QColor(64, 158, 255, 128) });
    grid.setAlphaBackgroundEnabled(true);
    grid.resize(100, 100);
    grid.show();
    QVERIFY(QTest::qWaitForWindowExposed(&grid));
    QTest::qWait(30);

    auto centerPixel = [ &grid ](QColor* out) -> bool {
        QToolButton* btn = grid.colorButton(0);
        if (!btn)
            return false;
        const QImage img = grid.grab().toImage();
        const QPoint c = btn->geometry().center();
        if (!img.rect().contains(c))
            return false;
        *out = img.pixelColor(c);
        return true;
    };

    QColor pixel;
    QVERIFY(centerPixel(&pixel));
    int diff = qAbs(pixel.red() - 64) + qAbs(pixel.green() - 158) + qAbs(pixel.blue() - 255);
    QVERIFY2(diff > 30, qPrintable(QStringLiteral("alpha 128 pixel=%1 diff=%2").arg(pixel.name()).arg(diff)));

    grid.setColorList({ QColor(64, 158, 255) });
    QTest::qWait(30);
    QVERIFY(centerPixel(&pixel));
    diff = qAbs(pixel.red() - 64) + qAbs(pixel.green() - 158) + qAbs(pixel.blue() - 255);
    QVERIFY2(diff <= 2, qPrintable(QStringLiteral("opaque pixel=%1 diff=%2").arg(pixel.name()).arg(diff)));
}

QTEST_MAIN(DaColorMenuTest)
#include "tst_dacolormenu.moc"
