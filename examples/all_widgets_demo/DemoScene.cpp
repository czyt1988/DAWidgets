#include "DemoScene.h"

#include <QScrollArea>
#include <QStringList>
#include <QVBoxLayout>

#include <DACollapseItem.h>
#include <DACollapseWidget.h>

#include "TabPages.h"

QWidget *createSceneTab(const QVector< DemoScene > &scenes, QWidget *parent, bool expandAll)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);

    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    auto *collapse = new DA::DACollapseWidget(content);
    QStringList activeNames;
    for (const DemoScene &scene : scenes) {
        auto *item = collapse->addItem(scene.title, scene.id);
        item->setContentWidget(scene.create(collapse));
        if (expandAll) {
            activeNames << scene.id;
        }
    }
    if (!expandAll && !scenes.isEmpty()) {
        activeNames << scenes.first().id;
    }
    collapse->setActiveNames(activeNames);

    v->addWidget(collapse);
    v->addStretch(1);
    scroll->setWidget(content);
    return scroll;
}

QVector< DemoTab > demoTabs()
{
    return {
        { QStringLiteral("选择器 / 标签"), QStringLiteral("selector"), selectorScenes() },
        { QStringLiteral("折叠面板"), QStringLiteral("collapse"), collapseScenes() },
        { QStringLiteral("颜色"), QStringLiteral("color"), colorScenes() },
        { QStringLiteral("绘图样式"), QStringLiteral("drawing-style"), drawingStyleScenes() },
        { QStringLiteral("对齐"), QStringLiteral("alignment"), alignmentScenes() },
        { QStringLiteral("工具组件"), QStringLiteral("utility"), utilityScenes() },
        { QStringLiteral("条目选择"), QStringLiteral("item-selection"), itemSelectionScenes() },
        { QStringLiteral("文本"), QStringLiteral("text"), textScenes() },
        { QStringLiteral("过滤"), QStringLiteral("filter"), filterScenes() },
    };
}
