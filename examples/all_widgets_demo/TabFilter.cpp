#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DACollapseItem.h>
#include <DACollapseWidget.h>

// ============================================================
// 过滤 Tab（树 + 过滤代理）
// Wave 2 填入 DAStringFilterProxyModel
// ============================================================
QWidget *createFilterTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    auto *collapse = new DA::DACollapseWidget(content);
    collapse->addItem(QStringLiteral("树过滤（待 Wave 2 填充）"), QStringLiteral("placeholder"))
        ->setContentWidget(new QLabel(QStringLiteral("DAStringFilterProxyModel")));

    v->addWidget(collapse);
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
