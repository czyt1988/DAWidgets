# 颜色菜单与拾取按钮

`DAColorMenu` 提供颜色选择菜单（主题色板 + 自定义色 + None），`DAColorPickerButton` 是自绘颜色块的颜色拾取按钮。

## DAColorMenu

### 核心特性

- ✅ **完整菜单结构**：标题 → 主题色板 → "None" Action → "Custom Color" Action → 自定义色网格
- ✅ **None 颜色**：`enableNoneColorAction` 控制 None 选项
- ✅ **自定义色管理**：最多 10 个自定义色，FIFO 淘汰
- ✅ **暴露子控件**：`colorPaletteGridWidget()`、`customColorsWidget()` 可直接访问

### 用法

```cpp
#include "DAColorMenu.h"

auto *menu = new DA::DAColorMenu(parent);

// 启用 None 颜色选项
menu->enableNoneColorAction(true);

// 监听颜色选择
QObject::connect(menu, &DA::DAColorMenu::selectedColor,
    [](const QColor& color) {
        if (color.isValid()) {
            qDebug() << "选择颜色:" << color.name();
        } else {
            qDebug() << "选择 None";
        }
    });

// 弹出菜单
menu->exec(QCursor::pos());
```

## DAColorPickerButton

### 核心特性

- ✅ **自绘颜色块**：在按钮上绘制当前颜色
- ✅ **无效色显示**：无效色（`QColor()`）显示红色斜杠
- ✅ **两种模式**：`WithColorMenu`（内置颜色菜单）/ `NoColorMenu`（纯显示）
- ✅ **静态方法**：`paintNoneColor()` 可供其他控件绘制 None 色块

### 用法

```cpp
#include "DAColorPickerButton.h"

// 方式一：带内置颜色菜单
auto *btn = new DA::DAColorPickerButton(parent);
btn->setColorToolButtonStyle(DA::DAColorPickerButton::ColorToolButtonStyle::WithColorMenu);
btn->setColor(Qt::red);

// 监听颜色变化
QObject::connect(btn, &DA::DAColorPickerButton::colorChanged,
    [](const QColor& color) {
        qDebug() << "颜色变化:" << color.name();
    });

// 方式二：纯显示（不弹菜单）
auto *display = new DA::DAColorPickerButton(parent);
display->setColorToolButtonStyle(DA::DAColorPickerButton::ColorToolButtonStyle::NoColorMenu);
display->setColor(Qt::blue);
```

### 自定义颜色菜单

```cpp
auto *btn = new DA::DAColorPickerButton(parent);

// 获取或创建颜色菜单
DA::DAColorMenu *menu = btn->colorMenu();
if (!menu) {
    menu = btn->createColorMenu();
}

// 自定义主题色板
menu->colorPaletteGridWidget()->setColorList({
    QColor("#FF6B6B"), QColor("#4ECDC4"), QColor("#45B7D1"),
    QColor("#FFA07A"), QColor("#98D8C8"), QColor("#F7DC6F")
});
```

## 核心方法

### DAColorMenu

| 方法 | 说明 |
|------|------|
| `enableNoneColorAction(bool)` | 启用/禁用 None 选项 |
| `colorPaletteGridWidget()` | 获取主题色板控件 |
| `customColorsWidget()` | 获取自定义色网格控件 |

### DAColorPickerButton

| 方法 | 说明 |
|------|------|
| `color()` / `setColor(QColor)` | 获取/设置颜色 |
| `setColorToolButtonStyle(Style)` | 设置按钮模式 |
| `colorMenu()` | 获取颜色菜单（可能为 null） |
| `createColorMenu()` | 创建颜色菜单 |
| `margins` | 颜色块边距 |
| `paintNoneColor(QPainter, rect)` | 静态方法，绘制 None 色块 |

## 参考

- [颜色网格与调色板](color-grid.md)
- [画笔样式编辑](pen-edit.md)
