# Qt5/Qt6 Compatibility Guide

This document introduces the compatibility macros and strategies used in the DAWidgets project to handle API differences between Qt5 and Qt6.

## Core Principle

DAWidgets is a **Qt5/Qt6 dual-compatible** library — this is a hard requirement. All version differences are handled through compatibility macros in `DAQt5Qt6Compat.h` and `#if QT_VERSION` guards.

## Common API Differences and Compatibility Macros

### 1. QMouseEvent / QTouchEvent pos() Method

In Qt6, `pos()` is deprecated, replaced by `position().toPoint()`:

```cpp
// ❌ Directly using pos() — Qt6 warns deprecation
QPoint pos = event->pos();

// ✅ Using the compatibility macro
#include "DAQt5Qt6Compat.h"
QPoint pos = Qt5Qt6Compat_QXXEvent_Pos(event);
```

Related macros:

| Macro | Qt5 Expansion | Qt6 Expansion |
|-------|---------------|---------------|
| `Qt5Qt6Compat_QXXEvent_Pos(ptr)` | `ptr->pos()` | `ptr->position().toPoint()` |
| `Qt5Qt6Compat_QXXEvent_x(ptr)` | `ptr->pos().x()` | `ptr->position().x()` |
| `Qt5Qt6Compat_QXXEvent_y(ptr)` | `ptr->pos().y()` | `ptr->position().y()` |

### 2. QFontMetrics width() Method

From Qt 5.11+, `width()` is deprecated, replaced by `horizontalAdvance()`:

```cpp
// ❌ Directly using width() — newer versions warn
int w = fontMetrics.width(text);

// ✅ Using the compatibility macro
int w = Qt5Qt6Compat_fontMetrics_width(fontMetrics, text);
```

| Macro | Qt < 5.12 Expansion | Qt >= 5.12 Expansion |
|-------|---------------------|----------------------|
| `Qt5Qt6Compat_fontMetrics_width(fm, str)` | `fm.width(str)` | `fm.horizontalAdvance(str)` |

### 3. QButtonGroup Signal Changes

In Qt6, `buttonClicked(int)` is renamed to `idClicked(int)`:

```cpp
// ❌ Direct usage — incompatible between Qt5/Qt6
connect(group, QOverload<int>::of(&QButtonGroup::buttonClicked), ...);  // Qt5
connect(group, &QButtonGroup::idClicked, ...);  // Qt6

// ✅ Using the compatibility macro
Qt5Qt6Compat_Connect_ButtonGroupClicked_int(group, MyClass::onButtonClicked);
```

### 4. QString::split SkipEmptyParts

```cpp
// ❌ Qt5 uses QString::SkipEmptyParts, Qt6 uses Qt::SkipEmptyParts
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    auto parts = str.split(",", QString::SkipEmptyParts);
#else
    auto parts = str.split(",", Qt::SkipEmptyParts);
#endif
```

### 5. QApplication::globalStrut()

Qt6 removed `globalStrut()`, requiring conditional compilation to work around.

## Include Strategy

!!! info "Include Strategy"
    `DAQt5Qt6Compat.h` is **included only in .cpp files on an as-needed basis** by default, avoiding propagation through public headers. This ensures that when data-workbench integrates DAWidgets, it does not passively inherit new compatibility macros.

```cpp
// DASelectComboBox.cpp
#include "DASelectComboBox.h"
#include "DAQt5Qt6Compat.h"  // Include only in cpp

void DASelectComboBox::resizeEvent(QResizeEvent* event)
{
    // ...
}
```

## Maintenance Contract

!!! warning "Maintenance Contract"
    The macro definitions in `DAQt5Qt6Compat.h` must be **verbatim identical** to the corresponding macros in the data-workbench project's `DAGlobals.h`. Both sides are guarded by `#ifndef`. You must not independently modify the macro definitions in this file — if changes are needed, you must synchronously update data-workbench's `DAGlobals.h`.

## Checklist

When adding new Qt version difference handling:

- [ ] Add the compatibility macro in `DAQt5Qt6Compat.h` (with `#ifndef` guard)
- [ ] Use `#if QT_VERSION < QT_VERSION_CHECK(...)` for version checks
- [ ] Include the compatibility header only in .cpp files
- [ ] Synchronously update data-workbench's `DAGlobals.h`
- [ ] Verify compilation under both Qt5 and Qt6

## Related Documents

- [Coding Standards](coding-standards.md)
- [PIMPL Pattern](pimpl-pattern.md)
