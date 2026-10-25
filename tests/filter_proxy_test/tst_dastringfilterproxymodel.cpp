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
    void testIncludeParentsOfMatches();
    void testClearFilterRestoresAll();

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

QTEST_MAIN(DaStringFilterProxyModelTest)
#include "tst_dastringfilterproxymodel.moc"
