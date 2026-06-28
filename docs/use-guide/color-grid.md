# 颜色网格与调色板

`DAColorGridWidget` 提供基础颜色网格，`DAColorPaletteGridWidget` 在其基础上构建 Office 风格调色板（主色 + 自动生成浅/深色行）。

## DAColorGridWidget

### 核心特性

- ✅ **网格布局**：QGridLayout + QButtonGroup
- ✅ **可配置列数**：`setColumnCount(int)`
- ✅ **自定义颜色列表**：`setColorList(QList<QColor>)`
- ✅ **可选中**：`setColorCheckable(bool)`
- ✅ **尺寸可调**：`setColorIconSize(QSize)`、`setSpacing(int)`

### 用法

```cpp
#include "DAColorGridWidget.h"

auto *grid = new DA::DAColorGridWidget(parent);

// 设置列数
grid->setColumnCount(8);

// 设置颜色列表
grid->setColorList({
    Qt::red, Qt::green, Qt::blue, Qt::yellow,
    Qt::cyan, Qt::magenta, Qt::gray, Qt::black
});

// 启用选中
grid->setColorCheckable(true);

// 监听点击
QObject::connect(grid, &DA::DAColorGridWidget::colorClicked,
    [](const QColor& color) {
        qDebug() << "点击颜色:" << color;
    });
```

## DAColorPaletteGridWidget

### 核心特性

- ✅ **Office 风格**：主色行 + 自动生成浅色行 + 深色行
- ✅ **浅/深色因子**：`setFactor(QList<int>)` 控制生成行数和深浅
- ✅ **组合两个网格**：内部使用两个 `DAColorGridWidget`

### 用法

```cpp
#include "DAColorPaletteGridWidget.h"

auto *palette = new DA::DAColorPaletteGridWidget(parent);

// 设置主色列表
palette->setColorList({
    QColor("#C00000"), QColor("#FF0000"), QColor("#FFC000"),
    QColor("#FFFF00"), QColor("#92D050"), QColor("#00B050"),
    QColor("#00B0F0"), QColor("#0070C0"), QColor("#002060"), QColor("#7030A0")
});

// 浅/深色生成因子（百分比，正值=浅色，负值=深色）
palette->setFactor({ 80, 60, 40, -25, -50 });

// 监听颜色点击
QObject::connect(palette, &DA::DAColorPaletteGridWidget::colorClicked,
    [](const QColor& color) {
        qDebug() << "选择颜色:" << color.name();
    });
```

### 工作原理

```text
┌─────────────────────────────────────────┐
│  主色行 (原始颜色)                       │
│  ■ ■ ■ ■ ■ ■ ■ ■ ■ ■                   │
├─────────────────────────────────────────┤
│  浅色行 1 (+80%)                        │
│  ░ ░ ░ ░ ░ ░ ░ ░ ░ ░                   │
├─────────────────────────────────────────┤
│  ...更多浅色行...                        │
├─────────────────────────────────────────┤
│  深色行 1 (-25%)                        │
│  ▓ ▓ ▓ ▓ ▓ ▓ ▓ ▓ ▓ ▓                   │
└─────────────────────────────────────────┘
```

## 核心方法

### DAColorGridWidget

| 方法 | 说明 |
|------|------|
| `setColumnCount(int)` | 设置列数 |
| `setColorList(QList<QColor>)` | 设置颜色列表 |
| `setColorIconSize(QSize)` | 设置颜色块尺寸 |
| `setSpacing(int)` | 设置间距 |
| `setColorCheckable(bool)` | 启用/禁用选中 |
| `currentCheckedColor()` | 获取当前选中颜色 |

### DAColorPaletteGridWidget

| 方法 | 说明 |
|------|------|
| `setColorList(QList<QColor>)` | 设置主色列表 |
| `setFactor(QList<int>)` | 设置浅/深色因子 |
| `setColorIconSize(QSize)` | 设置颜色块尺寸 |

## 参考

- [颜色菜单与拾取按钮](color-menu.md)
