#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DACollapseItem.h>
#include <DACollapseWidget.h>

// ============================================================
// 条目选择 Tab（勾选列表 + 字符串列表）
// Wave 1 填入 DACheckListWidget / DAItemListPickDialog / DAStringListEditWidget
// ============================================================
QWidget *createItemSelectionTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    auto *collapse = new DA::DACollapseWidget(content);
    collapse->addItem(QStringLiteral("条目选择控件（待 Wave 1 填充）"), QStringLiteral("placeholder"))
        ->setContentWidget(new QLabel(QStringLiteral("DACheckListWidget / DAStringListEditWidget")));

    v->addWidget(collapse);
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
