#include <QSignalSpy>
#include <QToolButton>
#include <QtTest/QtTest>

#include <DATextAlignmentEditWidget.h>

using namespace DA;

class DaTextAlignmentEditWidgetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDefaultValue();
    void testSetAlignmentRoundTrip();
    void testSetAlignmentPartialKeepsOtherAxis();
    void testSetAlignmentSameValueDoesNotEmit();
    void testClickUpdatesValueAndEmits();
    void testClickSameButtonDoesNotEmit();
    void testButtonGroupExclusive();
    void testTooltipsPresent();
};

// 默认值为左对齐 + 垂直居中（与表格默认一致）
void DaTextAlignmentEditWidgetTest::testDefaultValue()
{
    DATextAlignmentEditWidget w;
    QCOMPARE(w.horizontalAlignment(), Qt::Alignment(Qt::AlignLeft));
    QCOMPARE(w.verticalAlignment(), Qt::Alignment(Qt::AlignVCenter));
}

// setAlignment 往返：水平与垂直位都写入
void DaTextAlignmentEditWidgetTest::testSetAlignmentRoundTrip()
{
    DATextAlignmentEditWidget w;
    w.setAlignment(Qt::AlignRight | Qt::AlignTop);
    QCOMPARE(w.alignment(), Qt::Alignment(Qt::AlignRight | Qt::AlignTop));
}

// 只给水平位时垂直位保持原值（反之亦然）
void DaTextAlignmentEditWidgetTest::testSetAlignmentPartialKeepsOtherAxis()
{
    DATextAlignmentEditWidget w;
    w.setAlignment(Qt::AlignRight | Qt::AlignTop);
    w.setAlignment(Qt::AlignHCenter);  // 只改水平
    QCOMPARE(w.alignment(), Qt::Alignment(Qt::AlignHCenter | Qt::AlignTop));
    w.setAlignment(Qt::AlignBottom);  // 只改垂直
    QCOMPARE(w.alignment(), Qt::Alignment(Qt::AlignHCenter | Qt::AlignBottom));
}

// 设置相同值不发信号
void DaTextAlignmentEditWidgetTest::testSetAlignmentSameValueDoesNotEmit()
{
    DATextAlignmentEditWidget w;
    w.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);  // 与默认相同
    QSignalSpy spy(&w, &DATextAlignmentEditWidget::alignmentChanged);
    w.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QCOMPARE(spy.count(), 0);
}

// 点击按钮更新对应轴并发信号
void DaTextAlignmentEditWidgetTest::testClickUpdatesValueAndEmits()
{
    DATextAlignmentEditWidget w;
    QSignalSpy spy(&w, &DATextAlignmentEditWidget::alignmentChanged);
    QToolButton* btnRight = w.findChild< QToolButton* >(QStringLiteral("tbAlignRight"));
    QVERIFY(btnRight);
    QTest::mouseClick(btnRight, Qt::LeftButton);
    // 水平变为右对齐，垂直保持默认居中
    QCOMPARE(w.alignment(), Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    QCOMPARE(spy.count(), 1);

    QToolButton* btnBottom = w.findChild< QToolButton* >(QStringLiteral("tbAlignBottom"));
    QVERIFY(btnBottom);
    QTest::mouseClick(btnBottom, Qt::LeftButton);
    QCOMPARE(w.alignment(), Qt::Alignment(Qt::AlignRight | Qt::AlignBottom));
    QCOMPARE(spy.count(), 2);
}

// 重复点击已选中的按钮不改变值、不发信号
void DaTextAlignmentEditWidgetTest::testClickSameButtonDoesNotEmit()
{
    DATextAlignmentEditWidget w;
    QToolButton* btnLeft = w.findChild< QToolButton* >(QStringLiteral("tbAlignLeft"));
    QVERIFY(btnLeft);
    QSignalSpy spy(&w, &DATextAlignmentEditWidget::alignmentChanged);
    QTest::mouseClick(btnLeft, Qt::LeftButton);  // 已经是左对齐
    QCOMPARE(spy.count(), 0);
    QVERIFY(btnLeft->isChecked());
}

// 两行各自互斥：同一行点第二个按钮后第一个自动取消
void DaTextAlignmentEditWidgetTest::testButtonGroupExclusive()
{
    DATextAlignmentEditWidget w;
    QToolButton* btnLeft    = w.findChild< QToolButton* >(QStringLiteral("tbAlignLeft"));
    QToolButton* btnHCenter = w.findChild< QToolButton* >(QStringLiteral("tbAlignHCenter"));
    QToolButton* btnTop     = w.findChild< QToolButton* >(QStringLiteral("tbAlignTop"));
    QVERIFY(btnLeft && btnHCenter && btnTop);
    QTest::mouseClick(btnHCenter, Qt::LeftButton);
    QVERIFY(btnHCenter->isChecked());
    QVERIFY(!btnLeft->isChecked());
    // 垂直行不受水平行影响
    QVERIFY(!btnTop->isChecked());
}

// 六个按钮都有提示文本（空提示说明图标语义无文字兜底）
void DaTextAlignmentEditWidgetTest::testTooltipsPresent()
{
    DATextAlignmentEditWidget w;
    const QStringList names = { QStringLiteral("tbAlignLeft"),  QStringLiteral("tbAlignHCenter"),
                                QStringLiteral("tbAlignRight"), QStringLiteral("tbAlignTop"),
                                QStringLiteral("tbAlignVCenter"), QStringLiteral("tbAlignBottom") };
    for (const QString& n : names) {
        QToolButton* btn = w.findChild< QToolButton* >(n);
        QVERIFY2(btn, qPrintable(n));
        QVERIFY2(!btn->toolTip().isEmpty(), qPrintable(n));
    }
}

QTEST_MAIN(DaTextAlignmentEditWidgetTest)
#include "tst_datextalignmenteditwidget.moc"
