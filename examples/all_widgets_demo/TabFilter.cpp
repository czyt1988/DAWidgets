#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

#include <DAStringFilterProxyModel.h>

#include "TabPages.h"

// ============================================================
// DAStringFilterProxyModel —— 树/列表过滤
// ============================================================
static QStandardItem *makeLeaf(const QString &text)
{
    return new QStandardItem(text);
}

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
    fruit->appendRow(makeLeaf(QStringLiteral("苹果 Apple")));
    fruit->appendRow(makeLeaf(QStringLiteral("香蕉 Banana")));
    auto *veg = new QStandardItem(QStringLiteral("蔬菜"));
    veg->appendRow(makeLeaf(QStringLiteral("胡萝卜 Carrot")));
    veg->appendRow(makeLeaf(QStringLiteral("土豆 Potato")));
    auto *tool = new QStandardItem(QStringLiteral("工具"));
    tool->appendRow(makeLeaf(QStringLiteral("扳手 Wrench")));
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
// 场景列表（tab 与截图导出共用）
// ============================================================
QVector< DemoScene > filterScenes()
{
    return {
        { QStringLiteral("filter-tree"), QStringLiteral("DAStringFilterProxyModel — 树过滤"),
          createFilterContent, 340 },
    };
}
