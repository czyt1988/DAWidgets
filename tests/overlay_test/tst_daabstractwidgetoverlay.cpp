#include <QApplication>
#include <QPainter>
#include <QWidget>
#include <QtTest/QtTest>

#include <DAAbstractWidgetOverlay.h>

using namespace DA;

namespace
{
class TestOverlay : public DAAbstractWidgetOverlay
{
public:
    using DAAbstractWidgetOverlay::DAAbstractWidgetOverlay;

protected:
    void drawOverlay(QPainter* painter) const override
    {
        painter->fillRect(rect(), QColor(0, 0, 0, 40));
    }
};
}  // namespace

class DaAbstractWidgetOverlayTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testConstructionAndDefaults();
    void testMaskModeRoundTrip();
    void testRenderModeRoundTrip();
    void testFollowsParentResize();
    void testMaskRegions();
    void testUpdateOverlay();
};

void DaAbstractWidgetOverlayTest::testConstructionAndDefaults()
{
    QWidget parent;
    parent.resize(120, 80);
    TestOverlay overlay(&parent);
    QCOMPARE(overlay.size(), QSize(120, 80));
    QCOMPARE(overlay.getMaskMode(), DAAbstractWidgetOverlay::MaskHint);
    QCOMPARE(overlay.getRenderMode(), DAAbstractWidgetOverlay::AutoRenderMode);
    QVERIFY(overlay.testAttribute(Qt::WA_TransparentForMouseEvents));
}

void DaAbstractWidgetOverlayTest::testMaskModeRoundTrip()
{
    QWidget parent;
    TestOverlay overlay(&parent);
    overlay.setMaskMode(DAAbstractWidgetOverlay::AlphaMask);
    QCOMPARE(overlay.getMaskMode(), DAAbstractWidgetOverlay::AlphaMask);
    overlay.setMaskMode(DAAbstractWidgetOverlay::NoMask);
    QCOMPARE(overlay.getMaskMode(), DAAbstractWidgetOverlay::NoMask);
}

void DaAbstractWidgetOverlayTest::testRenderModeRoundTrip()
{
    QWidget parent;
    TestOverlay overlay(&parent);
    overlay.setRenderMode(DAAbstractWidgetOverlay::CopyAlphaMask);
    QCOMPARE(overlay.getRenderMode(), DAAbstractWidgetOverlay::CopyAlphaMask);
    overlay.setRenderMode(DAAbstractWidgetOverlay::DrawOverlay);
    QCOMPARE(overlay.getRenderMode(), DAAbstractWidgetOverlay::DrawOverlay);
}

void DaAbstractWidgetOverlayTest::testFollowsParentResize()
{
    QWidget parent;
    parent.resize(100, 60);
    TestOverlay overlay(&parent);
    parent.show();
    QVERIFY(QTest::qWaitForWindowExposed(&parent));
    parent.resize(200, 140);
    // 事件过滤器在父控件 resize 时同步尺寸
    QApplication::processEvents();
    QCOMPARE(overlay.size(), QSize(200, 140));
}

void DaAbstractWidgetOverlayTest::testMaskRegions()
{
    const QRegion box = DAAbstractWidgetOverlay::maskRegion(QRect(10, 10, 20, 20), 2);
    QVERIFY(!box.isEmpty());
    const QRegion hline = DAAbstractWidgetOverlay::maskRegionVOrHLine(QLine(0, 10, 40, 10), 2);
    QVERIFY(!hline.isEmpty());
    const QRegion vline = DAAbstractWidgetOverlay::maskRegionVOrHLine(QLine(10, 0, 10, 40), 2);
    QVERIFY(!vline.isEmpty());
}

void DaAbstractWidgetOverlayTest::testUpdateOverlay()
{
    QWidget parent;
    parent.resize(50, 50);
    TestOverlay overlay(&parent);
    overlay.setMaskMode(DAAbstractWidgetOverlay::AlphaMask);
    // 不崩溃即可（遮罩计算 + 重绘）
    overlay.updateOverlay();
    QVERIFY(true);
}

QTEST_MAIN(DaAbstractWidgetOverlayTest)
#include "tst_daabstractwidgetoverlay.moc"
