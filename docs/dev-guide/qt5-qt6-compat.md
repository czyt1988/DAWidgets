# Qt5/Qt6 兼容指引

本文档介绍 DAWidgets 项目中处理 Qt5 与 Qt6 API 差异的兼容宏和策略。

## 核心原则

DAWidgets 是 **Qt5/Qt6 双兼容**库，这是硬性要求。所有版本差异通过 `DAQt5Qt6Compat.h` 中的兼容宏和 `#if QT_VERSION` 守卫处理。

## 常见 API 差异与兼容宏

### 1. QMouseEvent / QTouchEvent 的 pos() 方法

Qt6 中 `pos()` 被废弃，改用 `position().toPoint()`：

```cpp
// ❌ 直接使用 pos() — Qt6 会警告废弃
QPoint pos = event->pos();

// ✅ 使用兼容宏
#include "DAQt5Qt6Compat.h"
QPoint pos = Qt5Qt6Compat_QXXEvent_Pos(event);
```

相关宏：

| 宏 | Qt5 展开 | Qt6 展开 |
|----|---------|---------|
| `Qt5Qt6Compat_QXXEvent_Pos(ptr)` | `ptr->pos()` | `ptr->position().toPoint()` |
| `Qt5Qt6Compat_QXXEvent_x(ptr)` | `ptr->pos().x()` | `ptr->position().x()` |
| `Qt5Qt6Compat_QXXEvent_y(ptr)` | `ptr->pos().y()` | `ptr->position().y()` |

### 2. QFontMetrics 的 width() 方法

Qt 5.11+ 中 `width()` 被废弃，改用 `horizontalAdvance()`：

```cpp
// ❌ 直接使用 width() — 新版本会警告
int w = fontMetrics.width(text);

// ✅ 使用兼容宏
int w = Qt5Qt6Compat_fontMetrics_width(fontMetrics, text);
```

| 宏 | Qt < 5.12 展开 | Qt ≥ 5.12 展开 |
|----|---------------|---------------|
| `Qt5Qt6Compat_fontMetrics_width(fm, str)` | `fm.width(str)` | `fm.horizontalAdvance(str)` |

### 3. QButtonGroup 信号变化

Qt6 中 `buttonClicked(int)` 被重命名为 `idClicked(int)`：

```cpp
// ❌ 直接使用 — Qt5/Qt6 不兼容
connect(group, QOverload<int>::of(&QButtonGroup::buttonClicked), ...);  // Qt5
connect(group, &QButtonGroup::idClicked, ...);  // Qt6

// ✅ 使用兼容宏
Qt5Qt6Compat_Connect_ButtonGroupClicked_int(group, MyClass::onButtonClicked);
```

### 4. QString::split 的 SkipEmptyParts

```cpp
// ❌ Qt5 用 QString::SkipEmptyParts，Qt6 用 Qt::SkipEmptyParts
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    auto parts = str.split(",", QString::SkipEmptyParts);
#else
    auto parts = str.split(",", Qt::SkipEmptyParts);
#endif
```

### 5. QApplication::globalStrut()

Qt6 移除了 `globalStrut()`，需要条件编译规避。

## 引入策略

!!! info "引入策略"
    `DAQt5Qt6Compat.h` **默认仅在 .cpp 文件中按需 include**，避免通过公共头传递，从而 data-workbench 引入 DAWidgets 时不会被动引入新的兼容宏。

```cpp
// DASelectComboBox.cpp
#include "DASelectComboBox.h"
#include "DAQt5Qt6Compat.h"  // 仅在 cpp 中引入

void DASelectComboBox::resizeEvent(QResizeEvent* event)
{
    // ...
}
```

## 维护契约

!!! warning "维护契约"
    `DAQt5Qt6Compat.h` 中的宏定义必须与 data-workbench 项目的 `DAGlobals.h` 中对应宏**逐字一致**。两侧均以 `#ifndef` 守卫。不可独立修改本文件中的宏定义——若需变更，需同步更新 data-workbench 的 `DAGlobals.h`。

## 检查清单

添加新的 Qt 版本差异处理时：

- [ ] 在 `DAQt5Qt6Compat.h` 中添加兼容宏（`#ifndef` 守卫）
- [ ] 使用 `#if QT_VERSION < QT_VERSION_CHECK(...)` 进行版本判断
- [ ] 仅在 .cpp 文件中引入兼容头
- [ ] 同步更新 data-workbench 的 `DAGlobals.h`
- [ ] 在 Qt5 和 Qt6 下分别编译验证

## 相关文档

- [编码规范](coding-standards.md)
- [PIMPL 模式](pimpl-pattern.md)
