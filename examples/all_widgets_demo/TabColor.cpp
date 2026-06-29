#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include <DAColorGridWidget.h>
#include <DAColorMenu.h>
#include <DAColorPaletteGridWidget.h>
#include <DAColorPickerButton.h>

// ============================================================
// DAColorPickerButton —— 颜色选择按钮
// ============================================================
static QGroupBox *createColorPickerGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAColorPickerButton — 自绘颜色选择按钮"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("自绘颜色块按钮。WithColorMenu 模式（默认）点击弹出 DAColorMenu 颜色面板；\n"
                       "NoColorMenu 模式仅发射 colorClicked 信号，由外部处理。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // ---- WithColorMenu（默认） ----
    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel(QStringLiteral("WithColorMenu:")));
    auto *btn1 = new DA::DAColorPickerButton(DA::DAColorPickerButton::WithColorMenu, box);
    btn1->setColor(QColor(64, 158, 255));
    row1->addWidget(btn1);
    row1->addStretch();
    v->addLayout(row1);

    // ---- NoColorMenu ----
    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel(QStringLiteral("NoColorMenu:")));
    auto *btn2 = new DA::DAColorPickerButton(DA::DAColorPickerButton::NoColorMenu, box);
    btn2->setColor(QColor(103, 194, 58));
    row2->addWidget(btn2);
    row2->addStretch();
    v->addLayout(row2);

    // ---- 无颜色（invalid color 渲染为红色斜线） ----
    auto *row3 = new QHBoxLayout;
    row3->addWidget(new QLabel(QStringLiteral("无颜色 (QColor()):")));
    auto *btn3 = new DA::DAColorPickerButton(box);
    btn3->setColor(QColor());  // invalid
    row3->addWidget(btn3);
    row3->addStretch();
    v->addLayout(row3);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(btn1, &DA::DAColorPickerButton::colorChanged,
        [log](const QColor &c) {
            log->setText(QStringLiteral("WithColorMenu 颜色变更: ") + c.name());
        });
    QObject::connect(btn2, &DA::DAColorPickerButton::colorClicked,
        [log](const QColor &c) {
            log->setText(QStringLiteral("NoColorMenu 被点击, 当前颜色: ") + c.name());
        });

    return box;
}

// ============================================================
// DAColorPaletteGridWidget —— Office 风格调色板
// ============================================================
static QGroupBox *createPaletteGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAColorPaletteGridWidget — Office 风格调色板"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("顶部一行为标准色，下方自动生成 5 行深浅变体（lighter/darker）。\n"
                       "点击任意色块发射 colorClicked 信号。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *palette = new DA::DAColorPaletteGridWidget(box);
    palette->setColorCheckable(true);
    v->addWidget(palette);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(palette, &DA::DAColorPaletteGridWidget::colorClicked,
        [log](const QColor &c) {
            log->setText(QStringLiteral("选中颜色: ") + c.name());
        });

    return box;
}

// ============================================================
// DAColorGridWidget —— 基础颜色网格
// ============================================================
static QGroupBox *createColorGridGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAColorGridWidget — 基础颜色网格"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("使用 QGridLayout + QButtonGroup 排列色块按钮。\n"
                       "可设置列数、间距、图标大小，支持 checkable 互斥选中。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *grid = new DA::DAColorGridWidget(box);
    grid->setColumnCount(6);
    grid->setColorList({
        QColor(255, 0, 0), QColor(255, 165, 0), QColor(255, 255, 0),
        QColor(0, 128, 0), QColor(0, 0, 255), QColor(128, 0, 128),
        QColor(255, 192, 203), QColor(135, 206, 235), QColor(144, 238, 144),
        QColor(255, 20, 147), QColor(0, 255, 255), QColor(255, 215, 0),
    });
    grid->setColorCheckable(true);
    v->addWidget(grid);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(grid, &DA::DAColorGridWidget::colorClicked,
        [log](const QColor &c) {
            log->setText(QStringLiteral("点击: ") + c.name());
        });

    return box;
}

// ============================================================
// DAColorMenu —— 颜色选择菜单（通过按钮弹出）
// ============================================================
static QGroupBox *createColorMenuGroup(QWidget *parent)
{
    auto *box = new QGroupBox(QStringLiteral("DAColorMenu — 颜色选择弹出菜单"), parent);
    auto *v = new QVBoxLayout(box);

    auto *desc = new QLabel(
        QStringLiteral("DAColorMenu 是一个 QMenu，内嵌调色板 + 无颜色 + 自定义颜色 + 历史记录。\n"
                       "点击下方按钮弹出菜单体验。"), box);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *btn = new QPushButton(QStringLiteral("点击弹出 DAColorMenu"), box);
    v->addWidget(btn);

    auto *menu = new DA::DAColorMenu(QStringLiteral("选择颜色"), box);
    menu->enableNoneColorAction(true);

    auto *log = new QLabel(box);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(btn, &QPushButton::clicked, [menu, btn]() {
        menu->exec(btn->mapToGlobal(QPoint(0, btn->height())));
    });
    QObject::connect(menu, &DA::DAColorMenu::selectedColor,
        [log](const QColor &c) {
            if (c.isValid())
                log->setText(QStringLiteral("选中颜色: ") + c.name());
            else
                log->setText(QStringLiteral("选中: 无颜色"));
        });

    return box;
}

// ============================================================
// 入口
// ============================================================
QWidget *createColorTab(QWidget *parent)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *v = new QVBoxLayout(content);
    v->setSpacing(12);

    v->addWidget(createColorPickerGroup(content));
    v->addWidget(createPaletteGroup(content));
    v->addWidget(createColorGridGroup(content));
    v->addWidget(createColorMenuGroup(content));
    v->addStretch(1);

    scroll->setWidget(content);
    return scroll;
}
