#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "DACollapseWidget.h"
#include "DACollapseItem.h"

static const char* kStyle = R"(
DA--DACollapseItem {
    border:1px solid #ebeef5;
    border-radius:4px;
    margin-bottom:8px;
}
QFrame#header {
    background:#fafafa;
    border:none;
    border-top-left-radius:4px;
    border-top-right-radius:4px;
}
QFrame#header:hover {
    background:#f5f7fa;
}
/* 箭头为 svg 图标,颜色由 svg 文件 fill 决定,qss color 无效 */
QLabel#DACollapseItemTitle { color:#303133; font-weight:bold; }
DA--DACollapseItem[disabled="true"] QFrame#header { color:#c0c4cc; }
DA--DACollapseItem[disabled="true"] QFrame#header:hover { background:#fafafa; }
QFrame#DACollapseItemContent { background:#ffffff; }
)";

static QLabel* makeContent(const QStringList& lines, QWidget* parent)
{
    auto* w = new QLabel(parent);
    w->setWordWrap(true);
    w->setText(lines.join(QStringLiteral("<br>")));
    w->setTextFormat(Qt::RichText);
    return w;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setStyleSheet(QString::fromLatin1(kStyle));

    QWidget w;
    w.setWindowTitle(QStringLiteral("DACollapse Demo - 对标 Element UI Collapse"));

    auto* root = new QVBoxLayout(&w);

    // ---- 基础用法:可同时展开多个 ----
    auto* col = new DA::DACollapseWidget(&w);
    col->addItem(QStringLiteral("一致性 Consistency"), QStringLiteral("1"))
        ->setContentWidget(makeContent(
            {
                QStringLiteral("与现实生活一致：与现实生活的流程、逻辑保持一致，遵循用户习惯的语言和概念；"),
                QStringLiteral("在界面中一致：所有的元素和结构需保持一致，比如：设计样式、图标和文本、元素的位置等。"),
            },
            col));
    col->addItem(QStringLiteral("反馈 Feedback"), QStringLiteral("2"))
        ->setContentWidget(makeContent(
            {
                QStringLiteral("控制反馈：通过界面样式和交互动效让用户可以清晰的感知自己的操作；"),
                QStringLiteral("页面反馈：操作后，通过页面元素的变化清晰地展现当前状态。"),
            },
            col));
    col->addItem(QStringLiteral("效率 Efficiency"), QStringLiteral("3"))
        ->setContentWidget(makeContent(
            {
                QStringLiteral("简化流程：设计简洁直观的操作流程；"),
                QStringLiteral("清晰明确：语言表达清晰且表意明确，让用户快速理解进而作出决策；"),
            },
            col));
    // 禁用项演示
    auto* disabledItem = col->addItem(QStringLiteral("可控 Controllability（禁用）"), QStringLiteral("4"));
    disabledItem->setDisabled(true);
    disabledItem->setContentWidget(makeContent(
        {
            QStringLiteral("用户决策：根据场景可给予用户操作建议或安全提示，但不能代替用户进行决策；"),
        },
        col));

    col->setActiveNames({ QStringLiteral("1") });

    auto* statusLabel = new QLabel(QStringLiteral("activeNames: [\"1\"]"), &w);
    QObject::connect(col, &DA::DACollapseWidget::activeNamesChanged, [ statusLabel ](const QStringList& names) {
        statusLabel->setText(QStringLiteral("activeNames: ") + names.join(QStringLiteral(", ")));
    });

    root->addWidget(new QLabel(QStringLiteral("基础用法（可同时展开多个面板）"), &w));
    root->addWidget(col);
    root->addWidget(statusLabel);

    // ---- 手风琴模式 ----
    auto* acc = new DA::DACollapseWidget(&w);
    acc->setAccordion(true);
    acc->addItem(QStringLiteral("一致性 Consistency"), QStringLiteral("a1"))
        ->setContentWidget(makeContent(
            {
                QStringLiteral("与现实生活一致：与现实生活的流程、逻辑保持一致，遵循用户习惯的语言和概念；"),
            },
            acc));
    acc->addItem(QStringLiteral("反馈 Feedback"), QStringLiteral("a2"))
        ->setContentWidget(makeContent(
            {
                QStringLiteral("控制反馈：通过界面样式和交互动效让用户可以清晰的感知自己的操作；"),
            },
            acc));
    acc->addItem(QStringLiteral("效率 Efficiency"), QStringLiteral("a3"))
        ->setContentWidget(makeContent(
            {
                QStringLiteral("简化流程：设计简洁直观的操作流程；"),
            },
            acc));
    acc->setActiveNames({ QStringLiteral("a1") });

    root->addSpacing(12);
    root->addWidget(new QLabel(QStringLiteral("手风琴效果（每次只能展开一个面板）"), &w));
    root->addWidget(acc);

    // ---- 自定义标题演示 ----
    auto* acc2 = new DA::DACollapseWidget(&w);
    acc2->setAccordion(true);
    auto* ci          = acc2->addItem(QString(), QStringLiteral("c1"));
    auto* customTitle = new QLabel(QStringLiteral("自定义标题 <b>带图标</b> \xe2\x9c\x93"), ci);
    customTitle->setTextFormat(Qt::RichText);
    ci->setTitleWidget(customTitle);
    ci->setContentWidget(
        makeContent({ QStringLiteral("通过 setTitleWidget 设置自定义标题内容，对标 Element UI 的 title slot。") }, acc2));

    root->addSpacing(12);
    root->addWidget(new QLabel(QStringLiteral("自定义面板标题"), &w));
    root->addWidget(acc2);

    root->addStretch(1);

    w.resize(520, 720);
    w.show();
    return app.exec();
}
