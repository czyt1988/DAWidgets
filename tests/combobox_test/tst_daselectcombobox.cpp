#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <QtTest/QtTest>

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
};

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

QTEST_MAIN(DaSelectComboBoxTest)
#include "tst_daselectcombobox.moc"
