# 对齐编辑器

`DAAligmentEditWidget` 和 `DAAligmentPositionEditWidget` 提供对齐方式的编辑控件。前者通过按钮组选择水平/垂直对齐，后者通过九宫格选择对齐位置。

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
