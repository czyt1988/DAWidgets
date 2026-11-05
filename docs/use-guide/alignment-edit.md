# 对齐编辑器

`DATextAlignmentEditWidget`、`DAAligmentEditWidget` 和 `DAAligmentPositionEditWidget` 提供对齐方式的编辑控件。`DATextAlignmentEditWidget` 用两行互斥按钮组分别设置水平/垂直对齐，`DAAligmentEditWidget` 通过单个按钮组选择水平/垂直对齐，`DAAligmentPositionEditWidget` 通过九宫格选择对齐位置。

## DATextAlignmentEditWidget

### 核心特性

- ✅ 水平一组（左/居中/右）+ 垂直一组（顶端/居中/底端），两组独立互斥
- ✅ 值以 `Qt::Alignment` 组合表示（可同时「右对齐 + 顶端对齐」）
- ✅ 信号通知：`alignmentChanged(Qt::Alignment)`，值未变化不发信号
- ✅ 默认值为左对齐 + 垂直居中（`AlignLeft | AlignVCenter`）

### 用法

```cpp
#include "DATextAlignmentEditWidget.h"

auto *edit = new DA::DATextAlignmentEditWidget(parent);

// 设置当前对齐（只给水平位/垂直位时，另一半保持当前值）
edit->setAlignment(Qt::AlignRight | Qt::AlignTop);

// 监听变化
QObject::connect(edit, &DA::DATextAlignmentEditWidget::alignmentChanged,
    [](Qt::Alignment align) {
        qDebug() << "对齐方式:" << align;
    });
```

与 `DAAligmentEditWidget` 的区别：后者是单选语义（水平垂直共用一个按钮组，选中「居中」即两个方向都居中）；本控件把水平与垂直拆成两组，可以分别设置。

## DAAligmentEditWidget

### 核心特性

- ✅ 水平对齐：左/右/居中
- ✅ 垂直对齐：上/下/居中
- ✅ 信号通知：`alignmentChanged(Qt::Alignment)`

### 用法

```cpp
#include "DAAligmentEditWidget.h"

auto *alignEdit = new DA::DAAligmentEditWidget(parent);

// 设置当前对齐
alignEdit->setCurrentAlignment(Qt::AlignLeft | Qt::AlignVCenter);

// 监听变化
QObject::connect(alignEdit, &DA::DAAligmentEditWidget::alignmentChanged,
    [](Qt::Alignment align) {
        qDebug() << "对齐方式:" << align;
    });
```

## DAAligmentPositionEditWidget

### 核心特性

- ✅ 九宫格位置选择
- ✅ 静态方法 `acceptAligment()` 判断是否接受某对齐方式
- ✅ 信号通知：`aligmentPositionChanged()`

### 用法

```cpp
#include "DAAligmentPositionEditWidget.h"

auto *posEdit = new DA::DAAligmentPositionEditWidget(parent);

// 设置当前位置
posEdit->setAligmentPosition(Qt::AlignTop | Qt::AlignLeft);

// 监听变化
QObject::connect(posEdit, &DA::DAAligmentPositionEditWidget::aligmentPositionChanged,
    [](Qt::Alignment align) {
        qDebug() << "位置对齐:" << align;
    });
```

## 核心方法

### DAAligmentEditWidget

| 方法 | 说明 |
|------|------|
| `getCurrentAlignment()` | 获取当前对齐方式 |
| `setCurrentAlignment(Qt::Alignment)` | 设置对齐方式 |

### DAAligmentPositionEditWidget

| 方法 | 说明 |
|------|------|
| `getAligmentPosition()` | 获取当前位置 |
| `setAligmentPosition(Qt::Alignment)` | 设置位置 |
| `acceptAligment(Qt::Alignment)` | 静态方法，判断是否接受 |

## 参考

- [DAPropertyItemWidget 属性项](property-item.md)
