#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include "DACollapseWidget.h"
#include "DACollapseItem.h"

// 辅助：生成富文本内容
static QLabel* makeContent(const QStringList& lines, QWidget* parent)
{
    auto* w = new QLabel(parent);
    w->setWordWrap(true);
    w->setText(lines.join(QStringLiteral("<br>")));
    w->setTextFormat(Qt::RichText);
    return w;
}

QWidget* createCollapseTab(QWidget* parent)
{
    auto* scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto* content = new QWidget;
    auto* v       = new QVBoxLayout(content);
    v->setSpacing(12);

    // ============================================================
    // 基础用法：可同时展开多个面板
    // ============================================================
    {
        auto* box = new QGroupBox(QStringLiteral("基础用法 — 可同时展开多个面板"), content);
        auto* lay = new QVBoxLayout(box);

        auto* desc = new QLabel(QStringLiteral("DACollapse 是折叠面板容器，内部放置 DACollapseItem。\n"
                                               "点击标题栏展开/折叠内容区域，非手风琴模式下可同时展开多个。"),
                                box);
        desc->setWordWrap(true);
        lay->addWidget(desc);

        auto* col = new DA::DACollapseWidget(box);
        col->addItem(QStringLiteral("一致性 Consistency"), QStringLiteral("1"))
            ->setContentWidget(makeContent(
                {
                    QStringLiteral("与现实生活一致：与现实生活的流程、逻辑保持一致，遵循用户习惯的语言和概念；"),
                    QStringLiteral(
                        "在界面中一致：所有的元素和结构需保持一致，比如：设计样式、图标和文本、元素的位置等。"),
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
        col->setActiveNames({ QStringLiteral("1") });

        auto* statusLabel = new QLabel(QStringLiteral("activeNames: [\"1\"]"), box);
        QObject::connect(col, &DA::DACollapseWidget::activeNamesChanged, [ statusLabel ](const QStringList& names) {
            statusLabel->setText(QStringLiteral("activeNames: ") + names.join(QStringLiteral(", ")));
        });

        lay->addWidget(col);
        lay->addWidget(statusLabel);
        v->addWidget(box);
    }

    // ============================================================
    // 手风琴模式：每次只能展开一个面板
    // ============================================================
    {
        auto* box = new QGroupBox(QStringLiteral("手风琴模式 — 每次只能展开一个面板"), content);
        auto* lay = new QVBoxLayout(box);

        auto* desc = new QLabel(QStringLiteral("setAccordion(true) 后，展开新面板会自动折叠上一个面板。"), box);
        desc->setWordWrap(true);
        lay->addWidget(desc);

        auto* acc = new DA::DACollapseWidget(box);
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

        lay->addWidget(acc);
        v->addWidget(box);
    }

    // ============================================================
    // 自定义标题 + 禁用项
    // ============================================================
    {
        auto* box = new QGroupBox(QStringLiteral("自定义标题 & 禁用项"), content);
        auto* lay = new QVBoxLayout(box);

        auto* desc =
            new QLabel(QStringLiteral("setTitleWidget() 可设置自定义标题内容（对标 Element UI 的 title slot）。\n"
                                      "setDisabled(true) 可禁用某个面板，使其无法展开。"),
                       box);
        desc->setWordWrap(true);
        lay->addWidget(desc);

        auto* acc = new DA::DACollapseWidget(box);
        acc->setAccordion(true);

        // 自定义标题
        auto* ci          = acc->addItem(QString(), QStringLiteral("c1"));
        auto* customTitle = new QLabel(QStringLiteral("自定义标题 <b>带富文本</b> ✓"), ci);
        customTitle->setTextFormat(Qt::RichText);
        ci->setTitleWidget(customTitle);
        ci->setContentWidget(makeContent(
            {
                QStringLiteral("通过 setTitleWidget 设置自定义标题内容，对标 Element UI 的 title slot。"),
            },
            acc));

        // 禁用项
        auto* disabledItem = acc->addItem(QStringLiteral("可控 Controllability（禁用）"), QStringLiteral("c2"));
        disabledItem->setDisabled(true);
        disabledItem->setContentWidget(makeContent(
            {
                QStringLiteral("用户决策：根据场景可给予用户操作建议或安全提示，但不能代替用户进行决策；"),
            },
            acc));

        lay->addWidget(acc);
        v->addWidget(box);
    }

    v->addStretch(1);
    scroll->setWidget(content);
    return scroll;
}
