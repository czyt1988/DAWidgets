#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DAChip.h>
#include <DAChipsEdit.h>
#include <DAFlowLayout.h>
#include <DASelectComboBox.h>

// ============================================================
// DAChip —— 单个可关闭标签
// ============================================================
static QGroupBox *createChipGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAChip — 单个可关闭标签"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("DAChip 是一个独立的标签控件，带文本和可选的关闭按钮。\n"
                       "点击标签触发 clicked()，点击关闭按钮触发 closeRequested()。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // ---- 不可关闭的标签 ----
    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel(QStringLiteral("不可关闭:")));
    auto *chip1 = new DA::DAChip(QStringLiteral("只读标签"), box);
    chip1->setClosable(false);
    row1->addWidget(chip1);
    row1->addStretch();
    v->addLayout(row1);

    // ---- 可关闭的标签 ----
    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel(QStringLiteral("可关闭:")));
    auto *chip2 = new DA::DAChip(QStringLiteral("点 × 关闭"), box);
    chip2->setClosable(true);
    row2->addWidget(chip2);
    row2->addStretch();
    v->addLayout(row2);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(chip1, &DA::DAChip::clicked,
        [log]() { log->setText(QStringLiteral("← clicked: 只读标签被点击")); });
    QObject::connect(chip2, &DA::DAChip::closeRequested,
        [log]() { log->setText(QStringLiteral("← closeRequested: 可关闭标签被关闭")); });

    return box;
}

// ============================================================
// DAChipsEdit —— 流式标签输入框
// ============================================================
static QGroupBox *createChipsEditGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAChipsEdit — 流式标签输入框"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("输入文本后按回车/逗号/分号提交标签，Backspace 删除最后一个标签。\n"
                       "支持去重、最大数量限制、自定义分隔符。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAChipsEdit(box);
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
    auto *lbl = new QLabel(QStringLiteral("tags: Qt, C++"), box);
    auto *addBtn = new QPushButton(QStringLiteral("追加 'QWidget'"), box);
    auto *clearBtn = new QPushButton(QStringLiteral("清空"), box);
    row->addWidget(lbl, 1);
    row->addWidget(addBtn);
    row->addWidget(clearBtn);
    v->addLayout(row);

    // ---- 选项区 ----
    auto *dupChk = new QCheckBox(QStringLiteral("允许重复标签"), box);
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

    return box;
}

// ============================================================
// DAFlowLayout —— 流式布局
// ============================================================
static QGroupBox *createFlowLayoutGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAFlowLayout — 流式自动换行布局"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("DAFlowLayout 将子控件按行排列，空间不足时自动换行。\n"
                       "setTrailingExpanding(true) 可让末尾控件填满剩余宽度（DAChipsEdit 内部使用此特性）。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // 用 DAFlowLayout 放置一组按钮，演示自动换行
    auto *container = new QWidget(box);
    auto *flow = new DA::DAFlowLayout(container, 0, 6, 6);
    for (int i = 1; i <= 10; ++i) {
        auto *btn = new QPushButton(QStringLiteral("按钮 %1").arg(i), container);
        flow->addWidget(btn);
    }
    v->addWidget(container);

    return box;
}

// ============================================================
// DASelectComboBox —— 多标签下拉选择器
// ============================================================
static QGroupBox *createSelectComboBoxGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DASelectComboBox — 多标签下拉选择器"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("对标 Element UI el-select (multiple + filterable + allow-create)。\n"
                       "输入文本实时过滤候选列表，回车选中；不在列表中的文本可直接创建。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *cb = new DA::DASelectComboBox(box);
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
    auto *lbl = new QLabel(QStringLiteral("tags: Qt, C++"), box);
    auto *growChk = new QCheckBox(QStringLiteral("随标签增高"), box);
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

    return box;
}

// ============================================================
// 入口
// ============================================================
QWidget *createSelectorTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    v->addWidget(createChipGroup(content));
    v->addWidget(createChipsEditGroup(content));
    v->addWidget(createFlowLayoutGroup(content));
    v->addWidget(createSelectComboBoxGroup(content));
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
