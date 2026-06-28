# 画笔样式编辑

`DAPenStyleComboBox` 是 Qt::PenStyle 选择下拉框，`DAPenEditWidget` 组合颜色 + 线型 + 线宽的完整画笔编辑器。

## DAPenStyleComboBox

### 核心特性

- ✅ **所有线型**：实线、虚线、点线、点划线等
- ✅ **图标预览**：下拉项带线型图标
- ✅ **静态工具**：`penStyleToString()`、`drawPenStyle()`、`generatePenIcon()`

### 用法

```cpp
#include "DAPenStyleComboBox.h"

auto *combo = new DA::DAPenStyleComboBox(parent);

// 设置当前线型
combo->setCurrentPenStyle(Qt::DashLine);

// 获取当前线型
Qt::PenStyle style = combo->getCurrentPenStyle();

// 监听变化
QObject::connect(combo, &DA::DAPenStyleComboBox::currentPenStyleChanged,
    [](Qt::PenStyle style) {
        qDebug() << "线型:" << style;
    });
```

## DAPenEditWidget

### 核心特性

- ✅ **颜色 + 线型 + 线宽**：完整画笔属性编辑
- ✅ **线型文本可隐藏**：`setLineStyleTextVisible(bool)`

### 用法

```cpp
#include "DAPenEditWidget.h"

auto *penEdit = new DA::DAPenEditWidget(parent);

// 设置当前画笔
QPen pen(Qt::blue, 2, Qt::DashDotLine);
penEdit->setCurrentPen(pen);

// 获取当前画笔
QPen currentPen = penEdit->getCurrentPen();

// 监听变化
QObject::connect(penEdit, &DA::DAPenEditWidget::penChanged,
    [](const QPen& pen) {
        qDebug() << "画笔:" << pen.color() << pen.width() << pen.style();
    });
```

## 核心方法

### DAPenStyleComboBox

| 方法 | 说明 |
|------|------|
| `getCurrentPenStyle()` | 获取当前线型 |
| `setCurrentPenStyle(Qt::PenStyle)` | 设置线型 |
| `setPen(QPen)` | 从 QPen 设置线型和颜色 |
| `setPenColor(QColor)` | 设置线型图标颜色 |
| `setPenLineWidth(int)` | 设置线型图标线宽 |
| `setStyleTextVisible(bool)` | 是否显示线型文本 |

### DAPenEditWidget

| 方法 | 说明 |
|------|------|
| `getCurrentPen()` | 获取当前画笔 |
| `setCurrentPen(QPen)` | 设置画笔 |
| `setLineStyleTextVisible(bool)` | 线型下拉框文本可见性 |

## 参考

- [画刷样式编辑](brush-edit.md)
- [颜色菜单与拾取按钮](color-menu.md)
