#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <DACollapseItem.h>
#include <DACollapseWidget.h>

#include "TabPages.h"

// 向折叠项内容区追加若干行说明文字
static void addTextContent(DA::DACollapseItem *item, const QStringList &lines)
{
    for (const QString &line : lines) {
        auto *label = new QLabel(line);
        label->setWordWrap(true);
        item->contentLayout()->addWidget(label);
    }
}

// ============================================================
// DACollapseWidget —— 手风琴 / 非手风琴
// ============================================================
static QWidget *createCollapseContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("手风琴模式同一时间只展开一个面板；非手风琴模式可同时展开多个。\n"
                       "activeNames 为展开面板的 name 列表，展开/折叠带高度动画。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *row = new QHBoxLayout;

    // ---- 手风琴模式 ----
    auto *accordionBox = new QGroupBox(QStringLiteral("手风琴模式（只展开一个）"), w);
    auto *accordionLayout = new QVBoxLayout(accordionBox);
    auto *accordion = new DA::DACollapseWidget(accordionBox);
    accordion->setAccordion(true);
    addTextContent(accordion->addItem(QStringLiteral("账号信息"), QStringLiteral("account")),
                   { QStringLiteral("用户名：admin"), QStringLiteral("角色：管理员") });
    addTextContent(accordion->addItem(QStringLiteral("安全设置"), QStringLiteral("security")),
                   { QStringLiteral("两步验证：已开启"), QStringLiteral("最近登录：2026-11-23") });
    addTextContent(accordion->addItem(QStringLiteral("通知偏好"), QStringLiteral("notify")),
                   { QStringLiteral("邮件通知：开启"), QStringLiteral("站内信：开启") });
    accordion->setActiveNames({ QStringLiteral("account") });
    accordionLayout->addWidget(accordion);
    accordionLayout->addStretch(1);

    // ---- 非手风琴模式 ----
    auto *multiBox = new QGroupBox(QStringLiteral("非手风琴模式（可同时展开多个）"), w);
    auto *multiLayout = new QVBoxLayout(multiBox);
    auto *multi = new DA::DACollapseWidget(multiBox);
    multi->setAccordion(false);
    addTextContent(multi->addItem(QStringLiteral("基本信息"), QStringLiteral("basic")),
                   { QStringLiteral("名称：示例工程"), QStringLiteral("类型：桌面应用") });
    addTextContent(multi->addItem(QStringLiteral("高级设置"), QStringLiteral("advanced")),
                   { QStringLiteral("缓存目录：默认"), QStringLiteral("日志级别：Info") });
    addTextContent(multi->addItem(QStringLiteral("关于"), QStringLiteral("about")),
                   { QStringLiteral("DAWidgets 全控件演示") });
    multi->setActiveNames({ QStringLiteral("basic"), QStringLiteral("advanced") });
    multiLayout->addWidget(multi);
    multiLayout->addStretch(1);

    row->addWidget(accordionBox, 1);
    row->addWidget(multiBox, 1);
    v->addLayout(row);

    auto *log = new QLabel(QStringLiteral("当前展开: account / basic, advanced"), w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(accordion, &DA::DACollapseWidget::activeNamesChanged, log,
                     [log](const QStringList &names) {
                         log->setText(QStringLiteral("手风琴展开: %1")
                                          .arg(names.isEmpty() ? QStringLiteral("(无)")
                                                               : names.join(QStringLiteral(", "))));
                     });
    QObject::connect(multi, &DA::DACollapseWidget::activeNamesChanged, log,
                     [log](const QStringList &names) {
                         log->setText(QStringLiteral("非手风琴展开: %1")
                                          .arg(names.isEmpty() ? QStringLiteral("(无)")
                                                               : names.join(QStringLiteral(", "))));
                     });

    return w;
}

// ============================================================
// DACollapseItem —— 禁用 / 自定义标题 / 关闭动画
// ============================================================
static QWidget *createCollapseItemContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("DACollapseItem 支持禁用、自定义标题控件、关闭动画，也可脱离容器单独使用。\n"
                       "禁用项点击无响应；自定义标题控件会替代默认标题文本。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *collapse = new DA::DACollapseWidget(w);
    addTextContent(collapse->addItem(QStringLiteral("基本信息"), QStringLiteral("basic")),
                   { QStringLiteral("这是默认标题，点击标题栏展开/折叠。"),
                     QStringLiteral("内容区可以直接用 contentLayout() 追加控件。") });

    auto *disabled = collapse->addItem(QStringLiteral("高级设置（禁用）"), QStringLiteral("disabled"));
    disabled->setDisabled(true);
    addTextContent(disabled, { QStringLiteral("禁用后标题栏不可点击。") });

    auto *custom = collapse->addItem(QString(), QStringLiteral("custom"));
    auto *title = new QLabel(QStringLiteral("★ 自定义标题控件（QLabel）"));
    title->setStyleSheet(QStringLiteral("color:#409eff;font-weight:bold;"));
    custom->setTitleWidget(title);
    addTextContent(custom, { QStringLiteral("setTitleWidget() 用任意控件替代默认标题文本。") });

    collapse->setActiveNames({ QStringLiteral("basic") });
    v->addWidget(collapse);

    auto *noAnimDesc = new QLabel(QStringLiteral("关闭动画（animationEnabled = false）："), w);
    v->addWidget(noAnimDesc);
    auto *noAnim = new DA::DACollapseWidget(w);
    noAnim->setAccordion(true);
    auto *instant = noAnim->addItem(QStringLiteral("无动画展开"), QStringLiteral("instant"));
    instant->setAnimationEnabled(false);
    addTextContent(instant, { QStringLiteral("展开/折叠瞬时完成，适合性能敏感场景。") });
    noAnim->setActiveNames({ QStringLiteral("instant") });
    v->addWidget(noAnim);

    return w;
}

// ============================================================
// 场景列表（tab 与截图导出共用）
// ============================================================
QVector< DemoScene > collapseScenes()
{
    return {
        { QStringLiteral("collapse"), QStringLiteral("DACollapseWidget — 折叠面板（手风琴/非手风琴）"),
          createCollapseContent, 0 },
        { QStringLiteral("collapse-item"), QStringLiteral("DACollapseItem — 折叠条目（禁用/自定义标题）"),
          createCollapseItemContent, 0 },
    };
}
