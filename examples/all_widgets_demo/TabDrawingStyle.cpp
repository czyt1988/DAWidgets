#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DABrushEditWidget.h>
#include <DABrushStyleComboBox.h>
#include <DAFontEditPannelWidget.h>
#include <DAPenEditWidget.h>
#include <DAPenStyleComboBox.h>
#include <DAShapeEditPannelWidget.h>

// 辅助：将 QBrush 描述为文字
static QString brushToString(const QBrush &b)
{
    if (b.style() == Qt::NoBrush)
        return QStringLiteral("NoBrush");
    QString s = QStringLiteral("color=%1, style=%2")
                    .arg(b.color().name())
                    .arg(DA::DABrushStyleComboBox::brushStyleToString(b.style()));
    return s;
}

// 辅助：将 QPen 描述为文字
static QString penToString(const QPen &p)
{
    return QStringLiteral("color=%1, style=%2, width=%3")
        .arg(p.color().name())
        .arg(DA::DAPenStyleComboBox::penStyleToString(p.style()))
        .arg(p.width());
}

// ============================================================
// DABrushStyleComboBox —— 画刷样式下拉框
// ============================================================
static QGroupBox *createBrushStyleComboGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DABrushStyleComboBox — 画刷样式下拉框"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("QComboBox 子类，每个条目用色块图标展示 Qt::BrushStyle。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *row = new QHBoxLayout;
    row->addWidget(new QLabel(QStringLiteral("当前样式:")));
    auto *combo = new DA::DABrushStyleComboBox(box);
    row->addWidget(combo, 1);
    v->addLayout(row);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(combo, &DA::DABrushStyleComboBox::currentBrushStyleChanged,
        [log](Qt::BrushStyle s) {
            log->setText(QStringLiteral("样式变更: ")
                         + DA::DABrushStyleComboBox::brushStyleToString(s));
        });

    return box;
}

// ============================================================
// DABrushEditWidget —— 画刷编辑器
// ============================================================
static QGroupBox *createBrushEditGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DABrushEditWidget — 画刷编辑器"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("颜色按钮 + 画刷样式下拉框的组合编辑器。\n"
                       "支持纯色、图案、渐变、纹理四种填充模式。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DABrushEditWidget(box);
    edit->setCurrentBrush(QBrush(QColor(64, 158, 255)));
    v->addWidget(edit);

    auto *log = new QLabel(QStringLiteral("brush: color=#409eff, style=SolidPattern"), box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DABrushEditWidget::brushChanged,
        [log](const QBrush &b) { log->setText(QStringLiteral("brush: ") + brushToString(b)); });

    return box;
}

// ============================================================
// DAPenStyleComboBox —— 画笔样式下拉框
// ============================================================
static QGroupBox *createPenStyleComboGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAPenStyleComboBox — 画笔样式下拉框"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("QComboBox 子类，每个条目用线条图标展示 Qt::PenStyle。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *row = new QHBoxLayout;
    row->addWidget(new QLabel(QStringLiteral("当前样式:")));
    auto *combo = new DA::DAPenStyleComboBox(box);
    row->addWidget(combo, 1);
    v->addLayout(row);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(combo, &DA::DAPenStyleComboBox::currentPenStyleChanged,
        [log](Qt::PenStyle s) {
            log->setText(QStringLiteral("样式变更: ")
                         + DA::DAPenStyleComboBox::penStyleToString(s));
        });

    return box;
}

// ============================================================
// DAPenEditWidget —— 画笔编辑器
// ============================================================
static QGroupBox *createPenEditGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAPenEditWidget — 画笔编辑器"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("颜色按钮 + 线条样式下拉框 + 线宽调节，编辑 QPen 的全部属性。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAPenEditWidget(QPen(QColor(255, 0, 0), 2, Qt::SolidLine), box);
    v->addWidget(edit);

    auto *log = new QLabel(QStringLiteral("pen: color=#ff0000, style=SolidLine, width=2"), box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAPenEditWidget::penChanged,
        [log](const QPen &p) { log->setText(QStringLiteral("pen: ") + penToString(p)); });

    return box;
}

// ============================================================
// DAShapeEditPannelWidget —— 形状编辑面板
// ============================================================
static QGroupBox *createShapeEditGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAShapeEditPannelWidget — 形状编辑面板"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("组合 DABrushEditWidget（背景填充）和 DAPenEditWidget（边框画笔）的复合编辑器。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAShapeEditPannelWidget(box);
    v->addWidget(edit);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAShapeEditPannelWidget::backgroundBrushChanged,
        [log](const QBrush &b) { log->setText(QStringLiteral("背景: ") + brushToString(b)); });
    QObject::connect(edit, &DA::DAShapeEditPannelWidget::borderPenChanged,
        [log](const QPen &p) { log->setText(QStringLiteral("边框: ") + penToString(p)); });

    return box;
}

// ============================================================
// DAFontEditPannelWidget —— 字体编辑面板
// ============================================================
static QGroupBox *createFontEditGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAFontEditPannelWidget — 字体编辑面板"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("字体族 + 字号 + 加粗/斜体/下划线 + 字体颜色的组合编辑器。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAFontEditPannelWidget(box);
    v->addWidget(edit);

    // 预览标签
    auto *preview = new QLabel(QStringLiteral("预览：The quick brown fox 简体中文 12345"), box);
    preview->setStyleSheet(QStringLiteral("font-size: 16px; padding: 8px;"));
    v->addWidget(preview);

    QObject::connect(edit, &DA::DAFontEditPannelWidget::currentFontChanged,
        [preview](const QFont &f) { preview->setFont(f); });
    QObject::connect(edit, &DA::DAFontEditPannelWidget::currentFontColorChanged,
        [preview](const QColor &c) {
            preview->setStyleSheet(QStringLiteral("font-size: 16px; padding: 8px; color: %1;").arg(c.name()));
        });

    return box;
}

// ============================================================
// 入口
// ============================================================
QWidget *createDrawingStyleTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    v->addWidget(createBrushStyleComboGroup(content));
    v->addWidget(createBrushEditGroup(content));
    v->addWidget(createPenStyleComboGroup(content));
    v->addWidget(createPenEditGroup(content));
    v->addWidget(createShapeEditGroup(content));
    v->addWidget(createFontEditGroup(content));
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
