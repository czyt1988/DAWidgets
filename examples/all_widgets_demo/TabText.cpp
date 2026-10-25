#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DACollapseItem.h>
#include <DACollapseWidget.h>

// ============================================================
// 文本 Tab（文本查看/编辑器）
// Wave 2 填入 DATextEditWidget
// ============================================================
QWidget *createTextTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    auto *collapse = new DA::DACollapseWidget(content);
    collapse->addItem(QStringLiteral("文本编辑器（待 Wave 2 填充）"), QStringLiteral("placeholder"))
        ->setContentWidget(new QLabel(QStringLiteral("DATextEditWidget")));

    v->addWidget(collapse);
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
