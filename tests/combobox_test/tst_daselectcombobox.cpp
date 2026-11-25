#include <QApplication>
#include <QInputMethodEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QPushButton>
#include <QSignalSpy>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <QToolButton>
#include <QtTest/QtTest>

#include <DAChip.h>
#include <DASelectComboBox.h>

using namespace DA;

class DaSelectComboBoxTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testIsComboBox();
    void testFilterAndSelect();
    void testAllowCreate();
    void testHeightModeSwitch();
    void testArrowGeometry();
    void testImeEnterDoesNotPickCandidate();
    void testGroupedOptionsFlat();
    void testGroupHeadersInPopup();
    void testKeyboardSkipsGroupHeaders();
    void testFilterHidesEmptyGroups();
    void testEmptyStateVisibleOnce();
    void testEmptyStateTextOnlyDoesNotEnable();
    void testCreateButtonFromEmptyState();
    void testClearableSignalOnce();
    void testClearButtonPixels();
    void testCollapseTagsState();
    void testCollapseTagsPixels();
    void testEmptyWidgetLifecycle();
};

namespace {
QListWidget *popupOf(DA::DASelectComboBox &)
{
    const auto tops = QApplication::topLevelWidgets();
    for (QWidget *w : tops) {
        if (auto *lw = qobject_cast< QListWidget * >(w)) {
            if (lw->objectName() == QStringLiteral("DASelectPopup"))
                return lw;
        }
    }
    return nullptr;
}

QRect editFieldRect(QWidget *w)
{
    QStyleOptionComboBox opt;
    opt.initFrom(w);
    opt.rect = w->rect();
    opt.editable = true;
    return w->style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, w);
}

int countColumnRuns(const QPixmap &pix, const QRect &area, QRgb target)
{
    const QImage img = pix.toImage();
    int runs = 0;
    bool inRun = false;
    for (int x = area.left(); x <= area.right(); ++x) {
        bool hit = false;
        for (int y = area.top(); y <= area.bottom(); ++y) {
            if (img.pixel(x, y) == target) {
                hit = true;
                break;
            }
        }
        if (hit && !inRun) {
            ++runs;
            inRun = true;
        } else if (!hit) {
            inRun = false;
        }
    }
    return runs;
}

int countDarkPixels(const QPixmap &pix, const QRect &area, int threshold = 200)
{
    const QImage img = pix.toImage();
    int dark = 0;
    for (int y = area.top(); y <= area.bottom(); ++y) {
        for (int x = area.left(); x <= area.right(); ++x) {
            const QRgb px = img.pixel(x, y);
            if (qRed(px) < threshold || qGreen(px) < threshold || qBlue(px) < threshold)
                ++dark;
        }
    }
    return dark;
}

QLineEdit *inputOf(DA::DASelectComboBox &cb)
{
    return cb.findChild< QLineEdit * >(QStringLiteral("DAChipsEditInput"));
}
}  // namespace

static QStringList sl(std::initializer_list<const char *> items)
{
    QStringList out;
    for (const char *s : items)
        out << QString::fromLatin1(s);
    return out;
}

void DaSelectComboBoxTest::testIsComboBox()
{
    DASelectComboBox cb;
    QVERIFY(cb.metaObject()->inherits(&QComboBox::staticMetaObject));
    QVERIFY(cb.isEditable()); // 内部 editable，编辑区由 DAChipsEdit 接管
}

void DaSelectComboBoxTest::testFilterAndSelect()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "Qt", "C++", "Python", "QML", "Qt Quick", "Rust" }));
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));

    auto *input = cb.findChild<QLineEdit *>(QStringLiteral("DAChipsEditInput"));
    QVERIFY(input);
    input->setFocus();

    // 输入 "Q" 过滤出 Qt / QML / Qt Quick，回车选中第一项 Qt（default-first-option）
    QTest::keyClicks(input, QStringLiteral("Q"));
    QTest::qWait(60);
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(cb.tags(), sl({ "Qt" }));
}

void DaSelectComboBoxTest::testAllowCreate()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "Qt", "C++" }));
    cb.setAllowCreate(true);
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));

    auto *input = cb.findChild<QLineEdit *>(QStringLiteral("DAChipsEditInput"));
    QVERIFY(input);
    input->setFocus();

    // 输入不在列表中的 "Rust"，回车创建
    QTest::keyClicks(input, QStringLiteral("Rust"));
    QTest::qWait(60);
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(cb.tags(), sl({ "Rust" }));
}

void DaSelectComboBoxTest::testHeightModeSwitch()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "Qt", "C++", "Python" }));
    cb.setFixedWidth(100); // 强制窄宽度，确保标签换行
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));

    // GrowWithTags：空标签单行
    const int h0 = cb.sizeHint().height();

    // 多个长标签在窄宽度下必然换行 → 高度增长
    cb.setTags(sl({ "tag-01", "tag-02", "tag-03", "tag-04",
                    "tag-05", "tag-06", "tag-07", "tag-08" }));
    cb.updateGeometry();
    const int hGrow = cb.sizeHint().height();
    QVERIFY2(hGrow > h0,
             qPrintable(QStringLiteral("hGrow=%1 h0=%2").arg(hGrow).arg(h0)));

    // 切到 FixedHeightScrollable：高度回落到固定行数
    cb.setHeightMode(DASelectComboBox::HeightMode::FixedHeightScrollable);
    cb.setFixedRowCount(1);
    const int hFixed1 = cb.sizeHint().height();
    QVERIFY(hFixed1 < hGrow);

    // 增加标签，固定模式高度不变
    cb.setTags(sl({ "tag-01", "tag-02", "tag-03", "tag-04", "tag-05",
                    "tag-06", "tag-07", "tag-08", "tag-09", "tag-10" }));
    QCOMPARE(cb.sizeHint().height(), hFixed1);

    // 固定 2 行比 1 行高
    cb.setFixedRowCount(2);
    QVERIFY(cb.sizeHint().height() > hFixed1);
}

void DaSelectComboBoxTest::testArrowGeometry()
{
    // 诊断：对比 DASelectComboBox 与原生 editable QComboBox 的箭头子控件矩形。
    // 若 DASelectComboBox 箭头宽度为 0，则是几何问题；否则是渲染/qss 问题。
    QComboBox plain;
    plain.setEditable(true);
    plain.resize(200, plain.sizeHint().height());

    DASelectComboBox cb;
    cb.resize(200, cb.sizeHint().height());

    auto arrowRect = [](QComboBox *w) -> QRect {
        QStyleOptionComboBox opt;
        opt.initFrom(w);
        opt.rect = w->rect();
        opt.editable = true;
        return w->style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                          QStyle::SC_ComboBoxArrow, w);
    };

    const QRect plainArrow = arrowRect(&plain);
    const QRect cbArrow = arrowRect(&cb);
    qDebug() << "plain QComboBox arrow rect:" << plainArrow
             << "size=" << plain.size();
    qDebug() << "DASelectComboBox arrow rect:" << cbArrow
             << "size=" << cb.size();

    QVERIFY2(cbArrow.width() > 0,
             qPrintable(QStringLiteral("DASelectComboBox arrow width=%1 (plain=%2)")
                            .arg(cbArrow.width()).arg(plainArrow.width())));

    // 渲染验证：把 DASelectComboBox 渲染到 QPixmap，检查箭头区域是否有
    // 非背景像素（即箭头确实被绘制）。这能区分"几何正常但渲染缺失"。
    auto renderAndCountDark = [](QComboBox *w, const QRect &arrow) -> int {
        QPixmap pix(w->size());
        pix.fill(Qt::white);
        w->render(&pix);
        QImage img = pix.toImage();
        int dark = 0;
        for (int y = arrow.top(); y < arrow.bottom() && y < img.height(); ++y) {
            for (int x = arrow.left(); x < arrow.right() && x < img.width(); ++x) {
                const QRgb px = img.pixel(x, y);
                // 非白色像素（箭头通常是灰/黑色三角）
                if (qRed(px) < 240 || qGreen(px) < 240 || qBlue(px) < 240)
                    ++dark;
            }
        }
        return dark;
    };

    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    const int cbDark = renderAndCountDark(&cb, cbArrow);
    qDebug() << "DASelectComboBox arrow dark-pixel count (no qss):" << cbDark
             << "in rect" << cbArrow;

    QVERIFY2(cbDark > 5,
             "DASelectComboBox arrow region has no rendered pixels — arrow not drawn");

    // 验证组件自带默认样式（qrc SVG 箭头）下箭头渲染正常。
    // 构造函数已应用默认样式，不手动设 qss。渲染并打印 ASCII 网格确认形状。
    qApp->setStyleSheet(QString());
    // 验证 qrc 箭头 SVG 资源可加载
    {
        QPixmap ap(QStringLiteral(":/DAWidgetsIcons/arrow-down.svg"));
        qDebug() << "built-in arrow svg from qrc isNull=" << ap.isNull()
                 << "size=" << ap.size();
    }
    cb.ensurePolished();
    cb.update();
    QTest::qWait(50);
    const int cbDarkDefault = renderAndCountDark(&cb, cbArrow);
    {
        QPixmap pix(cb.size());
        pix.fill(Qt::white);
        cb.render(&pix);
        QImage img = pix.toImage().copy(cbArrow);
        QString grid;
        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                const QRgb px = img.pixel(x, y);
                grid += (qRed(px) < 200 || qGreen(px) < 200 || qBlue(px) < 200)
                            ? QStringLiteral("#") : QStringLiteral(".");
            }
            grid += QStringLiteral("\n");
        }
        qDebug() << "arrow ASCII grid under default style (18x31):\n" << grid;
    }
    qDebug() << "DASelectComboBox arrow dark-pixel count (default style):" << cbDarkDefault;
    QVERIFY2(cbDarkDefault > 5,
             "Arrow not drawn under default style — built-in PNG arrow missing");
}

void DaSelectComboBoxTest::testImeEnterDoesNotPickCandidate()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "Qt", "C++" }));
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));

    auto *input = cb.findChild<QLineEdit *>(QStringLiteral("DAChipsEditInput"));
    QVERIFY(input);
    input->setFocus();

    QSignalSpy selectedSpy(&cb, &DASelectComboBox::optionSelected);

    // 组字期间回车不得选中候选
    QInputMethodEvent preedit(QStringLiteral("Q"), QList<QInputMethodEvent::Attribute>());
    QCoreApplication::sendEvent(input, &preedit);
    QTest::keyClick(input, Qt::Key_Return);
    QVERIFY(cb.tags().isEmpty());
    QCOMPARE(selectedSpy.count(), 0);

    // 组字结束（commit）后回车恢复"选中第一项"
    QInputMethodEvent commit;
    commit.setCommitString(QStringLiteral("Q"));
    QCoreApplication::sendEvent(input, &commit);
    QTest::qWait(60);
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(cb.tags(), sl({ "Qt" }));
}

void DaSelectComboBoxTest::testGroupedOptionsFlat()
{
    DASelectComboBox cb;
    cb.setGroupedOptions({ DASelectGroup{ QStringLiteral("G1"), { QStringLiteral("a1"), QStringLiteral("a2") } },
                           DASelectGroup{ QStringLiteral("G2"), { QStringLiteral("b1") } } });
    QCOMPARE(cb.options(), sl({ "a1", "a2", "b1" }));
    QCOMPARE(cb.groupedOptions().size(), 2);
    QCOMPARE(cb.groupedOptions().at(0).title, QStringLiteral("G1"));

    cb.setOptions(sl({ "x", "y" }));
    QVERIFY(cb.groupedOptions().isEmpty());
    QCOMPARE(cb.options(), sl({ "x", "y" }));
}

void DaSelectComboBoxTest::testGroupHeadersInPopup()
{
    DASelectComboBox cb;
    cb.setGroupedOptions({ DASelectGroup{ QStringLiteral("G1"), { QStringLiteral("a1"), QStringLiteral("a2") } },
                           DASelectGroup{ QString(), { QStringLiteral("b1") } } });
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));

    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(50);

    auto *popup = popupOf(cb);
    QVERIFY(popup);
    QCOMPARE(popup->count(), 4);  // G1, a1, a2, b1 —— 空标题组不产生标题行
    QCOMPARE(popup->item(0)->text(), QStringLiteral("G1"));
    QVERIFY(!(popup->item(0)->flags() & Qt::ItemIsSelectable));
    QCOMPARE(popup->item(1)->text(), QStringLiteral("a1"));
    QCOMPARE(popup->item(3)->text(), QStringLiteral("b1"));
}

void DaSelectComboBoxTest::testKeyboardSkipsGroupHeaders()
{
    DASelectComboBox cb;
    cb.setGroupedOptions({ DASelectGroup{ QStringLiteral("G1"), { QStringLiteral("a1"), QStringLiteral("a2") } } });
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(50);

    auto *popup = popupOf(cb);
    QVERIFY(popup);
    QVERIFY(popup->currentItem());
    QCOMPARE(popup->currentItem()->text(), QStringLiteral("a1"));  // default-first-option 跳过标题

    QTest::keyClick(input, Qt::Key_Down);
    QCOMPARE(popup->currentItem()->text(), QStringLiteral("a2"));

    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(cb.tags(), sl({ "a2" }));
    QVERIFY(!cb.tags().contains(QStringLiteral("G1")));
}

void DaSelectComboBoxTest::testFilterHidesEmptyGroups()
{
    DASelectComboBox cb;
    cb.setGroupedOptions({ DASelectGroup{ QStringLiteral("G1"), { QStringLiteral("a1"), QStringLiteral("a2") } },
                           DASelectGroup{ QStringLiteral("G2"), { QStringLiteral("b1") } } });
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(50);

    QTest::keyClicks(input, QStringLiteral("b"));
    QTest::qWait(40);

    auto *popup = popupOf(cb);
    QVERIFY(popup);
    QStringList texts;
    for (int i = 0; i < popup->count(); ++i)
        texts << popup->item(i)->text();
    QCOMPARE(texts, sl({ "G2", "b1" }));
}

void DaSelectComboBoxTest::testEmptyStateVisibleOnce()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "a1" }));
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(30);

    auto *popup = popupOf(cb);
    QVERIFY(popup);

    // 默认：无匹配时弹层关闭
    QTest::keyClicks(input, QStringLiteral("zzz"));
    QTest::qWait(30);
    QVERIFY(!popup->isVisible());

    // 打开空态后无匹配也保持弹层，且不插占位 item
    cb.setNoMatchText(QStringLiteral("没有匹配项"));
    cb.setEmptyStateEnabled(true);
    QTest::keyClicks(input, QStringLiteral("z"));
    QTest::qWait(30);
    QVERIFY(popup->isVisible());
    QCOMPARE(popup->count(), 0);

    const auto labels = popup->findChildren< QLabel * >(QStringLiteral("DASelectEmptyText"));
    QCOMPARE(labels.size(), 1);
    QCOMPARE(labels.first()->text(), QStringLiteral("没有匹配项"));
    QVERIFY(!labels.first()->parentWidget()->isHidden());
}

void DaSelectComboBoxTest::testEmptyStateTextOnlyDoesNotEnable()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "a1" }));
    cb.setNoMatchText(QStringLiteral("x"));
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(30);

    auto *popup = popupOf(cb);
    QVERIFY(popup);
    QTest::keyClicks(input, QStringLiteral("zzz"));
    QTest::qWait(30);
    QVERIFY(!popup->isVisible());
}

void DaSelectComboBoxTest::testCreateButtonFromEmptyState()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "a1" }));
    cb.setAllowCreate(true);
    cb.setEmptyStateEnabled(true);
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(30);

    QTest::keyClicks(input, QStringLiteral("zzz"));
    QTest::qWait(30);

    auto *popup = popupOf(cb);
    QVERIFY(popup);
    auto *create = popup->findChild< QPushButton * >(QStringLiteral("DASelectCreateButton"));
    QVERIFY(create);
    QVERIFY(create->isVisible());
    QCOMPARE(create->text(), QStringLiteral("Create \"zzz\""));

    QSignalSpy createdSpy(&cb, &DASelectComboBox::created);
    QSignalSpy selectedSpy(&cb, &DASelectComboBox::optionSelected);
    create->click();
    QCOMPARE(cb.tags(), sl({ "zzz" }));
    QCOMPARE(createdSpy.count(), 1);
    QCOMPARE(selectedSpy.count(), 0);

    // 回车路径与按钮结果一致
    DASelectComboBox cb2;
    cb2.setOptions(sl({ "a1" }));
    cb2.setAllowCreate(true);
    cb2.setEmptyStateEnabled(true);
    cb2.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb2));
    auto *input2 = inputOf(cb2);
    QVERIFY(input2);
    input2->setFocus();
    QTest::keyClicks(input2, QStringLiteral("yyy"));
    QTest::qWait(30);
    QSignalSpy created2(&cb2, &DASelectComboBox::created);
    QTest::keyClick(input2, Qt::Key_Return);
    QCOMPARE(cb2.tags(), sl({ "yyy" }));
    QCOMPARE(created2.count(), 1);
}

void DaSelectComboBoxTest::testClearableSignalOnce()
{
    DASelectComboBox cb;
    cb.setClearable(true);
    cb.setOptions(sl({ "a", "b", "c" }));
    cb.setTags(sl({ "a", "b" }));
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(40);

    auto *popup = popupOf(cb);
    QVERIFY(popup);
    QVERIFY(popup->isVisible());

    auto *button = cb.findChild< QToolButton * >(QStringLiteral("DASelectClearButton"));
    QVERIFY(button);
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(&cb, &enter);
    QVERIFY(button->isVisible());

    QSignalSpy tagsSpy(&cb, &DASelectComboBox::tagsChanged);
    QSignalSpy clearSpy(&cb, &DASelectComboBox::clearClicked);
    button->click();

    QCOMPARE(tagsSpy.count(), 1);
    QCOMPARE(clearSpy.count(), 1);
    QVERIFY(cb.tags().isEmpty());
    QVERIFY(popup->isVisible());  // 清空后弹层保持打开
}

void DaSelectComboBoxTest::testClearButtonPixels()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "a" }));
    cb.setTags(sl({ "a" }));
    cb.resize(300, cb.sizeHint().height());
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    QTest::qWait(30);

    const QRect field = editFieldRect(&cb);
    const QRect region(field.right() - 24, field.top(), 24, field.height());

    auto darkInRegion = [&]() {
        QPixmap pix(cb.size());
        pix.fill(Qt::white);
        cb.render(&pix);
        return countDarkPixels(pix, region);
    };

    QCOMPARE(darkInRegion(), 0);  // 未开 clearable：× 区域近白

    cb.setClearable(true);
    // 离屏平台的光标可能恰好落在控件上导致自动 Enter，先显式 Leave 保证初始隐藏
    QEvent leaveFirst(QEvent::Leave);
    QApplication::sendEvent(&cb, &leaveFirst);
    QTest::qWait(20);
    QCOMPARE(darkInRegion(), 0);  // 有标签但未悬停：按钮仍隐藏

    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(&cb, &enter);
    QTest::qWait(20);
    QVERIFY2(darkInRegion() > 0, "clear button must be painted on hover");

    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(&cb, &leave);
    QTest::qWait(20);
    QCOMPARE(darkInRegion(), 0);
}

void DaSelectComboBoxTest::testCollapseTagsState()
{
    DASelectComboBox cb;
    cb.setCollapseTags(true);
    cb.setCollapseTagCount(2);
    cb.setOptions(sl({ "a", "b", "c", "d", "e" }));
    cb.setTags(sl({ "a", "b", "c", "d", "e" }));
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    QTest::qWait(40);

    // 窗口激活可能把焦点给输入框；折叠只在未聚焦时生效
    inputOf(cb)->clearFocus();
    QTest::qWait(30);

    auto visibleChips = [&cb]() {
        int n = 0;
        for (auto *chip : cb.findChildren< DAChip * >()) {
            if (chip->isVisible())
                ++n;
        }
        return n;
    };

    QCOMPARE(visibleChips(), 2);
    auto *summary = cb.findChild< QLabel * >(QStringLiteral("DASelectTagSummary"));
    QVERIFY(summary);
    QVERIFY(summary->isVisible());
    QCOMPARE(summary->text(), QStringLiteral("+3"));
    QCOMPARE(cb.tags().size(), 5);

    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(50);
    QCOMPARE(visibleChips(), 5);
    QVERIFY(!summary->isVisible());
}

void DaSelectComboBoxTest::testCollapseTagsPixels()
{
    DASelectComboBox cb;
    cb.setCollapseTags(true);
    cb.setCollapseTagCount(2);
    cb.setOptions(sl({ "a", "b", "c", "d", "e" }));
    cb.setTags(sl({ "a", "b", "c", "d", "e" }));
    cb.setStyleSheet(cb.styleSheet()
                     + QStringLiteral("DA--DAChip { background:#ecf5ff; border:0; }"));
    cb.resize(420, cb.sizeHint().height());
    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    QTest::qWait(50);
    inputOf(cb)->clearFocus();
    QTest::qWait(30);

    const QRect field = editFieldRect(&cb);
    const QRgb target  = 0xFFECF5FF;

    auto chipRuns = [&]() {
        return countColumnRuns(cb.grab(), field, target);
    };

    QCOMPARE(chipRuns(), 2);

    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::qWait(60);
    QCOMPARE(chipRuns(), 5);
}

void DaSelectComboBoxTest::testEmptyWidgetLifecycle()
{
    DASelectComboBox cb;
    cb.setOptions(sl({ "a1" }));
    cb.setEmptyStateEnabled(true);
    auto *label = new QLabel(QStringLiteral("custom empty"));
    label->setAlignment(Qt::AlignCenter);
    QPointer< QLabel > guard(label);
    cb.setEmptyWidget(label);
    QVERIFY(cb.emptyWidget() == label);

    cb.show();
    QVERIFY(QTest::qWaitForWindowExposed(&cb));
    auto *input = inputOf(cb);
    QVERIFY(input);
    input->setFocus();
    QTest::keyClicks(input, QStringLiteral("zzz"));
    QTest::qWait(40);

    auto *popup = popupOf(cb);
    QVERIFY(popup);
    QVERIFY(guard);
    QVERIFY(guard->isVisible());
    QCOMPARE(guard->parentWidget(), popup);
    QCOMPARE(guard->geometry(), popup->viewport()->geometry());

    // 多次过滤刷新（每次 popup->clear()）后控件仍存活
    QTest::keyClicks(input, QStringLiteral("z"));
    QTest::qWait(20);
    QTest::keyClicks(input, QStringLiteral("z"));
    QTest::qWait(20);
    QVERIFY(guard);
    QVERIFY(guard->isVisible());

    auto *panel = popup->findChild< QWidget * >(QStringLiteral("DASelectEmptyPanel"));
    QVERIFY(panel);
    QVERIFY(!panel->isVisible());
}

QTEST_MAIN(DaSelectComboBoxTest)
#include "tst_daselectcombobox.moc"