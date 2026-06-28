#include <QtTest>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QResizeEvent>
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
    void testCollapseAnimationResetsMaxHeight();
    void testContentNaturalHeightForWordWrap();
    void testExpandDoesNotFlickerSibling();
    void testDebugLoggingOnExpand();
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

void DACollapseTest::testCollapseAnimationResetsMaxHeight()
{
    // 回归:折叠动画起始值必须基于 content 实际高度,而非 maximumHeight()。
    // 早期版本 onAnimationFinished 把 maximumHeight 设为 QWIDGETSIZE_MAX,折叠时
    // 直接拿 maximumHeight() 做起始值,动画走 16777215->0,前 99% 周期无视觉效果。
    // 修复后:onAnimationFinished 锁定 maximumHeight 为实际高度(同时解决兄弟面板
    // 闪烁),折叠时 doExpand 用 height() 作为起始值。
    DACollapseItem item(QStringLiteral("t"));
    item.setAnimationEnabled(true);
    auto* lbl = new QLabel(QStringLiteral("line1\nline2\nline3\nline4"));
    item.setContentWidget(lbl);
    item.resize(200, 400);
    item.show();
    QVERIFY(QTest::qWaitForWindowExposed(&item));

    auto* content = item.findChild< QFrame* >(QStringLiteral("DACollapseItemContent"));
    QVERIFY(content != nullptr);

    // 展开并等待 onAnimationFinished 锁定 maximumHeight
    item.setExpanded(true);
    QTest::qWait(600);

    const int naturalH = content->height();
    QVERIFY2(naturalH > 0, "expanded content must have non-zero height");
    QVERIFY2(naturalH < QWIDGETSIZE_MAX,
             "actual height must be far below QWIDGETSIZE_MAX for the test to be meaningful");

    // 修复后:maximumHeight 锁定在自然高度,不再是 QWIDGETSIZE_MAX
    QVERIFY2(content->maximumHeight() != QWIDGETSIZE_MAX,
             "onAnimationFinished must pin maximumHeight to actual height, not "
             "QWIDGETSIZE_MAX (which causes dead collapse animation + sibling flicker)");
    QCOMPARE(content->maximumHeight(), naturalH);

    // 触发折叠:起始值必须是实际高度
    item.setExpanded(false);
    QCOMPARE(content->maximumHeight(), naturalH);

    // 折叠动画应正常结束并隐藏 content
    QTRY_VERIFY_WITH_TIMEOUT(!content->isVisible(), 2000);
}

void DACollapseTest::testContentNaturalHeightForWordWrap()
{
    // 回归:contentNaturalHeight() 必须用 heightForWidth(item宽度) 计算,而非
    // layout->sizeHint().height()。sizeHint 基于 idealWidth(可能比实际宽度窄或宽),
    // 对 wordWrap(heightForWidth) content 的高度计算不准,导致动画 target 错误、
    // 结束后高度跳变。修复后展开高度应匹配 heightForWidth(实际宽度)。
    DACollapseItem item(QStringLiteral("t"));
    auto* lbl = new QLabel;
    lbl->setWordWrap(true);
    lbl->setText(QStringLiteral("与现实生活一致：与现实生活的流程、逻辑保持一致，遵循用户习惯的语言和概念；"
                                "在界面中一致：所有的元素和结构需保持一致，比如：设计样式、图标和文本、元素的位置等。"));
    item.setContentWidget(lbl);
    item.resize(500, 400);
    item.show();
    QVERIFY(QTest::qWaitForWindowExposed(&item));

    auto* content = item.findChild< QFrame* >(QStringLiteral("DACollapseItemContent"));
    QVERIFY(content != nullptr);
    auto* lay = content->layout();

    const int sizeHintH = lay->sizeHint().height();
    const int hfwH      = lay->heightForWidth(item.width());
    QVERIFY2(hfwH > 0, "heightForWidth must return a valid positive height");

    // 展开后实际高度应匹配 heightForWidth(实际宽度),而非 sizeHint(idealWidth)
    item.setAnimationEnabled(false);
    item.setExpanded(true);
    QTest::qWait(50);
    const int actualH = content->height();
    QVERIFY2(qAbs(actualH - hfwH) <= 4,
             qPrintable(QStringLiteral("actualH=%1 should match heightForWidth(%2)=%3, "
                                        "not sizeHint=%4")
                            .arg(actualH).arg(item.width()).arg(hfwH).arg(sizeHintH)));
    // 若 sizeHint 与 hfwH 本就不同,actualH 必须远离 sizeHint(证明用的是 hfw)
    if (qAbs(sizeHintH - hfwH) > 4) {
        QVERIFY2(qAbs(actualH - sizeHintH) > 4,
                 qPrintable(QStringLiteral("actualH=%1 should NOT match sizeHint=%2 "
                                           "(should use heightForWidth=%3 instead)")
                                .arg(actualH).arg(sizeHintH).arg(hfwH)));
    }
}

namespace {
class HeightProbe : public QObject
{
    Q_OBJECT
public:
    QList<int> heights;
protected:
    bool eventFilter(QObject*, QEvent* e) override
    {
        if (e->type() == QEvent::Resize) {
            auto* re = static_cast< QResizeEvent* >(e);
            heights << re->size().height();
        }
        return false;
    }
};
}  // namespace

void DACollapseTest::testExpandDoesNotFlickerSibling()
{
    // 回归:非手风琴模式下展开 item2 时,item1(已展开)的 content 不应被布局
    // 挤压导致"收缩又伸长"闪烁。根因:onAnimationFinished 把 content maximumHeight
    // 设为 QWIDGETSIZE_MAX,使 content 在父高度异步增长时被挤压。修复后 maximumHeight
    // 锁定在自然高度,content 不受兄弟面板展开影响。
    // 模拟 demo 结构:DACollapse 在 root VBox 中,高度由布局动态决定,窗口空间紧张。
    QWidget root;
    auto* rootLay = new QVBoxLayout(&root);
    rootLay->setContentsMargins(0, 0, 0, 0);
    rootLay->addWidget(new QLabel(QStringLiteral("基础用法")));

    auto* c = new DA::DACollapse(&root);
    auto* i1 = c->addItem(QStringLiteral("一致性"), QStringLiteral("1"));
    auto* lbl1 = new QLabel;
    lbl1->setWordWrap(true);
    lbl1->setText(QStringLiteral("与现实生活一致：与现实生活的流程、逻辑保持一致，遵循用户习惯的语言和概念；"
                                 "在界面中一致：所有的元素和结构需保持一致，比如：设计样式、图标和文本、元素的位置等。"));
    i1->setContentWidget(lbl1);

    auto* i2 = c->addItem(QStringLiteral("反馈"), QStringLiteral("2"));
    auto* lbl2 = new QLabel;
    lbl2->setWordWrap(true);
    lbl2->setText(QStringLiteral("控制反馈：通过界面样式和交互动效让用户可以清晰的感知自己的操作；"
                                 "页面反馈：操作后，通过页面元素的变化清晰地展现当前状态。"));
    i2->setContentWidget(lbl2);

    auto* i3 = c->addItem(QStringLiteral("效率"), QStringLiteral("3"));
    auto* lbl3 = new QLabel;
    lbl3->setWordWrap(true);
    lbl3->setText(QStringLiteral("简化流程：设计简洁直观的操作流程；清晰明确：语言表达清晰且表意明确。"));
    i3->setContentWidget(lbl3);

    rootLay->addWidget(c);
    rootLay->addWidget(new QLabel(QStringLiteral("状态")));
    rootLay->addStretch(1);

    root.resize(520, 300);  // 压低高度,模拟 demo 空间紧张触发父高度异步增长
    root.show();
    QVERIFY(QTest::qWaitForWindowExposed(&root));

    i1->setExpanded(true);
    QTest::qWait(600);

    auto* content1 = i1->findChild< QFrame* >(QStringLiteral("DACollapseItemContent"));
    QVERIFY(content1 != nullptr);
    const int stableH = content1->height();
    QVERIFY2(stableH > 0, "item1 content must have non-zero height after expand");

    HeightProbe probe;
    content1->installEventFilter(&probe);

    // 展开 item2:父布局高度异步增长期间,item1 content 不应被挤压
    i2->setExpanded(true);
    QTest::qWait(900);

    // 允许 2px 像素级容差,但不应有"收缩又伸长"的大幅波动
    for (int h : std::as_const(probe.heights)) {
        QVERIFY2(qAbs(h - stableH) <= 2,
                 qPrintable(QStringLiteral("item1 content flickered during sibling "
                                           "expand: %1 vs stable %2 (all: %3)")
                                .arg(h).arg(stableH)
                                .arg(QVariant::fromValue(probe.heights).toString())));
    }
}

void DACollapseTest::testDebugLoggingOnExpand()
{
    qputenv("DAWIDGETS_COLLAPSE_DEBUG", "1");

    DACollapseItem item(QStringLiteral("debug-item"));
    item.setAnimationEnabled(false);
    item.resize(240, 160);
    item.setContentWidget(new QLabel(QStringLiteral("debug content")));

    QTest::ignoreMessage(QtDebugMsg,
                         "[DEBUG-DACollapse] setExpanded title=\"debug-item\" name=\"\" from=false to=true");
    item.setExpanded(true);

    qunsetenv("DAWIDGETS_COLLAPSE_DEBUG");
}

QTEST_MAIN(DACollapseTest)
#include "tst_dacollapse.moc"



