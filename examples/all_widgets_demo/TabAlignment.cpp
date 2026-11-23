#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <DAAligmentEditWidget.h>
#include <DAAligmentPositionEditWidget.h>
#include <DATextAlignmentEditWidget.h>

#include "TabPages.h"

// ============================================================
// DATextAlignmentEditWidget —— 水平/垂直两组独立的文本对齐编辑器
// ============================================================
static QWidget *createTextAlignmentContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("两行互斥按钮组：水平（左/居中/右）与垂直（顶端/居中/底端）可分别设置。\n"
                       "值以 Qt::Alignment 组合表示（如右对齐 + 顶端对齐），点击即发射 alignmentChanged。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *row = new QHBoxLayout;
    row->addWidget(new QLabel(QStringLiteral("文本对齐:")));
    auto *edit = new DA::DATextAlignmentEditWidget(w);
    row->addWidget(edit);
    row->addStretch(1);
    v->addLayout(row);

    auto *log = new QLabel(QStringLiteral("alignment: 0x%1").arg(static_cast<int>(edit->alignment()), 0, 16), w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DATextAlignmentEditWidget::alignmentChanged,
        [log](Qt::Alignment al) {
            log->setText(QStringLiteral("alignment: %1 (0x%2)")
                             .arg(static_cast<int>(al))
                             .arg(static_cast<int>(al), 0, 16));
        });

    return w;
}

// ============================================================
// DAAligmentEditWidget —— 对齐方式编辑器
// ============================================================
static QWidget *createAlignmentContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("通过按钮组选择 Qt::Alignment 值（水平/垂直对齐方式）。\n"
                       "点击按钮切换对齐方式，发射 alignmentChanged 信号。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *row = new QHBoxLayout;
    row->addWidget(new QLabel(QStringLiteral("对齐方式:")));
    auto *edit = new DA::DAAligmentEditWidget(w);
    row->addWidget(edit, 1);
    v->addLayout(row);

    auto *log = new QLabel(QStringLiteral("alignment: AlignLeft | AlignVCenter"), w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAAligmentEditWidget::alignmentChanged,
        [log](Qt::Alignment al) {
            log->setText(QStringLiteral("alignment: %1 (0x%2)")
                             .arg(static_cast<int>(al))
                             .arg(static_cast<int>(al), 0, 16));
        });

    return w;
}

// ============================================================
// DAAligmentPositionEditWidget —— 九宫格对齐位置编辑器
// ============================================================
static QWidget *createAlignmentPositionContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("九宫格按钮，选择内容在区域中的对齐位置（左上、居中、右下等）。\n"
                       "支持的对齐组合参见 acceptAligment() 静态方法。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAAligmentPositionEditWidget(w);
    v->addWidget(edit);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAAligmentPositionEditWidget::aligmentPositionChanged,
        [log](Qt::Alignment al) {
            log->setText(QStringLiteral("position: %1 (0x%2)")
                             .arg(static_cast<int>(al))
                             .arg(static_cast<int>(al), 0, 16));
        });

    return w;
}

// ============================================================
// 场景列表（tab 与截图导出共用）
// ============================================================
QVector< DemoScene > alignmentScenes()
{
    return {
        { QStringLiteral("text-alignment-edit"),
          QStringLiteral("DATextAlignmentEditWidget — 水平/垂直独立对齐"), createTextAlignmentContent, 0 },
        { QStringLiteral("alignment-edit"), QStringLiteral("DAAligmentEditWidget — 对齐方式编辑器"),
          createAlignmentContent, 0 },
        { QStringLiteral("alignment-position-edit"),
          QStringLiteral("DAAligmentPositionEditWidget — 九宫格对齐位置"), createAlignmentPositionContent,
          0 },
    };
}
