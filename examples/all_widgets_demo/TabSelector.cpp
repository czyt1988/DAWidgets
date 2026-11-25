#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <DAChip.h>
#include <DAChipsEdit.h>
#include <DAFlowLayout.h>
#include <DASelectComboBox.h>

#include "TabPages.h"

// ============================================================
// DAChip —— 单个可关闭标签
// ============================================================
static QWidget *createChipContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("DAChip 是一个独立的标签控件，带文本和可选的关闭按钮。\n"
                       "点击标签触发 clicked()，点击关闭按钮触发 closeRequested()。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // ---- 不可关闭的标签 ----
    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel(QStringLiteral("不可关闭:")));
    auto *chip1 = new DA::DAChip(QStringLiteral("只读标签"), w);
    chip1->setClosable(false);
    row1->addWidget(chip1);
    row1->addStretch();
    v->addLayout(row1);

    // ---- 可关闭的标签 ----
    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel(QStringLiteral("可关闭:")));
    auto *chip2 = new DA::DAChip(QStringLiteral("点 × 关闭"), w);
    chip2->setClosable(true);
    row2->addWidget(chip2);
    row2->addStretch();
    v->addLayout(row2);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(chip1, &DA::DAChip::clicked,
        [log]() { log->setText(QStringLiteral("← clicked: 只读标签被点击")); });
    QObject::connect(chip2, &DA::DAChip::closeRequested,
        [log]() { log->setText(QStringLiteral("← closeRequested: 可关闭标签被关闭")); });

    return w;
}

// ============================================================
// DAChipsEdit —— 流式标签输入框
// ============================================================
static QWidget *createChipsEditContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("输入文本后按回车/逗号/分号提交标签，Backspace 删除最后一个标签。\n"
                       "支持去重、最大数量限制、自定义分隔符。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAChipsEdit(w);
    edit->setPlaceholder(QStringLiteral("输入标签后回车提交（逗号/分号也可）"));
    edit->setSeparators(QList<DA::DAChipsEdit::Separator>{
        DA::DAChipsEdit::Separator::Comma,
        DA::DAChipsEdit::Separator::Enter,
        DA::DAChipsEdit::Separator::Semicolon,
    });
    edit->setTags({ QStringLiteral("Qt"), QStringLiteral("C++") });
    v->addWidget(edit);

    // ---- 控制区 ----
    auto *row = new QHBoxLayout;
    auto *lbl = new QLabel(QStringLiteral("tags: Qt, C++"), w);
    auto *addBtn = new QPushButton(QStringLiteral("追加 'QWidget'"), w);
    auto *clearBtn = new QPushButton(QStringLiteral("清空"), w);
    row->addWidget(lbl, 1);
    row->addWidget(addBtn);
    row->addWidget(clearBtn);
    v->addLayout(row);

    // ---- 选项区 ----
    auto *dupChk = new QCheckBox(QStringLiteral("允许重复标签"), w);
    dupChk->setChecked(false);
    edit->setAllowDuplicate(false);
    v->addWidget(dupChk);

    QObject::connect(edit, &DA::DAChipsEdit::tagsChanged,
        [lbl](const QStringList &t) {
            lbl->setText(QStringLiteral("tags: ") + t.join(QStringLiteral(", ")));
        });
    QObject::connect(edit, &DA::DAChipsEdit::tagAdded,
        [](const QString &t) { qDebug("DAChipsEdit added: %s", qPrintable(t)); });
    QObject::connect(edit, &DA::DAChipsEdit::tagRemoved,
        [](const QString &t) { qDebug("DAChipsEdit removed: %s", qPrintable(t)); });
    QObject::connect(addBtn, &QPushButton::clicked,
        [edit] { edit->appendTag(QStringLiteral("QWidget")); });
    QObject::connect(clearBtn, &QPushButton::clicked, edit, &DA::DAChipsEdit::clear);
    QObject::connect(dupChk, &QCheckBox::toggled, edit, &DA::DAChipsEdit::setAllowDuplicate);

    // ---- 粘贴拆分 / 超长省略 / 数量计数（三项默认关闭，这里显式打开） ----
    auto *advTitle = new QLabel(QStringLiteral("粘贴拆分 / 超长省略 / 数量计数"), w);
    QFont titleFont = advTitle->font();
    titleFont.setBold(true);
    advTitle->setFont(titleFont);
    v->addWidget(advTitle);

    auto *advDesc = new QLabel(
        QStringLiteral("Ctrl+V / Shift+Insert 与下面的“模拟粘贴”都走 pasteText()：按分隔符集合"
                       "（Enter = 换行）一次拆成多个标签。\n"
                       "超过 96px 的标签以 … 结尾、悬浮显示全文；右下角显示 n / maxCount。"),
        w);
    advDesc->setWordWrap(true);
    v->addWidget(advDesc);

    auto *advanced = new DA::DAChipsEdit(w);
    advanced->setPlaceholder(QStringLiteral("粘贴一整列标签，或输入后回车"));
    advanced->setSeparators(QList<DA::DAChipsEdit::Separator>{
        DA::DAChipsEdit::Separator::Comma,
        DA::DAChipsEdit::Separator::Semicolon,
        DA::DAChipsEdit::Separator::Enter,
    });
    advanced->setPasteSplitEnabled(true);
    advanced->setTagElideWidth(96);
    advanced->setShowCount(true);
    advanced->setMaxCount(5);
    advanced->setTags({ QStringLiteral("this-is-a-very-long-tag-name") });
    v->addWidget(advanced);

    auto *advRow = new QHBoxLayout;
    auto *advLog = new QLabel(QStringLiteral("pasteSplit: —   pasteRejected: —"), w);
    auto *pasteBtn = new QPushButton(QStringLiteral("模拟粘贴 “红, 绿; 蓝\\n黄”"), w);
    auto *advClearBtn = new QPushButton(QStringLiteral("清空"), w);
    advRow->addWidget(advLog, 1);
    advRow->addWidget(pasteBtn);
    advRow->addWidget(advClearBtn);
    v->addLayout(advRow);

    QObject::connect(pasteBtn, &QPushButton::clicked, [advanced] {
        advanced->pasteText(QStringLiteral("红, 绿; 蓝\n黄"));
    });
    QObject::connect(advClearBtn, &QPushButton::clicked, advanced, &DA::DAChipsEdit::clear);
    QObject::connect(advanced, &DA::DAChipsEdit::pasteSplit, [advLog](const QStringList &tokens) {
        advLog->setText(QStringLiteral("pasteSplit: 拆出 %1 项").arg(tokens.size()));
    });
    QObject::connect(advanced, &DA::DAChipsEdit::pasteRejected, [advLog](const QString &reason) {
        advLog->setText(QStringLiteral("pasteRejected: %1（maxCount = 已达上限）").arg(reason));
    });

    return w;
}

// ============================================================
// DAFlowLayout —— 流式布局
// ============================================================
static QWidget *createFlowLayoutContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("DAFlowLayout 将子控件按行排列，空间不足时自动换行。\n"
                       "setTrailingExpanding(true) 可让末尾控件填满剩余宽度（DAChipsEdit 内部使用此特性）。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // 用 DAFlowLayout 放置一组按钮，演示自动换行
    auto *container = new QWidget(w);
    auto *flow = new DA::DAFlowLayout(container, 0, 6, 6);
    for (int i = 1; i <= 10; ++i) {
        auto *btn = new QPushButton(QStringLiteral("按钮 %1").arg(i), container);
        flow->addWidget(btn);
    }
    v->addWidget(container);

    return w;
}

// ============================================================
// DASelectComboBox —— 多标签下拉选择器
// ============================================================
static QWidget *createSelectComboBoxContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("对标 Element UI el-select (multiple + filterable + allow-create)。\n"
                       "输入文本实时过滤候选列表，回车选中；不在列表中的文本可直接创建。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *cb = new DA::DASelectComboBox(w);
    cb->setPlaceholder(QStringLiteral("请选择标签（输入过滤，回车选中；不在列表中可直接创建）"));
    cb->setOptions({
        QStringLiteral("Qt"), QStringLiteral("C++"), QStringLiteral("Python"),
        QStringLiteral("QML"), QStringLiteral("Qt Quick"), QStringLiteral("JavaScript"),
        QStringLiteral("Rust"), QStringLiteral("Go"), QStringLiteral("Kotlin"),
    });
    cb->setAllowCreate(true);
    cb->setTags({ QStringLiteral("Qt"), QStringLiteral("C++") });
    v->addWidget(cb);

    auto *row = new QHBoxLayout;
    auto *lbl = new QLabel(QStringLiteral("tags: Qt, C++"), w);
    auto *growChk = new QCheckBox(QStringLiteral("随标签增高"), w);
    growChk->setChecked(true);
    row->addWidget(lbl, 1);
    row->addWidget(growChk);
    v->addLayout(row);

    QObject::connect(cb, &DA::DASelectComboBox::tagsChanged,
        [lbl](const QStringList &t) {
            lbl->setText(QStringLiteral("tags: ") + t.join(QStringLiteral(", ")));
        });
    QObject::connect(growChk, &QCheckBox::toggled,
        [cb](bool on) {
            cb->setHeightMode(on ? DA::DASelectComboBox::HeightMode::GrowWithTags
                                 : DA::DASelectComboBox::HeightMode::FixedHeightScrollable);
        });
    QObject::connect(cb, &DA::DASelectComboBox::optionSelected,
        [](const QString &t) { qDebug("DASelectComboBox selected: %s", qPrintable(t)); });
    QObject::connect(cb, &DA::DASelectComboBox::created,
        [](const QString &t) { qDebug("DASelectComboBox created: %s", qPrintable(t)); });

    return w;
}

// ============================================================
// 场景列表（tab 与截图导出共用）
// ============================================================
QVector< DemoScene > selectorScenes()
{
    return {
        { QStringLiteral("chip"), QStringLiteral("DAChip — 单个可关闭标签"), createChipContent, 0 },
        { QStringLiteral("chips-edit"), QStringLiteral("DAChipsEdit — 流式标签输入框"),
          createChipsEditContent, 0 },
        { QStringLiteral("flow-layout"), QStringLiteral("DAFlowLayout — 流式自动换行布局"),
          createFlowLayoutContent, 0 },
        { QStringLiteral("select-combobox"), QStringLiteral("DASelectComboBox — 多标签下拉选择器"),
          createSelectComboBoxContent, 0 },
    };
}
