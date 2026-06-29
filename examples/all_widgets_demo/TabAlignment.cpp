#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DAAligmentEditWidget.h>
#include <DAAligmentPositionEditWidget.h>

// ============================================================
// DAAligmentEditWidget —— 对齐方式编辑器
// ============================================================
static QGroupBox *createAlignmentGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAAligmentEditWidget — 对齐方式编辑器"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("通过按钮组选择 Qt::Alignment 值（水平/垂直对齐方式）。\n"
                       "点击按钮切换对齐方式，发射 alignmentChanged 信号。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *row = new QHBoxLayout;
    row->addWidget(new QLabel(QStringLiteral("对齐方式:")));
    auto *edit = new DA::DAAligmentEditWidget(box);
    row->addWidget(edit, 1);
    v->addLayout(row);

    auto *log = new QLabel(QStringLiteral("alignment: AlignLeft | AlignVCenter"), box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAAligmentEditWidget::alignmentChanged,
        [log](Qt::Alignment al) {
            log->setText(QStringLiteral("alignment: %1 (0x%2)")
                             .arg(static_cast<int>(al))
                             .arg(static_cast<int>(al), 0, 16));
        });

    return box;
}

// ============================================================
// DAAligmentPositionEditWidget —— 九宫格对齐位置编辑器
// ============================================================
static QGroupBox *createAlignmentPositionGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAAligmentPositionEditWidget — 九宫格对齐位置"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("九宫格按钮，选择内容在区域中的对齐位置（左上、居中、右下等）。\n"
                       "支持的对齐组合参见 acceptAligment() 静态方法。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAAligmentPositionEditWidget(box);
    v->addWidget(edit);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAAligmentPositionEditWidget::aligmentPositionChanged,
        [log](Qt::Alignment al) {
            log->setText(QStringLiteral("position: %1 (0x%2)")
                             .arg(static_cast<int>(al))
                             .arg(static_cast<int>(al), 0, 16));
        });

    return box;
}

// ============================================================
// 入口
// ============================================================
QWidget *createAlignmentTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    v->addWidget(createAlignmentGroup(content));
    v->addWidget(createAlignmentPositionGroup(content));
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
