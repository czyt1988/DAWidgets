#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
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

    auto *log = new QLabel(QStringLiteral("当前展开: account / basic, advanced"), w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));

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

    // ---- 第三列：键盘 + 懒加载 + 变体 ----
    auto *extraBox    = new QGroupBox(QStringLiteral("键盘 / 懒加载 / 变体"), w);
    auto *extraLayout = new QVBoxLayout(extraBox);

    auto *kbTitle = new QLabel(QStringLiteral("键盘导航（默认关闭，此处已打开）"), extraBox);
    QFont kbFont = kbTitle->font();
    kbFont.setBold(true);
    kbTitle->setFont(kbFont);
    extraLayout->addWidget(kbTitle);

    auto *kbDesc = new QLabel(QStringLiteral("Tab 聚焦标题、↑/↓ 换面板、Enter/Space 开合。"), extraBox);
    kbDesc->setWordWrap(true);
    extraLayout->addWidget(kbDesc);

    auto *kb = new DA::DACollapseWidget(extraBox);
    kb->setKeyboardNavigation(true);
    addTextContent(kb->addItem(QStringLiteral("账号信息"), QStringLiteral("k1")),
                   { QStringLiteral("Tab 到这里，↑/↓ 切换面板") });
    addTextContent(kb->addItem(QStringLiteral("安全设置"), QStringLiteral("k2")),
                   { QStringLiteral("Enter 或 Space 展开/折叠") });
    addTextContent(kb->addItem(QStringLiteral("通知偏好"), QStringLiteral("k3")),
                   { QStringLiteral("首尾不循环，禁用项会被跳过") });
    kb->setActiveNames({ QStringLiteral("k1") });
    extraLayout->addWidget(kb);

    auto *lazyTitle = new QLabel(QStringLiteral("懒加载（expandRequested）"), extraBox);
    lazyTitle->setFont(kbFont);
    extraLayout->addWidget(lazyTitle);

    auto *lazyDesc = new QLabel(
        QStringLiteral("首次展开时预约内容，400ms 后调用 refreshContentHeight() 刷新高度。"), extraBox);
    lazyDesc->setWordWrap(true);
    extraLayout->addWidget(lazyDesc);

    auto *lazy     = new DA::DACollapseWidget(extraBox);
    auto *lazyItem = lazy->addItem(QStringLiteral("异步加载的内容"), QStringLiteral("lazy"));
    auto *lazyLog  = new QLabel(QStringLiteral("尚未展开"), extraBox);
    lazyLog->setStyleSheet(QStringLiteral("color:#909399;"));
    QObject::connect(lazy, &DA::DACollapseWidget::expandRequested, lazy,
                     [lazyItem, lazyLog](const QString &name) {
                         lazyLog->setText(QStringLiteral("expandRequested: ") + name);
                         lazyItem->setContentWidget(new QLabel(QStringLiteral("加载中…"), lazyItem));
                         lazyItem->refreshContentHeight();
                         QTimer::singleShot(400, lazyItem, [lazyItem, lazyLog]() {
                             auto *label = new QLabel(
                                 QStringLiteral("加载完成：这里可以替换成耗时构建的表格或图表。"), lazyItem);
                             label->setWordWrap(true);
                             lazyItem->setContentWidget(label);
                             lazyItem->refreshContentHeight();
                             lazyLog->setText(QStringLiteral("内容已加载，高度已刷新"));
                         });
                     });
    extraLayout->addWidget(lazy);
    extraLayout->addWidget(lazyLog);

    auto *variantTitle = new QLabel(QStringLiteral("外观变体"), extraBox);
    variantTitle->setFont(kbFont);
    extraLayout->addWidget(variantTitle);

    auto makeVariant = [extraBox](DA::DACollapseWidget::Variant variant, const QString &caption) {
        auto *box = new QWidget(extraBox);
        auto *lay = new QVBoxLayout(box);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(2);
        auto *captionLabel = new QLabel(caption, box);
        captionLabel->setStyleSheet(QStringLiteral("color:#909399;"));
        auto *collapse = new DA::DACollapseWidget(box);
        collapse->setVariant(variant);
        addTextContent(collapse->addItem(QStringLiteral("面板 A"), QStringLiteral("a")),
                       { QStringLiteral("变体只改间距与留白，不改 activeNames。") });
        addTextContent(collapse->addItem(QStringLiteral("面板 B"), QStringLiteral("b")),
                       { QStringLiteral("Card 的边框由调用方 qss 提供。") });
        collapse->setActiveNames({ QStringLiteral("a") });
        lay->addWidget(captionLabel);
        lay->addWidget(collapse);
        return box;
    };

    extraLayout->addWidget(makeVariant(DA::DACollapseWidget::Variant::Default,
                                       QStringLiteral("Default（现状）")));
    extraLayout->addWidget(makeVariant(DA::DACollapseWidget::Variant::Borderless,
                                       QStringLiteral("Borderless（贴边）")));
    auto *cardBox = makeVariant(DA::DACollapseWidget::Variant::Card,
                                QStringLiteral("Card（8px 间距 + qss 边框）"));
    cardBox->setStyleSheet(QStringLiteral(
        "DA--DACollapseItem[daVariant=\"card\"] { border:1px solid #EBEEF5; border-radius:4px; }"));
    extraLayout->addWidget(cardBox);
    extraLayout->addStretch(1);

    row->addWidget(accordionBox, 1);
    row->addWidget(multiBox, 1);
    row->addWidget(extraBox, 1);
    v->addLayout(row);
    v->addWidget(log);

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
        { QStringLiteral("collapse"), QStringLiteral("DACollapseWidget — 折叠面板（手风琴/键盘/懒加载/变体）"),
          createCollapseContent, 0 },
        { QStringLiteral("collapse-item"), QStringLiteral("DACollapseItem — 折叠条目（禁用/自定义标题）"),
          createCollapseItemContent, 0 },
    };
}
