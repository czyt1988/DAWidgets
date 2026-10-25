---
title: DAWidgets Introduction
description: DAWidgets is a Qt QWidget supplement library inspired by Element UI, bringing Web-style component interactions to desktop applications.
keywords: [DAWidgets, Qt widget library, Element UI, QWidget, desktop controls]
---

# DAWidgets — Qt Widget Supplement Library

DAWidgets is a Qt **QWidget** UI supplement library inspired by [Element UI](https://element.eleme.io/). Its goal is to fill gaps in the native Qt Widgets control set by bringing Web-style component interactions to desktop applications. All public classes are prefixed with `DA` and placed in the `DA` namespace.

## Why DAWidgets?

Qt's native Widgets provide a basic control set, but there are many gaps in modern UI interactions: multi-tag selectors, flow-layout tag inputs, collapsible panels, color/pen editors are either missing or require significant manual coding. DAWidgets fills these gaps.

| Feature | Description |
|---------|-------------|
| **Element UI Style** | Components designed after Element UI, consistent interaction logic |
| **Qt5/Qt6 Compatible** | Single codebase supporting both Qt5 and Qt6 via compatibility macros |
| **PIMPL Pattern** | All widget classes use PIMPL for binary compatibility and clean headers |
| **Independently Usable** | Each component can be used independently or combined |
| **qss Customizable** | Custom widgets support qss stylesheets, selector format `DA--ClassName` |
| **Enum-First API** | Fixed-value parameters provide enum + string overloads |

## Component Families

### Selector Family

Matches Element UI's `el-select` (`multiple + filterable + allow-create`).

| Class | Description |
|-------|-------------|
| `DASelectComboBox` | Multi-tag dropdown selector, inherits `QComboBox` |
| `DAChipsEdit` | Flow-layout multi-tag input box, independently usable |
| `DAChip` | Single closable tag/pill widget |
| `DAFlowLayout` | Flow layout with trailing expanding item support |

### Collapse Family

Matches Element UI's `el-collapse` / `el-collapse-item`.

| Class | Description |
|-------|-------------|
| `DACollapse` | Collapsible panel container, accordion/multi-expand modes |
| `DACollapseItem` | Single collapsible item with animated expand/collapse |

### Cursor Utils

| Class | Description |
|-------|-------------|
| `DACursorScoped` | RAII cursor guard, exception-safe restoration |
| `DAWaitCursorScoped` | Wait cursor guard with `DA_WAIT_CURSOR_SCOPED()` macro |

### Property Edit Family

| Class | Description |
|-------|-------------|
| `DAPropertyItemWidget` | Property item container, Inline/Below layouts |
| `DATimeFormatEditWidget` | Time format editor |
| `DAAligmentEditWidget` | Alignment editor (left/right/top/bottom/center) |
| `DAAligmentPositionEditWidget` | 9-grid alignment position editor |
| `DAFilePathEditWidget` | File/directory path selector (line edit + browse; open/save/directory modes) |

### Item Selection & Preview Family

| Class | Description |
|-------|-------------|
| `DAItemEntry` | Generic candidate/selected entry: display text + identifying data (falls back to text equality) |
| `DAItemListEditWidget` | Selected-items list editor: inline remove, candidate-menu/pick-dialog add, remove-all |
| `DAItemListPickDialog` | Item check dialog: search filter (state preserved) + multi-select list |
| `DACheckListWidget` | Checkable list with search and optional Select All/Invert (forced entries stay checked; filtering keeps state) |
| `DAStringListEditWidget` | Plain string-list editor (Inline/External add modes, remove, up/down, max count) |
| `DAColorScalePreviewWidget` | 2/3-color horizontal linear gradient preview bar |

### Style Edit Family

| Class | Description |
|-------|-------------|
| `DABrushStyleComboBox` | Qt::BrushStyle selection combo box |
| `DAPenStyleComboBox` | Qt::PenStyle selection combo box |
| `DAColorGridWidget` | Basic color grid |
| `DAColorPaletteGridWidget` | Office-style color palette |
| `DAColorMenu` | Color selection menu |
| `DAColorPickerButton` | Color picker button |
| `DAPenEditWidget` | Pen editor (color + style + width) |
| `DABrushEditWidget` | Brush editor (solid/pattern/gradient/texture) |
| `DAFontEditPannelWidget` | Font editing panel |
| `DAShapeEditPannelWidget` | Shape editing panel (brush + pen) |

## Quick Integration

=== "CMake (Recommended)"

    ```cmake
    find_package(DAWidgets REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE DA::DAWidgets Qt${QT_VERSION_MAJOR}::Widgets)
    ```

=== "Source Files"

    ```cpp
    #include "DASelectComboBox.h"

    auto *combo = new DA::DASelectComboBox(parent);
    combo->setOptions({ "Option 1", "Option 2", "Option 3" });
    combo->setAllowCreate(true);
    ```

## Technical Specs

| Item | Value |
|------|-------|
| Language | C++17 |
| Framework | Qt Widgets (Qt5 / Qt6 dual-compatible) |
| Build System | CMake ≥ 3.16 |
| Namespace | `DA` |
| Class Prefix | `DA` |
| License | MIT |
