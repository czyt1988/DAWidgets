#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSize>
#include <QVBoxLayout>

#include <DAColorGridWidget.h>
#include <DAColorMenu.h>
#include <DAColorPaletteGridWidget.h>
#include <DAColorPickerButton.h>

#include "TabPages.h"

// ============================================================
// DAColorPickerButton —— 颜色选择按钮
// ============================================================
static QWidget *createColorPickerContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("自绘颜色块按钮。WithColorMenu 模式（默认）点击弹出 DAColorMenu 颜色面板；\n"
                       "NoColorMenu 模式仅发射 colorClicked 信号，由外部处理。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    // ---- WithColorMenu（默认） ----
    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel(QStringLiteral("WithColorMenu:")));
    auto *btn1 = new DA::DAColorPickerButton(DA::DAColorPickerButton::WithColorMenu, w);
    btn1->setColor(QColor(64, 158, 255));
    row1->addWidget(btn1);
    row1->addStretch();
    v->addLayout(row1);

    // ---- NoColorMenu ----
    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel(QStringLiteral("NoColorMenu:")));
    auto *btn2 = new DA::DAColorPickerButton(DA::DAColorPickerButton::NoColorMenu, w);
    btn2->setColor(QColor(103, 194, 58));
    row2->addWidget(btn2);
    row2->addStretch();
    v->addLayout(row2);

    // ---- 无颜色（invalid color 渲染为红色斜线） ----
    auto *row3 = new QHBoxLayout;
    row3->addWidget(new QLabel(QStringLiteral("无颜色 (QColor()):")));
    auto *btn3 = new DA::DAColorPickerButton(w);
    btn3->setColor(QColor());  // invalid
    row3->addWidget(btn3);
    row3->addStretch();
    v->addLayout(row3);

    auto *log = new QLabel(w);
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

    return w;
}

// ============================================================
// DAColorPaletteGridWidget —— Office 风格调色板
// ============================================================
static QWidget *createPaletteContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("顶部一行为标准色，下方自动生成 5 行深浅变体（lighter/darker）。\n"
                       "点击任意色块发射 colorClicked 信号。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *palette = new DA::DAColorPaletteGridWidget(w);
    palette->setColorCheckable(true);
    palette->setColorIconSize(QSize(20, 20));
    v->addWidget(palette);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(palette, &DA::DAColorPaletteGridWidget::colorClicked,
        [log](const QColor &c) {
            log->setText(QStringLiteral("选中颜色: ") + c.name());
        });

    return w;
}

// ============================================================
// DAColorGridWidget —— 基础颜色网格
// ============================================================
static QWidget *createColorGridContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("使用 QGridLayout + QButtonGroup 排列色块按钮。\n"
                       "可设置列数、间距、图标大小，支持 checkable 互斥选中。\n"
                       "此处显式打开键盘导航（方向键移动、Enter 取色）与棋盘格底（最后一块为 50% 透明）。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *grid = new DA::DAColorGridWidget(w);
    grid->setColumnCount(6);
    grid->setColorList({
        QColor(255, 0, 0), QColor(255, 165, 0), QColor(255, 255, 0),
        QColor(0, 128, 0), QColor(0, 0, 255), QColor(128, 0, 128),
        QColor(255, 192, 203), QColor(135, 206, 235), QColor(144, 238, 144),
        QColor(255, 20, 147), QColor(0, 255, 255), QColor(64, 158, 255, 128),
    });
    grid->setColorCheckable(true);
    grid->setKeyboardNavigationEnabled(true);
    grid->setAlphaBackgroundEnabled(true);
    auto *gridRow = new QHBoxLayout;
    gridRow->addWidget(grid);
    gridRow->addStretch(1);
    v->addLayout(gridRow);

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(grid, &DA::DAColorGridWidget::colorClicked,
        [log](const QColor &c) {
            log->setText(QStringLiteral("点击: ") + c.name());
        });

    return w;
}

// ============================================================
// DAColorMenu —— 颜色选择菜单（通过按钮弹出）
// ============================================================
static QWidget *createColorMenuContent(QWidget *parent)
{
    auto *w = new QWidget(parent);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(8, 4, 8, 4);

    auto *desc = new QLabel(
        QStringLiteral("DAColorMenu 是一个 QMenu，内嵌调色板 + 无颜色 + 自定义颜色 + 历史记录。\n"
                       "点击下方按钮弹出菜单体验。此处显式打开 alpha 滑杆、最近 5 色（方向键可选）与恢复默认色，"
                       "三项默认关闭。"), w);
    desc->setWordWrap(true);
    v->addWidget(desc);

    auto *btn = new QPushButton(QStringLiteral("点击弹出 DAColorMenu"), w);
    v->addWidget(btn);

    auto *menu = new DA::DAColorMenu(QStringLiteral("选择颜色"), w);
    menu->enableNoneColorAction(true);
    menu->setAlphaEnabled(true);
    menu->setRecentColors({ QColor(64, 158, 255), QColor(103, 194, 58), QColor(245, 108, 108),
                            QColor(230, 162, 60) });
    menu->setDefaultColor(QColor(64, 158, 255));

    auto *log = new QLabel(w);
    log->setStyleSheet(QStringLiteral("color:#909399;"));
    v->addWidget(log);

    QObject::connect(btn, &QPushButton::clicked, [menu, btn]() {
        menu->exec(btn->mapToGlobal(QPoint(0, btn->height())));
    });
    QObject::connect(menu, &DA::DAColorMenu::selectedColor,
        [log](const QColor &c) {
            if (c.isValid())
                log->setText(QStringLiteral("选中颜色: %1（alpha %2）").arg(c.name()).arg(c.alpha()));
            else
                log->setText(QStringLiteral("选中: 无颜色"));
        });
    QObject::connect(menu, &DA::DAColorMenu::alphaChanged, log, [log](int a) {
        log->setText(QStringLiteral("alpha: %1（%2%）").arg(a).arg(qRound(a * 100.0 / 255.0)));
    });

    return w;
}

// ============================================================
// 场景列表（tab 与截图导出共用）
// ============================================================
QVector< DemoScene > colorScenes()
{
    return {
        { QStringLiteral("color-picker-button"),
          QStringLiteral("DAColorPickerButton — 自绘颜色选择按钮"), createColorPickerContent, 0 },
        { QStringLiteral("color-palette"),
          QStringLiteral("DAColorPaletteGridWidget — Office 风格调色板"), createPaletteContent, 0 },
        { QStringLiteral("color-grid"), QStringLiteral("DAColorGridWidget — 基础颜色网格"),
          createColorGridContent, 0 },
        { QStringLiteral("color-menu"), QStringLiteral("DAColorMenu — 颜色选择弹出菜单"),
          createColorMenuContent, 0 },
    };
}
