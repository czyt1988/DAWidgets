#include <QLineEdit>
#include <QtTest/QtTest>

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

QTEST_MAIN(DaChipsEditTest)
#include "tst_dachipsedit.moc"
