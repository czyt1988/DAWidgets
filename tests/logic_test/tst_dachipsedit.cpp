#include <QFontMetrics>
#include <QInputMethodEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QSignalSpy>
#include <QtTest/QtTest>

#include <DAChip.h>
#include <DAChipsEdit.h>

using namespace DA;

class DaChipsEditTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDedupAndMaxCount();
    void testInsertRemove();
    void testKeyboardCommit();
    void testBackspaceRemove();
    void testPasteSplitDisabledByDefault();
    void testPasteSplitMultiSeparators();
    void testPasteSplitTrimsAndDropsEmpty();
    void testPasteSplitRespectsDedupAndMaxCount();
    void testPasteSplitEmitsOncePerPaste();
    void testPasteSplitIgnoredWhenAllowCreateFalse();
    void testImeComposingEnterDoesNotCommit();
    void testTagElideWidthAndTooltip();
    void testShowCountTextAndGeometry();
    void testShowCountWithoutMaxCount();
};

static QStringList sl(std::initializer_list<const char *> items)
{
    QStringList out;
    for (const char *s : items)
        out << QString::fromLatin1(s);
    return out;
}

void DaChipsEditTest::testDedupAndMaxCount()
{
    DAChipsEdit e;
    e.setAllowDuplicate(false);
    e.setTags({ QStringLiteral("a"), QStringLiteral("a"), QStringLiteral("b"),
                QStringLiteral("  a  "), QString(), QStringLiteral("b") });
    QCOMPARE(e.tags(), sl({ "a", "b" }));

    e.setAllowDuplicate(true);
    e.setTags({ QStringLiteral("a"), QStringLiteral("a") });
    QCOMPARE(e.tags(), sl({ "a", "a" }));

    e.setMaxCount(2);
    e.clear();
    e.appendTag(QStringLiteral("x"));
    e.appendTag(QStringLiteral("y"));
    e.appendTag(QStringLiteral("z"));
    QCOMPARE(e.tags(), sl({ "x", "y" }));

    e.clear();
    QVERIFY(e.tags().isEmpty());
}

void DaChipsEditTest::testInsertRemove()
{
    DAChipsEdit e;
    e.setTags({ QStringLiteral("a"), QStringLiteral("c") });
    QVERIFY(e.insertTag(1, QStringLiteral("b")));
    QCOMPARE(e.tags(), sl({ "a", "b", "c" }));

    QVERIFY(!e.insertTag(0, QStringLiteral("a"))); // 去重拒绝
    e.removeAt(1);
    QCOMPARE(e.tags(), sl({ "a", "c" }));
    e.removeTag(QStringLiteral("a"));
    QCOMPARE(e.tags(), sl({ "c" }));
}

void DaChipsEditTest::testKeyboardCommit()
{
    DAChipsEdit e;
    e.setSeparators(QList<DAChipsEdit::Separator>{
        DAChipsEdit::Separator::Comma,
        DAChipsEdit::Separator::Enter,
    });
    e.show();
    QVERIFY(QTest::qWaitForWindowExposed(&e));

    auto *input = e.findChild<QLineEdit *>(QStringLiteral("DAChipsEditInput"));
    QVERIFY(input);
    input->setFocus();

    QTest::keyClicks(input, QStringLiteral("Qt"));
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(e.tags(), sl({ "Qt" }));

    // 逗号触发即时提交 C++，"Go" 暂留输入框未提交
    QTest::keyClicks(input, QStringLiteral("C++,Go"));
    QCOMPARE(e.tags(), sl({ "Qt", "C++" }));
    QCOMPARE(input->text(), QStringLiteral("Go"));

    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(e.tags(), sl({ "Qt", "C++", "Go" }));
}

void DaChipsEditTest::testBackspaceRemove()
{
    DAChipsEdit e;
    e.setTags({ QStringLiteral("a"), QStringLiteral("b") });
    e.show();
    QVERIFY(QTest::qWaitForWindowExposed(&e));

    auto *input = e.findChild<QLineEdit *>(QStringLiteral("DAChipsEditInput"));
    input->setFocus();
    QVERIFY(input->text().isEmpty());

    QTest::keyClick(input, Qt::Key_Backspace);
    QCOMPARE(e.tags(), sl({ "a" })); // 删除末尾 b
}

void DaChipsEditTest::testPasteSplitDisabledByDefault()
{
    DAChipsEdit e;
    QSignalSpy splitSpy(&e, &DAChipsEdit::pasteSplit);
    QVERIFY(!e.isPasteSplitEnabled());

    // 关闭时退化为普通插入，不产生标签
    e.pasteText(QStringLiteral("alpha"));
    QCOMPARE(e.inputText(), QStringLiteral("alpha"));
    QVERIFY(e.tags().isEmpty());

    e.clearInput();
    e.pasteText(QStringLiteral("a,b"));
    QCOMPARE(splitSpy.count(), 0);
    // 关闭时插入走 QLineEdit::insert，仍会触发既有 textEdited 分隔符路径（与改前一致）
    QCOMPARE(e.tags(), sl({ "a", "b" }));
}

void DaChipsEditTest::testPasteSplitMultiSeparators()
{
    DAChipsEdit e;
    e.setPasteSplitEnabled(true);
    e.setSeparators(QList<DAChipsEdit::Separator>{ DAChipsEdit::Separator::Comma,
                                                   DAChipsEdit::Separator::Semicolon,
                                                   DAChipsEdit::Separator::Enter });
    QSignalSpy splitSpy(&e, &DAChipsEdit::pasteSplit);

    e.pasteText(QStringLiteral("红, 绿; 蓝\n黄"));
    QCOMPARE(e.tags(), (QStringList{ QStringLiteral("红"), QStringLiteral("绿"),
                                     QStringLiteral("蓝"), QStringLiteral("黄") }));
    QCOMPARE(splitSpy.count(), 1);
    QCOMPARE(splitSpy.at(0).at(0).toStringList().size(), 4);
}

void DaChipsEditTest::testPasteSplitTrimsAndDropsEmpty()
{
    DAChipsEdit e;
    e.setPasteSplitEnabled(true);
    e.setSeparators(QList<DAChipsEdit::Separator>{ DAChipsEdit::Separator::Comma,
                                                   DAChipsEdit::Separator::Semicolon,
                                                   DAChipsEdit::Separator::Enter });

    e.pasteText(QStringLiteral("  a ,, b ;\n , c "));
    QCOMPARE(e.tags(), sl({ "a", "b", "c" }));
}

void DaChipsEditTest::testPasteSplitRespectsDedupAndMaxCount()
{
    DAChipsEdit e;
    e.setPasteSplitEnabled(true);
    e.setAllowDuplicate(false);
    e.setMaxCount(3);
    QSignalSpy splitSpy(&e, &DAChipsEdit::pasteSplit);
    QSignalSpy rejectedSpy(&e, &DAChipsEdit::pasteRejected);

    e.pasteText(QStringLiteral("a,b,a,c,d"));
    QCOMPARE(e.tags(), sl({ "a", "b", "c" }));
    QCOMPARE(splitSpy.count(), 1);
    QCOMPARE(splitSpy.at(0).at(0).toStringList().size(), 5);
    QCOMPARE(rejectedSpy.count(), 1);
    QCOMPARE(rejectedSpy.at(0).at(0).toString(), QStringLiteral("maxCount"));
}

void DaChipsEditTest::testPasteSplitEmitsOncePerPaste()
{
    DAChipsEdit e;
    e.setPasteSplitEnabled(true);
    QSignalSpy splitSpy(&e, &DAChipsEdit::pasteSplit);
    QSignalSpy addedSpy(&e, &DAChipsEdit::tagAdded);

    e.pasteText(QStringLiteral("a,b"));
    e.pasteText(QStringLiteral("c,d"));
    QCOMPARE(splitSpy.count(), 2);
    QCOMPARE(addedSpy.count(), 4);
    QCOMPARE(e.tags(), sl({ "a", "b", "c", "d" }));
}

void DaChipsEditTest::testPasteSplitIgnoredWhenAllowCreateFalse()
{
    DAChipsEdit e;
    e.setPasteSplitEnabled(true);
    e.setAllowCreate(false);
    QSignalSpy splitSpy(&e, &DAChipsEdit::pasteSplit);

    e.pasteText(QStringLiteral("a,b"));
    QVERIFY(e.tags().isEmpty());
    QCOMPARE(e.inputText(), QStringLiteral("a,b"));
    QCOMPARE(splitSpy.count(), 0);
}

void DaChipsEditTest::testImeComposingEnterDoesNotCommit()
{
    DAChipsEdit e;
    e.show();
    QVERIFY(QTest::qWaitForWindowExposed(&e));

    auto *input = e.findChild<QLineEdit *>(QStringLiteral("DAChipsEditInput"));
    QVERIFY(input);
    input->setFocus();

    // 模拟输入法组字：preedit 非空（QLineEdit 只显示组字串，不进 text()）
    QInputMethodEvent preedit(QStringLiteral("hong"), QList<QInputMethodEvent::Attribute>());
    QCoreApplication::sendEvent(input, &preedit);

    QTest::keyClick(input, Qt::Key_Return);
    QVERIFY(e.tags().isEmpty());

    // 组字结束：commit 把拼音提交进输入框并清掉组字标志，回车恢复提交
    QInputMethodEvent commit;
    commit.setCommitString(QStringLiteral("hong"));
    QCoreApplication::sendEvent(input, &commit);
    QCOMPARE(input->text(), QStringLiteral("hong"));
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(e.tags(), sl({ "hong" }));
}

void DaChipsEditTest::testTagElideWidthAndTooltip()
{
    DAChipsEdit e;
    e.setTagElideWidth(96);
    QCOMPARE(e.tagElideWidth(), 96);

    const QString full =
        QStringLiteral("this-is-a-very-long-tag-name-that-must-be-elided-").repeated(4);
    e.setTags({ full });
    e.resize(400, 120);
    e.show();
    QVERIFY(QTest::qWaitForWindowExposed(&e));

    auto *chip = e.findChild<DAChip *>();
    QVERIFY(chip);
    auto *label = chip->findChild<QLabel *>(QStringLiteral("DAChipLabel"));
    QVERIFY(label);

    QVERIFY2(chip->width() <= 96,
             qPrintable(QStringLiteral("chip width=%1").arg(chip->width())));
    QCOMPARE(chip->text(), full);
    // 真实字体下文本被截短并以 … 结尾；Qt5 离屏平台无系统字体时可能退化为截短但无省略号
    const QFontMetrics fm(label->font());
    QVERIFY2(label->text().size() < full.size(),
             qPrintable(QStringLiteral("label=%1").arg(label->text())));
    if (fm.inFont(QChar(0x2026))) {
        QVERIFY(label->text().endsWith(QChar(0x2026)));
    } else {
        qWarning("Ellipsis glyph unavailable on this platform; skip ellipsis glyph assertion");
    }
    QCOMPARE(label->toolTip(), full);
    QCOMPARE(chip->toolTip(), full);

    const QPixmap pm = chip->grab();
    QVERIFY(!pm.isNull());
    QVERIFY(pm.width() <= 96);
}

void DaChipsEditTest::testShowCountTextAndGeometry()
{
    DAChipsEdit e;
    e.setMaxCount(5);
    e.setTags({ QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c") });
    e.setShowCount(true);
    QVERIFY(e.showCount());
    e.resize(320, 120);
    e.show();
    QVERIFY(QTest::qWaitForWindowExposed(&e));

    auto *count = e.findChild<QLabel *>(QStringLiteral("DAChipsEditCount"));
    QVERIFY(count);
    QCOMPARE(count->text(), QStringLiteral("3 / 5"));
    QVERIFY(count->geometry().right() < e.width());
    QVERIFY(count->geometry().bottom() < e.height());

    auto *input = e.findChild<QLineEdit *>(QStringLiteral("DAChipsEditInput"));
    QVERIFY(input);
    QVERIFY2(input->width() > 0, "count label must not eat the trailing input's width");
}

void DaChipsEditTest::testShowCountWithoutMaxCount()
{
    DAChipsEdit e;
    e.setShowCount(true);
    e.setTags({ QStringLiteral("a"), QStringLiteral("b") });
    e.resize(320, 120);
    e.show();
    QVERIFY(QTest::qWaitForWindowExposed(&e));

    auto *count = e.findChild<QLabel *>(QStringLiteral("DAChipsEditCount"));
    QVERIFY(count);
    QCOMPARE(count->text(), QStringLiteral("2"));
    QVERIFY(!count->text().contains(QLatin1Char('/')));
}

QTEST_MAIN(DaChipsEditTest)
#include "tst_dachipsedit.moc"
