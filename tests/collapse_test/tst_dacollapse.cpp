#include <QtTest>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QResizeEvent>
#include <QSignalSpy>
#include <QVBoxLayout>

#include "DACollapseWidget.h"
#include "DACollapseItem.h"

using namespace DA;

/// @brief DACollapse / DACollapseItem 行为测试,对标 Element UI Collapse 语义
class DACollapseTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
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
    void testMixedWidgets();
    void testKeyboardNavigationDisabledByDefault();
    void testKeyboardNavigationFocusOrder();
    void testKeyboardNavigationNoWrap();
    void testKeyboardNavigationSkipsDisabled();
    void testKeyboardNavigationEnterAndSpaceToggle();
    void testKeyboardNavigationKeepsAccordion();
    void testKeyboardNavigationOffRestoresFocusPolicy();
    void testExpandRequestedOnceBeforeAnimation();
    void testExpandRequestedSkippedWhenContentPresent();
    void testExpandRequestedFromSetActiveNames();
    void testRefreshContentHeightAfterAsyncLoad();
    void testVariantDoesNotChangeActiveNames();
    void testVariantCardSpacing();
    void testVariantBorderlessCompactPadding();
    void testVariantPropertyForQss();
    void testFocusRingPixels();
};

namespace {
/// 显示并激活窗口,让焦点断言在离屏/Windows 平台都成立
void showAndActivate(QWidget* w, const QSize& size = QSize(360, 320))
{
    w->resize(size);
    w->show();
    QTest::qWaitForWindowExposed(w);
    w->activateWindow();
    QTest::qWaitForWindowActive(w);
    QTest::qWait(20);
}

QWidget* headerOf(DACollapseItem* item)
{
    return item ? item->headerWidget() : nullptr;
}
}  // namespace

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
    DACollapseWidget c;
    c.addItem(QStringLiteral("a"), QStringLiteral("1"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("b"), QStringLiteral("2"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("c"), QStringLiteral("3"))->setAnimationEnabled(false);
    QSignalSpy spy(&c, &DACollapseWidget::activeNamesChanged);

    c.setActiveNames({ QStringLiteral("2"), QStringLiteral("3") });
    QCOMPARE(c.activeNames(), QStringList({ QStringLiteral("2"), QStringLiteral("3") }));
    QVERIFY(c.item(QStringLiteral("2"))->expanded());
    QVERIFY(c.item(QStringLiteral("3"))->expanded());
    QVERIFY(!c.item(QStringLiteral("1"))->expanded());
    QCOMPARE(spy.count(), 1);
}

void DACollapseTest::testAccordionMutualExclusion()
{
    DACollapseWidget c;
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
    DACollapseWidget c;
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
    DACollapseWidget c;
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
    DACollapseWidget c;
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

    auto* c = new DA::DACollapseWidget(&root);
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

/**
 * @brief 验证 DACollapseWidget 与普通 QWidget 混排
 * @details insertWidget/addWidget/removeWidget/indexOf 应正确管理普通控件,
 *          且普通控件不进入 items 列表,不参与 activeNames/accordion 协调。
 *          混排顺序应保持:widget1, item1, widget2, item2, widget3(弹簧在末尾)。
 */
void DACollapseTest::testMixedWidgets()
{
    DACollapseWidget c;
    c.addItem(QStringLiteral("g1"), QStringLiteral("1"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("g2"), QStringLiteral("2"))->setAnimationEnabled(false);

    auto* w1 = new QLabel(QStringLiteral("w1"));
    auto* w2 = new QLabel(QStringLiteral("w2"));
    auto* w3 = new QLabel(QStringLiteral("w3"));

    // 在头部插入 w1
    c.insertWidget(0, w1);
    // 在末尾(弹簧前)追加 w3
    c.addWidget(w3);
    // 在 item1 之后插入 w2:item1 位于索引 1(w1=0, item1=1),w2 应插入到索引 2
    c.insertWidget(2, w2);

    // 布局顺序: w1(0), item1(1), w2(2), item2(3), w3(4), stretch(5)
    QCOMPARE(c.indexOf(w1), 0);
    QCOMPARE(c.indexOf(w2), 2);
    QCOMPARE(c.indexOf(w3), 4);
    // 末尾弹簧不计入 items
    QCOMPARE(c.items().size(), 2);

    // 普通控件不参与 activeNames
    c.setActiveNames({ QStringLiteral("1") });
    QVERIFY(c.item(QStringLiteral("1"))->expanded());
    QVERIFY(!c.item(QStringLiteral("2"))->expanded());
    QCOMPARE(c.activeNames(), QStringList{ QStringLiteral("1") });

    // removeWidget 不销毁控件,且不影响 items
    c.removeWidget(w2);
    QCOMPARE(c.indexOf(w2), -1);
    QCOMPARE(c.items().size(), 2);
    QCOMPARE(c.indexOf(w1), 0);
    QCOMPARE(c.indexOf(w3), 3);  // w2 移除后 w3 索引前移

    // removeWidget 不接管所有权,手动删除避免泄漏
    delete w2;

    // clear 只清 DACollapseItem,普通控件需调用方自行管理
    auto* w1Parent = w1->parentWidget();
    c.clear();
    QCOMPARE(c.items().size(), 0);
    // w1/w3 仍存在(作为 DACollapseWidget 的子控件,parent 未变)
    QCOMPARE(w1->parentWidget(), w1Parent);
    QCOMPARE(w3->parentWidget(), w1Parent);

    delete w1;
    delete w3;
}

void DACollapseTest::testKeyboardNavigationDisabledByDefault()
{
    DACollapseWidget c;
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    c.addItem(QStringLiteral("b"), QStringLiteral("2"));

    QVERIFY(!c.isKeyboardNavigation());
    for (auto* it : c.items())
        QCOMPARE(it->headerWidget()->focusPolicy(), Qt::NoFocus);

    // NoFocus 下主动 setFocus 也拿不到焦点
    headerOf(i1)->setFocus();
    QVERIFY(!headerOf(i1)->hasFocus());
}

void DACollapseTest::testKeyboardNavigationFocusOrder()
{
    DACollapseWidget c;
    c.setKeyboardNavigation(true);
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    auto* i3 = c.addItem(QStringLiteral("c"), QStringLiteral("3"));
    showAndActivate(&c);

    QWidget* h1 = headerOf(i1);
    QWidget* h2 = headerOf(i2);
    QWidget* h3 = headerOf(i3);
    QCOMPARE(h1->focusPolicy(), Qt::StrongFocus);

    h1->setFocus(Qt::TabFocusReason);
    QVERIFY(h1->hasFocus());

    QTest::keyClick(h1, Qt::Key_Down);
    QVERIFY(h2->hasFocus());

    QTest::keyClick(h2, Qt::Key_Up);
    QVERIFY(h1->hasFocus());

    // 依次 Tab 能停在每个标题上
    h2->setFocus(Qt::TabFocusReason);
    QVERIFY(h2->hasFocus());
    h3->setFocus(Qt::TabFocusReason);
    QVERIFY(h3->hasFocus());
}

void DACollapseTest::testKeyboardNavigationNoWrap()
{
    DACollapseWidget c;
    c.setKeyboardNavigation(true);
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    showAndActivate(&c);

    QWidget* h1 = headerOf(i1);
    QWidget* h2 = headerOf(i2);
    h2->setFocus(Qt::TabFocusReason);
    QVERIFY(h2->hasFocus());
    QTest::keyClick(h2, Qt::Key_Down);
    QVERIFY(h2->hasFocus());

    h1->setFocus(Qt::TabFocusReason);
    QTest::keyClick(h1, Qt::Key_Up);
    QVERIFY(h1->hasFocus());
}

void DACollapseTest::testKeyboardNavigationSkipsDisabled()
{
    DACollapseWidget c;
    c.setKeyboardNavigation(true);
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    auto* i3 = c.addItem(QStringLiteral("c"), QStringLiteral("3"));
    i2->setDisabled(true);
    showAndActivate(&c);

    QWidget* h1 = headerOf(i1);
    h1->setFocus(Qt::TabFocusReason);
    QVERIFY(h1->hasFocus());
    QTest::keyClick(h1, Qt::Key_Down);
    QVERIFY(headerOf(i3)->hasFocus());
    QVERIFY(!headerOf(i2)->hasFocus());
}

void DACollapseTest::testKeyboardNavigationEnterAndSpaceToggle()
{
    DACollapseWidget c;
    c.setKeyboardNavigation(true);
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    i1->setAnimationEnabled(false);
    showAndActivate(&c);

    QSignalSpy spy(i1, &DACollapseItem::expandChanged);
    QWidget* h1 = headerOf(i1);
    h1->setFocus(Qt::TabFocusReason);
    QVERIFY(h1->hasFocus());

    QTest::keyClick(h1, Qt::Key_Return);
    QVERIFY(i1->expanded());
    QCOMPARE(spy.count(), 1);

    QTest::keyClick(h1, Qt::Key_Space);
    QVERIFY(!i1->expanded());
    QCOMPARE(spy.count(), 2);
}

void DACollapseTest::testKeyboardNavigationKeepsAccordion()
{
    DACollapseWidget c;
    c.setAccordion(true);
    c.setKeyboardNavigation(true);
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    i1->setAnimationEnabled(false);
    i2->setAnimationEnabled(false);
    showAndActivate(&c);

    QWidget* h2 = headerOf(i2);
    h2->setFocus(Qt::TabFocusReason);
    QTest::keyClick(h2, Qt::Key_Return);
    QVERIFY(i2->expanded());
    QVERIFY(!i1->expanded());
    QCOMPARE(c.activeNames(), QStringList{ QStringLiteral("2") });
}

void DACollapseTest::testKeyboardNavigationOffRestoresFocusPolicy()
{
    DACollapseWidget c;
    c.setKeyboardNavigation(true);
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    showAndActivate(&c);

    QWidget* h1 = headerOf(i1);
    h1->setFocus(Qt::TabFocusReason);
    QVERIFY(h1->hasFocus());

    c.setKeyboardNavigation(false);
    QCOMPARE(h1->focusPolicy(), Qt::NoFocus);
    QCOMPARE(headerOf(i2)->focusPolicy(), Qt::NoFocus);
    QVERIFY(!h1->hasFocus());

    // 关闭后方向键不再移动焦点
    QTest::keyClick(h1, Qt::Key_Down);
    QVERIFY(!headerOf(i2)->hasFocus());
    QVERIFY(!h1->hasFocus());
}

void DACollapseTest::testExpandRequestedOnceBeforeAnimation()
{
    DACollapseWidget c;
    auto* item = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    QSignalSpy contentSpy(item, &DACollapseItem::contentRequested);
    QSignalSpy requestSpy(&c, &DACollapseWidget::expandRequested);

    int heightInSlot = -1;
    QObject::connect(&c, &DACollapseWidget::expandRequested, [&](const QString& name) {
        auto* content = c.item(name)->findChild< QFrame* >(QStringLiteral("DACollapseItemContent"));
        if (content)
            heightInSlot = content->height();
    });

    item->setExpanded(true);
    QCOMPARE(contentSpy.count(), 1);
    QCOMPARE(requestSpy.count(), 1);
    QCOMPARE(requestSpy.at(0).at(0).toString(), QStringLiteral("1"));
    // 信号在动画开始前发出,此时内容区尚未展开
    QCOMPARE(heightInSlot, 0);

    // 同一面板再次展开不应重复请求
    item->setExpanded(false);
    item->setExpanded(true);
    QCOMPARE(contentSpy.count(), 1);
    QCOMPARE(requestSpy.count(), 1);

    // 宿主在槽里同步提供内容时,展开高度直接等于自然高度
    DACollapseWidget c2;
    auto* item2 = c2.addItem(QStringLiteral("b"), QStringLiteral("2"));
    item2->setAnimationEnabled(false);
    QObject::connect(&c2, &DACollapseWidget::expandRequested, [item2](const QString&) {
        auto* lbl = new QLabel(QStringLiteral("synchronous content"));
        lbl->setMinimumHeight(48);
        item2->setContentWidget(lbl);
    });
    item2->setExpanded(true);
    auto* content2 = item2->findChild< QFrame* >(QStringLiteral("DACollapseItemContent"));
    QVERIFY(content2);
    QVERIFY2(content2->maximumHeight() >= 48,
             qPrintable(QStringLiteral("maxHeight=%1").arg(content2->maximumHeight())));
}

void DACollapseTest::testExpandRequestedSkippedWhenContentPresent()
{
    DACollapseWidget c;
    auto* item = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    item->setContentWidget(new QLabel(QStringLiteral("already here")));
    item->setAnimationEnabled(false);
    QSignalSpy contentSpy(item, &DACollapseItem::contentRequested);
    QSignalSpy requestSpy(&c, &DACollapseWidget::expandRequested);

    item->setExpanded(true);
    QCOMPARE(contentSpy.count(), 0);
    QCOMPARE(requestSpy.count(), 0);
}

void DACollapseTest::testExpandRequestedFromSetActiveNames()
{
    DACollapseWidget c;
    c.addItem(QStringLiteral("a"), QStringLiteral("1"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("b"), QStringLiteral("2"))->setAnimationEnabled(false);
    QSignalSpy requestSpy(&c, &DACollapseWidget::expandRequested);

    c.setActiveNames({ QStringLiteral("2") });
    QCOMPARE(requestSpy.count(), 1);
    QCOMPARE(requestSpy.at(0).at(0).toString(), QStringLiteral("2"));
}

void DACollapseTest::testRefreshContentHeightAfterAsyncLoad()
{
    DACollapseWidget c;
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    i1->setAnimationEnabled(false);
    i2->setAnimationEnabled(false);
    showAndActivate(&c);

    i1->setExpanded(true);
    QTest::qWait(30);
    auto* content = i1->findChild< QFrame* >(QStringLiteral("DACollapseItemContent"));
    QVERIFY(content);
    const int emptyHeight = content->maximumHeight();

    auto* lbl = new QLabel(QStringLiteral("async line 1\nasync line 2\nasync line 3"));
    lbl->setMinimumHeight(64);
    i1->setContentWidget(lbl);
    i1->refreshContentHeight();

    QVERIFY2(content->maximumHeight() >= 64 && content->maximumHeight() > emptyHeight,
             qPrintable(QStringLiteral("maxHeight=%1 empty=%2")
                            .arg(content->maximumHeight())
                            .arg(emptyHeight)));

    // 折叠态调用 refreshContentHeight 必须早退,不能把 maximumHeight 改掉
    auto* content2 = i2->findChild< QFrame* >(QStringLiteral("DACollapseItemContent"));
    QVERIFY(content2);
    i2->refreshContentHeight();
    QCOMPARE(content2->maximumHeight(), 0);
}

void DACollapseTest::testVariantDoesNotChangeActiveNames()
{
    DACollapseWidget c;
    c.addItem(QStringLiteral("a"), QStringLiteral("1"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("b"), QStringLiteral("2"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("c"), QStringLiteral("3"))->setAnimationEnabled(false);
    c.setActiveNames({ QStringLiteral("1"), QStringLiteral("3") });

    QSignalSpy spy(&c, &DACollapseWidget::activeNamesChanged);
    c.setVariant(DACollapseWidget::Variant::Borderless);
    c.setVariant(DACollapseWidget::Variant::Card);
    c.setVariant(DACollapseWidget::Variant::Default);

    QCOMPARE(c.activeNames(), (QStringList{ QStringLiteral("1"), QStringLiteral("3") }));
    QCOMPARE(spy.count(), 0);
    QVERIFY(c.item(QStringLiteral("1"))->expanded());
    QVERIFY(!c.item(QStringLiteral("2"))->expanded());
    QVERIFY(c.item(QStringLiteral("3"))->expanded());
}

void DACollapseTest::testVariantCardSpacing()
{
    DACollapseWidget c;
    c.addItem(QStringLiteral("a"), QStringLiteral("1"))->setAnimationEnabled(false);
    c.addItem(QStringLiteral("b"), QStringLiteral("2"))->setAnimationEnabled(false);
    showAndActivate(&c, QSize(360, 400));

    auto gap = [&c]() {
        c.layout()->activate();
        QCoreApplication::processEvents();
        const auto items = c.items();
        return items.at(1)->geometry().top() - items.at(0)->geometry().bottom() - 1;
    };

    QCOMPARE(gap(), 0);
    c.setVariant(DACollapseWidget::Variant::Card);
    QCOMPARE(gap(), 8);
    c.setVariant(DACollapseWidget::Variant::Default);
    QCOMPARE(gap(), 0);
}

void DACollapseTest::testVariantBorderlessCompactPadding()
{
    DACollapseWidget c;
    auto* item = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    item->setAnimationEnabled(false);
    showAndActivate(&c);

    auto* arrow = item->findChild< QLabel* >(QStringLiteral("DACollapseItemArrow"));
    QVERIFY(arrow);

    auto settle = [&]() {
        c.layout()->activate();
        if (auto* hl = item->headerWidget()->layout())
            hl->activate();
        QCoreApplication::processEvents();
    };

    settle();
    QCOMPARE(arrow->x(), 12);
    QCOMPARE(item->contentLayout()->contentsMargins().left(), 12);

    c.setVariant(DACollapseWidget::Variant::Borderless);
    settle();
    QCOMPARE(arrow->x(), 0);
    QCOMPARE(item->contentLayout()->contentsMargins().left(), 0);

    c.setVariant(DACollapseWidget::Variant::Default);
    settle();
    QCOMPARE(arrow->x(), 12);
    QCOMPARE(item->contentLayout()->contentsMargins().left(), 12);
}

void DACollapseTest::testVariantPropertyForQss()
{
    DACollapseWidget c;
    auto* i1 = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    auto* i2 = c.addItem(QStringLiteral("b"), QStringLiteral("2"));
    QCOMPARE(i1->property("daVariant").toString(), QStringLiteral("default"));

    c.setVariant(DACollapseWidget::Variant::Card);
    QCOMPARE(c.property("daVariant").toString(), QStringLiteral("card"));
    QCOMPARE(i1->property("daVariant").toString(), QStringLiteral("card"));
    QCOMPARE(i2->property("daVariant").toString(), QStringLiteral("card"));

    auto* i3 = c.addItem(QStringLiteral("c"), QStringLiteral("3"));
    QCOMPARE(i3->property("daVariant").toString(), QStringLiteral("card"));
}

void DACollapseTest::testFocusRingPixels()
{
    DACollapseWidget c;
    c.setKeyboardNavigation(true);
    auto* item = c.addItem(QStringLiteral("a"), QStringLiteral("1"));
    item->setAnimationEnabled(false);
    showAndActivate(&c, QSize(360, 200));

    QWidget* header = headerOf(item);
    QVERIFY(header);
    // 窗口激活时 Qt 可能已把焦点给第一个可聚焦控件,先确保失焦再取基准图
    header->clearFocus();
    QTest::qWait(20);
    QVERIFY(!header->hasFocus());
    const QImage unfocused = header->grab().toImage();

    header->setFocus(Qt::TabFocusReason);
    QVERIFY(header->hasFocus());
    QTest::qWait(30);
    const QImage focused = header->grab().toImage();

    QCOMPARE(focused.size(), unfocused.size());
    int diff = 0;
    int nearEdge = 0;
    for (int y = 0; y < focused.height(); ++y) {
        for (int x = 0; x < focused.width(); ++x) {
            if (focused.pixel(x, y) == unfocused.pixel(x, y))
                continue;
            ++diff;
            if (x <= 5 || y <= 5 || x >= focused.width() - 6 || y >= focused.height() - 6)
                ++nearEdge;
        }
    }
    QVERIFY2(diff > 40, qPrintable(QStringLiteral("focus ring diff pixels=%1").arg(diff)));
    QVERIFY2(nearEdge > 40,
             qPrintable(QStringLiteral("focus ring pixels near edge=%1 of %2").arg(nearEdge).arg(diff)));
}

QTEST_MAIN(DACollapseTest)
#include "tst_dacollapse.moc"


