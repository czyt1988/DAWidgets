#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DABrushEditWidget.h>
#include <DABrushStyleComboBox.h>
#include <DACollapseItem.h>
#include <DACollapseWidget.h>
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
static QWidget *createBrushStyleComboContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("QComboBox 子类，每个条目用色块图标展示 Qt::BrushStyle。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *row = new QHBoxLayout;
    row->addWidget(new QLabel(QStringLiteral("当前样式:")));
    auto *combo = new DA::DABrushStyleComboBox(w);
    row->addWidget(combo, 1);
    v->addLayout(row);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(combo, &DA::DABrushStyleComboBox::currentBrushStyleChanged,
        [log](Qt::BrushStyle s) {
            log->setText(QStringLiteral("样式变更: ")
                         + DA::DABrushStyleComboBox::brushStyleToString(s));
        });

    return w;
}

// ============================================================
// DABrushEditWidget —— 画刷编辑器
// ============================================================
static QWidget *createBrushEditContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("颜色按钮 + 画刷样式下拉框的组合编辑器。\n"
                       "支持纯色、图案、渐变、纹理四种填充模式。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DABrushEditWidget(w);
    edit->setCurrentBrush(QBrush(QColor(64, 158, 255)));
    v->addWidget(edit);

    auto *log = new QLabel(QStringLiteral("brush: color=#409eff, style=SolidPattern"), w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DABrushEditWidget::brushChanged,
        [log](const QBrush &b) { log->setText(QStringLiteral("brush: ") + brushToString(b)); });

    return w;
}

// ============================================================
// DAPenStyleComboBox —— 画笔样式下拉框
// ============================================================
static QWidget *createPenStyleComboContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("QComboBox 子类，每个条目用线条图标展示 Qt::PenStyle。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *row = new QHBoxLayout;
    row->addWidget(new QLabel(QStringLiteral("当前样式:")));
    auto *combo = new DA::DAPenStyleComboBox(w);
    row->addWidget(combo, 1);
    v->addLayout(row);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(combo, &DA::DAPenStyleComboBox::currentPenStyleChanged,
        [log](Qt::PenStyle s) {
            log->setText(QStringLiteral("样式变更: ")
                         + DA::DAPenStyleComboBox::penStyleToString(s));
        });

    return w;
}

// ============================================================
// DAPenEditWidget —— 画笔编辑器
// ============================================================
static QWidget *createPenEditContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("颜色按钮 + 线条样式下拉框 + 线宽调节，编辑 QPen 的全部属性。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAPenEditWidget(QPen(QColor(255, 0, 0), 2, Qt::SolidLine), w);
    v->addWidget(edit);

    auto *log = new QLabel(QStringLiteral("pen: color=#ff0000, style=SolidLine, width=2"), w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAPenEditWidget::penChanged,
        [log](const QPen &p) { log->setText(QStringLiteral("pen: ") + penToString(p)); });

    return w;
}

// ============================================================
// DAShapeEditPannelWidget —— 形状编辑面板
// ============================================================
static QWidget *createShapeEditContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("组合 DABrushEditWidget（背景填充）和 DAPenEditWidget（边框画笔）的复合编辑器。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAShapeEditPannelWidget(w);
    v->addWidget(edit);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(edit, &DA::DAShapeEditPannelWidget::backgroundBrushChanged,
        [log](const QBrush &b) { log->setText(QStringLiteral("背景: ") + brushToString(b)); });
    QObject::connect(edit, &DA::DAShapeEditPannelWidget::borderPenChanged,
        [log](const QPen &p) { log->setText(QStringLiteral("边框: ") + penToString(p)); });

    return w;
}

// ============================================================
// DAFontEditPannelWidget —— 字体编辑面板
// ============================================================
static QWidget *createFontEditContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("字体族 + 字号 + 加粗/斜体/下划线 + 字体颜色的组合编辑器。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *edit = new DA::DAFontEditPannelWidget(w);
    v->addWidget(edit);

    // 预览标签
    auto *preview = new QLabel(QStringLiteral("预览：The quick brown fox 简体中文 12345"), w);
    preview->setStyleSheet(QStringLiteral("font-size: 16px; padding: 8px;"));
    v->addWidget(preview);

    QObject::connect(edit, &DA::DAFontEditPannelWidget::currentFontChanged,
        [preview](const QFont &f) { preview->setFont(f); });
    QObject::connect(edit, &DA::DAFontEditPannelWidget::currentFontColorChanged,
        [preview](const QColor &c) {
            preview->setStyleSheet(QStringLiteral("font-size: 16px; padding: 8px; color: %1;").arg(c.name()));
        });

    return w;
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

    auto *collapse = new DA::DACollapseWidget(content);
    collapse->addItem(QStringLiteral("DABrushStyleComboBox — 画刷样式下拉框"), QStringLiteral("1"))
        ->setContentWidget(createBrushStyleComboContent(collapse));
    collapse->addItem(QStringLiteral("DABrushEditWidget — 画刷编辑器"), QStringLiteral("2"))
        ->setContentWidget(createBrushEditContent(collapse));
    collapse->addItem(QStringLiteral("DAPenStyleComboBox — 画笔样式下拉框"), QStringLiteral("3"))
        ->setContentWidget(createPenStyleComboContent(collapse));
    collapse->addItem(QStringLiteral("DAPenEditWidget — 画笔编辑器"), QStringLiteral("4"))
        ->setContentWidget(createPenEditContent(collapse));
    collapse->addItem(QStringLiteral("DAShapeEditPannelWidget — 形状编辑面板"), QStringLiteral("5"))
        ->setContentWidget(createShapeEditContent(collapse));
    collapse->addItem(QStringLiteral("DAFontEditPannelWidget — 字体编辑面板"), QStringLiteral("6"))
        ->setContentWidget(createFontEditContent(collapse));
    collapse->setActiveNames({ QStringLiteral("1") });

    v->addWidget(collapse);
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
