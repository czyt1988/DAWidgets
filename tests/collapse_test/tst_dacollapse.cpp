#include <QtTest>
#include <QLabel>
#include <QPixmap>
#include <QSignalSpy>

#include "DACollapse.h"
#include "DACollapseItem.h"

using namespace DA;

/// @brief DACollapse / DACollapseItem 行为测试,对标 Element UI Collapse 语义
class DACollapseTest : public QObject
{
    Q_OBJECT
private slots:
    void testBasicExpand();
    void testActiveNames();
    void testAccordionMutualExclusion();
    void testAccordionSetActiveNamesTakesFirst();
    void testDisabledProperty();
    void testDisabledNotActivatable();
    void testArrowIconCachedAndDisabled();
    void testDisabledVisualState();
    void testRemoveAndClear();
    void testNameTitle();
    void testContentWidget();
};

void DACollapseTest::testBasicExpand()
{
    DACollapseItem item(QStringLiteral("t"));
    item.setAnimationEnabled(false);  // 关闭动画,避免时序依赖
    QSignalSpy spy(&item, &DACollapseItem::expandChanged);

    QVERIFY(!item.expanded());
    item.setExpanded(true);
    QVERIFY(item.expanded());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);

    item.setExpanded(true);  // 无变化,不应再触发
    QCOMPARE(spy.count(), 0);

    item.setExpanded(false);
    QVERIFY(!item.expanded());
    QCOMPARE(spy.count(), 1);
}

void DACollapseTest::testActiveNames()
{
    DACollapse c;
    c.addItem(QStringLiteral("a"), QStringLiteral("1"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("b"), QStringLiteral("2"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("c"), QStringLiteral("3"))->setAnimationEnabled(false);
    QSignalSpy spy(&c, &DACollapse::activeNamesChanged);

    c.setActiveNames({ QStringLiteral("2"), QStringLiteral("3") });
    QCOMPARE(c.activeNames(), QStringList({ QStringLiteral("2"), QStringLiteral("3") }));
    QVERIFY(c.item(QStringLiteral("2"))->expanded());
    QVERIFY(c.item(QStringLiteral("3"))->expanded());
    QVERIFY(!c.item(QStringLiteral("1"))->expanded());
    QCOMPARE(spy.count(), 1);
}

void DACollapseTest::testAccordionMutualExclusion()
{
    DACollapse c;
    c.setAccordion(true);
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    auto* i3 = c.addItem(QStringLiteral("c"), QStringLiteral("3"));
    for (auto* it : c.items())
        it->setAnimationEnabled(false);

    i1->setExpanded(true);
    i2->setExpanded(true);  // 展开新面板应折叠 i1

    QVERIFY(!i1->expanded());
    QVERIFY(i2->expanded());
    QVERIFY(!i3->expanded());
    QCOMPARE(c.activeNames(), QStringList { QStringLiteral("2") });
}

void DACollapseTest::testAccordionSetActiveNamesTakesFirst()
{
    DACollapse c;
    c.setAccordion(true);
    c.addItem(QStringLiteral("a"), QStringLiteral("1"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("b"), QStringLiteral("2"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("c"), QStringLiteral("3"))->setAnimationEnabled(false);

    // 传入多个,手风琴下仅取首个存在的
    c.setActiveNames({ QStringLiteral("1"), QStringLiteral("3") });
    QVERIFY(c.item(QStringLiteral("1"))->expanded());
    QVERIFY(!c.item(QStringLiteral("3"))->expanded());
    QCOMPARE(c.activeNames(), QStringList { QStringLiteral("1") });
}

void DACollapseTest::testDisabledProperty()
{
    DACollapseItem item(QStringLiteral("t"));
    QSignalSpy spy(&item, &DACollapseItem::disabledChanged);

    QVERIFY(!item.disabled());
    item.setDisabled(true);
    QVERIFY(item.disabled());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);
}

void DACollapseTest::testDisabledNotActivatable()
{
    DACollapse c;
    c.addItem(QStringLiteral("a"), QStringLiteral("1"))->setAnimationEnabled(false);
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    i2->setAnimationEnabled(false);
    i2->setDisabled(true);

    // 禁用面板不应被激活
    c.setActiveNames({ QStringLiteral("2") });
    QVERIFY(!i2->expanded());
    QVERIFY(c.activeNames().isEmpty());
}

void DACollapseTest::testArrowIconCachedAndDisabled()
{
    DACollapseItem item(QStringLiteral("t"));
    item.setAnimationEnabled(false);

    // 折叠态箭头应已渲染(非空 pixmap)
    auto* arrow = item.findChild< QLabel* >(QStringLiteral("DACollapseItemArrow"));
    QVERIFY(arrow != nullptr);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QVERIFY(!arrow->pixmap().isNull());
#else
    QVERIFY(arrow->pixmap() && !arrow->pixmap()->isNull());
#endif

    // 展开后箭头切换为另一图标
    item.setExpanded(true);

    // disabled 切换应刷新箭头:Disabled 模式由 QIcon 自动生成灰度版本,非空
    item.setDisabled(true);
    arrow = item.findChild< QLabel* >(QStringLiteral("DACollapseItemArrow"));
    QVERIFY(arrow != nullptr);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QVERIFY(!arrow->pixmap().isNull());
#else
    QVERIFY(arrow->pixmap() && !arrow->pixmap()->isNull());
#endif
}

void DACollapseTest::testDisabledVisualState()
{
    DACollapseItem item(QStringLiteral("t"));
    item.setAnimationEnabled(false);

    auto* title = item.findChild< QLabel* >(QStringLiteral("DACollapseItemTitle"));
    QVERIFY(title != nullptr);

    // 正常态标题色应非灰(#c0c4cc)
    const QColor gray(QStringLiteral("#c0c4cc"));
    QVERIFY(title->palette().color(QPalette::WindowText) != gray);

    // 禁用后标题色应变灰
    item.setDisabled(true);
    QCOMPARE(title->palette().color(QPalette::WindowText), gray);

    // 恢复后应还原为正常色
    item.setDisabled(false);
    QVERIFY(title->palette().color(QPalette::WindowText) != gray);
}

void DACollapseTest::testRemoveAndClear()
{
    DACollapse c;
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    QCOMPARE(c.items().size(), 2);

    c.removeItem(i1);
    QCOMPARE(c.items().size(), 1);
    QVERIFY(c.item(QStringLiteral("1")) == nullptr);

    c.clear();
    QCOMPARE(c.items().size(), 0);
}

void DACollapseTest::testNameTitle()
{
    DACollapseItem item;
    QSignalSpy ns(&item, &DACollapseItem::nameChanged);
    QSignalSpy ts(&item, &DACollapseItem::titleChanged);

    item.setName(QStringLiteral("x"));
    QCOMPARE(item.name(), QStringLiteral("x"));
    QCOMPARE(ns.count(), 1);

    item.setTitle(QStringLiteral("hello"));
    QCOMPARE(item.title(), QStringLiteral("hello"));
    QCOMPARE(ts.count(), 1);
}

void DACollapseTest::testContentWidget()
{
    DACollapseItem item;
    auto* label = new QLabel(QStringLiteral("content"));
    item.setContentWidget(label);
    QCOMPARE(item.contentWidget(), label);

    auto* label2 = new QLabel(QStringLiteral("replaced"));
    item.setContentWidget(label2);  // 旧内容应被销毁
    QCOMPARE(item.contentWidget(), label2);
}

QTEST_MAIN(DACollapseTest)
#include "tst_dacollapse.moc"
