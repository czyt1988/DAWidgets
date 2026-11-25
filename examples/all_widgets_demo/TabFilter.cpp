#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QTreeView>
#include <QVBoxLayout>
#include <functional>

#include "DAQt5Qt6Compat.h"
#include <DAStringFilterProxyModel.h>

#include "TabPages.h"

namespace
{
// demo 内部的最小高亮 delegate：不进库，只演示 matchRanges() 够用
class HighlightDelegate : public QStyledItemDelegate
{
public:
    explicit HighlightDelegate(DA::DAStringFilterProxyModel* proxy, QObject* parent = nullptr)
        : QStyledItemDelegate(parent), mProxy(proxy)
    {
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        const QVector< QPair< int, int > > ranges = mProxy->matchRanges(index);
        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);
        const QFontMetrics fm(opt.font);
        // 空区间 / 选中 / 超宽：回退原路，宁可少画不可画错
        if (ranges.isEmpty() || (opt.state & QStyle::State_Selected)
            || Qt5Qt6Compat_fontMetrics_width(fm, opt.text) > opt.rect.width() - 4) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }
        const QString text = opt.text;
        opt.text.clear();
        QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        const QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);

        painter->save();
        painter->setFont(opt.font);
        painter->setPen(opt.palette.color(QPalette::Text));
        int x = textRect.left() + 2;
        const int baseline = textRect.top() + (textRect.height() - fm.height()) / 2 + fm.ascent();
        int last = 0;
        for (const auto& r : ranges) {
            if (r.first > last) {
                const QString seg = text.mid(last, r.first - last);
                painter->drawText(QPoint(x, baseline), seg);
                x += Qt5Qt6Compat_fontMetrics_width(fm, seg);
            }
            const QString hit = text.mid(r.first, r.second);
            const int w = Qt5Qt6Compat_fontMetrics_width(fm, hit);
            painter->fillRect(QRect(x, textRect.top() + 1, w, textRect.height() - 2),
                              QColor(255, 214, 102, 130));
            painter->drawText(QPoint(x, baseline), hit);
            x += w;
            last = r.first + r.second;
        }
        if (last < text.size())
            painter->drawText(QPoint(x, baseline), text.mid(last));
        painter->restore();
    }

private:
    DA::DAStringFilterProxyModel* mProxy;
};

QStandardItem* leaf(const QString& text)
{
    return new QStandardItem(text);
}
}  // namespace

// ============================================================
// DAStringFilterProxyModel —— 树/列表过滤
// ============================================================
static QWidget *createFilterContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("通用文本过滤代理：默认大小写不敏感、可限定列；树形模型开启"
                       "“子节点命中放行父节点”后，父节点不会被单独过滤掉。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *model = new QStandardItemModel(w);
    auto *fruit = new QStandardItem(QStringLiteral("水果"));
    fruit->appendRow(leaf(QStringLiteral("苹果 Apple")));
    fruit->appendRow(leaf(QStringLiteral("香蕉 Banana")));
    auto *veg = new QStandardItem(QStringLiteral("蔬菜"));
    veg->appendRow(leaf(QStringLiteral("胡萝卜 Carrot")));
    veg->appendRow(leaf(QStringLiteral("土豆 Potato")));
    auto *tool = new QStandardItem(QStringLiteral("工具"));
    tool->appendRow(leaf(QStringLiteral("扳手 Wrench")));
    model->appendRow(fruit);
    model->appendRow(veg);
    model->appendRow(tool);

    auto *proxy = new DA::DAStringFilterProxyModel(w);
    proxy->setSourceModel(model);
    proxy->setIncludeParentsOfMatches(true);

    auto *search = new QLineEdit(w);
    search->setPlaceholderText(QStringLiteral("输入过滤文本（例如 ban / 胡萝 / wrench）"));
    search->setClearButtonEnabled(true);
    v->addWidget(search);

    auto *view = new QTreeView(w);
    view->setModel(proxy);
    view->setHeaderHidden(true);
    view->expandAll();
    v->addWidget(view, 1);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);
    auto updateCount = [proxy, model, log]() {
        log->setText(QStringLiteral("过滤后顶层 %1 项 / 源顶层 %2 项")
                         .arg(proxy->rowCount())
                         .arg(model->rowCount()));
    };
    QObject::connect(search, &QLineEdit::textChanged, proxy, [proxy, updateCount](const QString &t) {
        proxy->setFilterText(t);
        updateCount();
    });
    updateCount();

    return w;
}

// ============================================================
// DAStringFilterProxyModel —— 命中区间高亮（最小 delegate 示例）
// ============================================================
static QWidget *createHighlightContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("matchRanges() 把「这一格文本里哪几段命中」交给 delegate，库只算位置不做高亮。\n"
                       "左侧树演示「父节点仅因子节点命中被放行 → 自身不高亮」；右侧表格演示多列按列返回。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *search = new QLineEdit(w);
    search->setPlaceholderText(QStringLiteral("输入过滤文本（例如 an / 胡萝 / 设计）"));
    search->setClearButtonEnabled(true);
    v->addWidget(search);

    // 左：树（父节点放行）
    auto *treeModel = new QStandardItemModel(w);
    auto *fruit = new QStandardItem(QStringLiteral("Fruit"));
    fruit->appendRow(new QStandardItem(QStringLiteral("Apple")));
    fruit->appendRow(new QStandardItem(QStringLiteral("Banana")));
    auto *veg = new QStandardItem(QStringLiteral("Vegetable"));
    veg->appendRow(new QStandardItem(QStringLiteral("Carrot")));
    veg->appendRow(new QStandardItem(QStringLiteral("Potato")));
    treeModel->appendRow(fruit);
    treeModel->appendRow(veg);

    auto *treeProxy = new DA::DAStringFilterProxyModel(w);
    treeProxy->setSourceModel(treeModel);
    treeProxy->setIncludeParentsOfMatches(true);

    // 右：两列表格（按列返回区间）
    auto *tableModel = new QStandardItemModel(0, 2, w);
    tableModel->setHorizontalHeaderLabels({ QStringLiteral("名称"), QStringLiteral("描述") });
    const QList< QPair< QString, QString > > rows{
        { QStringLiteral("Engineer"), QStringLiteral("设计 and 实现") },
        { QStringLiteral("Designer"), QStringLiteral("界面设计") },
        { QStringLiteral("Manager"), QStringLiteral("计划与协调") },
    };
    for (const auto& r : rows) {
        tableModel->appendRow(QList< QStandardItem* > { new QStandardItem(r.first),
                                                        new QStandardItem(r.second) });
    }
    auto *tableProxy = new DA::DAStringFilterProxyModel(w);
    tableProxy->setSourceModel(tableModel);

    auto *views = new QHBoxLayout;
    auto *tree = new QTreeView(w);
    tree->setModel(treeProxy);
    tree->setHeaderHidden(true);
    tree->expandAll();
    tree->setItemDelegate(new HighlightDelegate(treeProxy, tree));
    views->addWidget(tree, 1);

    auto *table = new QTableView(w);
    table->setModel(tableProxy);
    table->setItemDelegate(new HighlightDelegate(tableProxy, table));
    table->horizontalHeader()->setStretchLastSection(true);
    table->setColumnWidth(0, 110);
    views->addWidget(table, 1);
    v->addLayout(views, 1);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    log->setWordWrap(true);
    v->addWidget(log);

    auto update = [ treeProxy, tableProxy, search, log ]() {
        int hits = 0;
        std::function< void(const QModelIndex&, DA::DAStringFilterProxyModel*) > walk =
            [ & ](const QModelIndex& parent, DA::DAStringFilterProxyModel* proxy) {
                for (int r = 0; r < proxy->rowCount(parent); ++r) {
                    for (int c = 0; c < proxy->columnCount(parent); ++c) {
                        hits += proxy->matchRanges(proxy->index(r, c, parent)).size();
                    }
                    walk(proxy->index(r, 0, parent), proxy);
                }
            };
        walk(QModelIndex(), treeProxy);
        for (int r = 0; r < tableProxy->rowCount(); ++r) {
            for (int c = 0; c < tableProxy->columnCount(); ++c)
                hits += tableProxy->matchRanges(tableProxy->index(r, c)).size();
        }
        log->setText(QStringLiteral("当前过滤：%1 / 高亮段数：%2")
                         .arg(search->text().isEmpty() ? QStringLiteral("（无）") : search->text())
                         .arg(hits));
    };
    QObject::connect(search, &QLineEdit::textChanged, w, [ treeProxy, tableProxy, update ](const QString& t) {
        treeProxy->setFilterText(t);
        tableProxy->setFilterText(t);
        update();
    });
    search->setText(QStringLiteral("an"));
    update();

    return w;
}

// ============================================================
// 场景列表（tab 与截图导出共用）
// ============================================================
QVector< DemoScene > filterScenes()
{
    return {
        { QStringLiteral("filter-tree"), QStringLiteral("DAStringFilterProxyModel — 树过滤"),
          createFilterContent, 340 },
        { QStringLiteral("filter-highlight"),
          QStringLiteral("DAStringFilterProxyModel — 命中高亮"), createHighlightContent, 340 },
    };
}
