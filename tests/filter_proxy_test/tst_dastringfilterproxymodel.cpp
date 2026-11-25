#include <QStandardItem>
#include <QStandardItemModel>
#include <QtTest/QtTest>

#include <DAStringFilterProxyModel.h>

using namespace DA;

class DaStringFilterProxyModelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testFlatFilter();
    void testCaseSensitivity();
    void testColumnFilter();
    void testFilterRole();
    void testMatchesText();
    void testIncludeParentsOfMatches();
    void testClearFilterRestoresAll();
    void testMatchRangesInBasic();
    void testMatchRangesInEmptyFilter();
    void testMatchRangesInCaseSensitivity();
    void testMatchRangesInOverlapPolicy();
    void testMatchRangesFlatIndex();
    void testMatchRangesColumnScope();
    void testMatchRangesParentWithoutOwnHit();
    void testMatchRangesInvalidIndex();
    void testMatchRangesMatchesTextConsistency();

private:
    static QStandardItemModel* makeFlatModel(const QStringList& texts);
    static QStandardItemModel* makeTreeModel();
};

QStandardItemModel* DaStringFilterProxyModelTest::makeFlatModel(const QStringList& texts)
{
    auto* m = new QStandardItemModel;
    for (const QString& t : texts) {
        m->appendRow(new QStandardItem(t));
    }
    return m;
}

QStandardItemModel* DaStringFilterProxyModelTest::makeTreeModel()
{
    auto* m = new QStandardItemModel;
    auto* fruit = new QStandardItem(QStringLiteral("Fruit"));
    fruit->appendRow(new QStandardItem(QStringLiteral("Apple")));
    fruit->appendRow(new QStandardItem(QStringLiteral("Banana")));
    auto* veg = new QStandardItem(QStringLiteral("Vegetable"));
    veg->appendRow(new QStandardItem(QStringLiteral("Carrot")));
    m->appendRow(fruit);
    m->appendRow(veg);
    return m;
}

void DaStringFilterProxyModelTest::testFlatFilter()
{
    QScopedPointer< QStandardItemModel > src(makeFlatModel({ "Apple", "Banana", "Cherry" }));
    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src.data());
    QCOMPARE(proxy.rowCount(), 3);
    proxy.setFilterText(QStringLiteral("an"));
    QCOMPARE(proxy.rowCount(), 1);
    QCOMPARE(proxy.index(0, 0).data().toString(), QStringLiteral("Banana"));
}

void DaStringFilterProxyModelTest::testCaseSensitivity()
{
    QScopedPointer< QStandardItemModel > src(makeFlatModel({ "Apple", "Banana" }));
    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src.data());
    // 默认不敏感
    proxy.setFilterText(QStringLiteral("APP"));
    QCOMPARE(proxy.rowCount(), 1);
    // 切到敏感后不再命中
    proxy.setFilterCaseSensitivity(Qt::CaseSensitive);
    QCOMPARE(proxy.rowCount(), 0);
    QCOMPARE(proxy.filterCaseSensitivity(), Qt::CaseSensitive);
}

void DaStringFilterProxyModelTest::testColumnFilter()
{
    auto* src = new QStandardItemModel;
    QList< QStandardItem* > row1;
    row1 << new QStandardItem(QStringLiteral("Alice")) << new QStandardItem(QStringLiteral("Engineer"));
    QList< QStandardItem* > row2;
    row2 << new QStandardItem(QStringLiteral("Bob")) << new QStandardItem(QStringLiteral("Designer"));
    src->appendRow(row1);
    src->appendRow(row2);
    QScopedPointer< QStandardItemModel > guard(src);

    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src);
    proxy.setFilterColumn(1);
    proxy.setFilterText(QStringLiteral("engineer"));
    QCOMPARE(proxy.rowCount(), 1);
    QCOMPARE(proxy.index(0, 0).data().toString(), QStringLiteral("Alice"));

    proxy.setFilterColumn(0);
    proxy.setFilterText(QStringLiteral("bob"));
    QCOMPARE(proxy.rowCount(), 1);
    QCOMPARE(proxy.index(0, 1).data().toString(), QStringLiteral("Designer"));
}

void DaStringFilterProxyModelTest::testFilterRole()
{
    auto* src  = new QStandardItemModel;
    auto* item = new QStandardItem(QStringLiteral("Visible"));
    item->setData(QStringLiteral("secret-id"), Qt::UserRole + 5);
    src->appendRow(item);
    QScopedPointer< QStandardItemModel > guard(src);

    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src);
    proxy.setFilterRole(Qt::UserRole + 5);
    proxy.setFilterText(QStringLiteral("secret"));
    QCOMPARE(proxy.rowCount(), 1);
    // 显示文本不参与匹配
    proxy.setFilterText(QStringLiteral("visible"));
    QCOMPARE(proxy.rowCount(), 0);
}

void DaStringFilterProxyModelTest::testIncludeParentsOfMatches()
{
    QScopedPointer< QStandardItemModel > src(makeTreeModel());
    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src.data());
    proxy.setFilterText(QStringLiteral("Carrot"));
    // 默认不放行父节点：父节点自身不匹配，子节点无法显示
    QCOMPARE(proxy.rowCount(), 0);
    proxy.setIncludeParentsOfMatches(true);
    QCOMPARE(proxy.rowCount(), 1);
    QCOMPARE(proxy.index(0, 0).data().toString(), QStringLiteral("Vegetable"));
    QCOMPARE(proxy.rowCount(proxy.index(0, 0)), 1);
    QCOMPARE(proxy.index(0, 0, proxy.index(0, 0)).data().toString(), QStringLiteral("Carrot"));
}

void DaStringFilterProxyModelTest::testClearFilterRestoresAll()
{
    QScopedPointer< QStandardItemModel > src(makeTreeModel());
    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src.data());
    proxy.setIncludeParentsOfMatches(true);
    proxy.setFilterText(QStringLiteral("apple"));
    QCOMPARE(proxy.rowCount(), 1);
    proxy.setFilterText(QString());
    QCOMPARE(proxy.rowCount(), 2);
}

void DaStringFilterProxyModelTest::testMatchesText()
{
    QVERIFY(DAStringFilterProxyModel::matchesText(QStringLiteral("Save Project"), QString(), Qt::CaseInsensitive));
    QVERIFY(DAStringFilterProxyModel::matchesText(QStringLiteral("Save Project"), QStringLiteral("save"), Qt::CaseInsensitive));
    QVERIFY(!DAStringFilterProxyModel::matchesText(QStringLiteral("Save Project"), QStringLiteral("save"), Qt::CaseSensitive));
}

void DaStringFilterProxyModelTest::testMatchRangesInBasic()
{
    const auto ranges = DAStringFilterProxyModel::matchRangesIn(QStringLiteral("Banana"),
                                                                QStringLiteral("an"),
                                                                Qt::CaseInsensitive);
    const QVector< QPair< int, int > > expected{ { 1, 2 }, { 3, 2 } };
    QCOMPARE(ranges, expected);
    const QString text = QStringLiteral("Banana");
    for (const auto& r : ranges)
        QCOMPARE(text.mid(r.first, r.second).compare(QStringLiteral("an"), Qt::CaseInsensitive), 0);
}

void DaStringFilterProxyModelTest::testMatchRangesInEmptyFilter()
{
    QVERIFY(DAStringFilterProxyModel::matchRangesIn(QStringLiteral("Banana"), QString(), Qt::CaseInsensitive)
                .isEmpty());
    QVERIFY(DAStringFilterProxyModel::matchesText(QStringLiteral("Banana"), QString(), Qt::CaseInsensitive));
}

void DaStringFilterProxyModelTest::testMatchRangesInCaseSensitivity()
{
    const QString text = QStringLiteral("Save Project");
    const auto insensitive = DAStringFilterProxyModel::matchRangesIn(text, QStringLiteral("SAVE"),
                                                                     Qt::CaseInsensitive);
    const QVector< QPair< int, int > > expected{ { 0, 4 } };
    QCOMPARE(insensitive, expected);
    QVERIFY(DAStringFilterProxyModel::matchRangesIn(text, QStringLiteral("SAVE"), Qt::CaseSensitive).isEmpty());
}

void DaStringFilterProxyModelTest::testMatchRangesInOverlapPolicy()
{
    const auto ranges = DAStringFilterProxyModel::matchRangesIn(QStringLiteral("aaaa"), QStringLiteral("aa"),
                                                                Qt::CaseInsensitive);
    const QVector< QPair< int, int > > expected{ { 0, 2 }, { 2, 2 } };
    QCOMPARE(ranges, expected);
}

void DaStringFilterProxyModelTest::testMatchRangesFlatIndex()
{
    QScopedPointer< QStandardItemModel > src(makeFlatModel({ "Apple", "Banana", "Cherry" }));
    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src.data());
    proxy.setFilterText(QStringLiteral("an"));
    QCOMPARE(proxy.rowCount(), 1);
    QCOMPARE(proxy.index(0, 0).data().toString(), QStringLiteral("Banana"));
    const auto ranges = proxy.matchRanges(proxy.index(0, 0));
    QCOMPARE(ranges.size(), 2);
    QCOMPARE(ranges.at(0), qMakePair(1, 2));
    QCOMPARE(ranges.at(1), qMakePair(3, 2));
}

void DaStringFilterProxyModelTest::testMatchRangesColumnScope()
{
    auto* src = new QStandardItemModel;
    src->appendRow(QList< QStandardItem* > { new QStandardItem(QStringLiteral("alpha")),
                                             new QStandardItem(QStringLiteral("beta")) });
    src->appendRow(QList< QStandardItem* > { new QStandardItem(QStringLiteral("gamma")),
                                             new QStandardItem(QStringLiteral("delta")) });
    QScopedPointer< QStandardItemModel > guard(src);

    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src);
    proxy.setFilterText(QStringLiteral("a"));
    QCOMPARE(proxy.rowCount(), 2);

    proxy.setFilterColumn(1);
    QVERIFY(proxy.matchRanges(proxy.index(0, 0)).isEmpty());
    const QVector< QPair< int, int > > betaRange{ { 3, 1 } };
    QCOMPARE(proxy.matchRanges(proxy.index(0, 1)), betaRange);

    proxy.setFilterColumn(-1);
    const auto col0 = proxy.matchRanges(proxy.index(0, 0));
    const QVector< QPair< int, int > > expected{ { 0, 1 }, { 4, 1 } };
    QCOMPARE(col0, expected);
    QCOMPARE(proxy.matchRanges(proxy.index(0, 1)), betaRange);
}

void DaStringFilterProxyModelTest::testMatchRangesParentWithoutOwnHit()
{
    QScopedPointer< QStandardItemModel > src(makeTreeModel());
    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(src.data());
    proxy.setIncludeParentsOfMatches(true);
    proxy.setFilterText(QStringLiteral("Carrot"));
    QCOMPARE(proxy.rowCount(), 1);
    const QModelIndex parent = proxy.index(0, 0);
    QCOMPARE(parent.data().toString(), QStringLiteral("Vegetable"));
    QVERIFY(proxy.matchRanges(parent).isEmpty());
    const auto child = proxy.index(0, 0, parent);
    QCOMPARE(child.data().toString(), QStringLiteral("Carrot"));
    const QVector< QPair< int, int > > carrotRange{ { 0, 6 } };
    QCOMPARE(proxy.matchRanges(child), carrotRange);
}

void DaStringFilterProxyModelTest::testMatchRangesInvalidIndex()
{
    DAStringFilterProxyModel noSourceProxy;
    QVERIFY(noSourceProxy.matchRanges(QModelIndex()).isEmpty());

    QStringList oneItem;
    oneItem << QStringLiteral("Apple");
    QScopedPointer< QStandardItemModel > foreign(makeFlatModel(oneItem));
    QScopedPointer< QStandardItemModel > otherModel(makeFlatModel(oneItem));
    DAStringFilterProxyModel proxy;
    proxy.setSourceModel(otherModel.data());
    proxy.setFilterText(QStringLiteral("App"));
    QVERIFY(proxy.matchRanges(foreign->index(0, 0)).isEmpty());
}

void DaStringFilterProxyModelTest::testMatchRangesMatchesTextConsistency()
{
    const QStringList texts{ QStringLiteral("Save Project"), QStringLiteral("Banana"),
                             QStringLiteral("ÄÖÜ"), QStringLiteral("aaaa"), QStringLiteral("abcabc") };
    const QStringList filters{ QStringLiteral("an"), QStringLiteral("save"), QStringLiteral("aa"),
                               QStringLiteral("äöü") };
    for (const QString& text : texts) {
        for (const QString& filter : filters) {
            const auto ranges = DAStringFilterProxyModel::matchRangesIn(text, filter, Qt::CaseInsensitive);
            const bool matched = DAStringFilterProxyModel::matchesText(text, filter, Qt::CaseInsensitive);
            QCOMPARE(!ranges.isEmpty(), matched);
            for (const auto& r : ranges)
                QCOMPARE(text.mid(r.first, r.second).compare(filter, Qt::CaseInsensitive), 0);
        }
        QVERIFY(DAStringFilterProxyModel::matchRangesIn(text, QString(), Qt::CaseInsensitive).isEmpty());
        QVERIFY(DAStringFilterProxyModel::matchesText(text, QString(), Qt::CaseInsensitive));
    }
}

QTEST_MAIN(DaStringFilterProxyModelTest)
#include "tst_dastringfilterproxymodel.moc"
