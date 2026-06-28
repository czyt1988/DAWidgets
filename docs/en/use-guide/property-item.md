# DAPropertyItemWidget

`DAPropertyItemWidget` is the base container widget for property editing panels. It provides a unified layout for the property name, description, and editor area, supporting both Inline (name and editor on the same row) and Below (name on top, editor below) layout modes.

## Key Features

- ✅ **Dual layout modes**: InlineLayout / BelowLayout
- ✅ **Property description**: Supports property name + description text
- ✅ **Arbitrary editor**: Embed any editor widget via `setEditorWidget()`
- ✅ **propertyId mechanism**: Each property item has a unique ID for batch management
- ✅ **Frame visibility**: Configurable border visibility

## Usage

### 1. Basic Usage

```cpp
#include "DAPropertyItemWidget.h"

// Create a property item
auto *prop = new DA::DAPropertyItemWidget(parent);
prop->setPropertyName("Font Size");
prop->setPropertyDescription("Set the font size of the text (points)");
prop->setPropertyId(1001);

// Embed an editor (any QWidget subclass)
auto *spinBox = new QSpinBox;
spinBox->setRange(6, 72);
spinBox->setValue(12);
prop->setEditorWidget(spinBox);

// Value changed signal
QObject::connect(prop, &DA::DAPropertyItemWidget::valueChanged,
    [](int propertyId) {
        qDebug() << "Property" << propertyId << "value changed";
    });
```

### 2. Layout Modes

```cpp
using namespace DA;

// Mode 1: Inline — name and editor on the same row
prop->setLayoutMode(DAPropertyItemWidget::LayoutMode::InlineLayout);

// Mode 2: Below — name on top, editor below
prop->setLayoutMode(DAPropertyItemWidget::LayoutMode::BelowLayout);
```

| Mode | Layout | Use Case |
|------|--------|----------|
| `InlineLayout` | Name and editor on the same row | Simple editors (input fields, combo boxes) |
| `BelowLayout` | Name on top, editor below | Complex editors (color pickers, multi-control combinations) |

### 3. Building a Property Panel

```cpp
// Typical property panel construction
auto *panel = new QWidget;
auto *layout = new QVBoxLayout(panel);

// Property 1: Font
auto *fontProp = new DA::DAPropertyItemWidget;
fontProp->setPropertyName("Font");
fontProp->setPropertyId(1);
fontProp->setEditorWidget(new QFontComboBox);
layout->addWidget(fontProp);

// Property 2: Color
auto *colorProp = new DA::DAPropertyItemWidget;
colorProp->setPropertyName("Color");
colorProp->setPropertyId(2);
colorProp->setEditorWidget(new DA::DAColorPickerButton);
layout->addWidget(colorProp);

layout->addStretch();
```

!!! tip "Batch Management"
    Use `propertyId` to identify which property changed in the `valueChanged(int)` signal handler. This is especially useful when multiple property items share a single signal connection.

## Core Properties & Methods

### Q_PROPERTY

| Property | Type | Description |
|----------|------|-------------|
| `propertyId` | `int` | Unique property identifier |
| `propertyName` | `QString` | Property name |
| `propertyDescription` | `QString` | Property description |
| `layoutMode` | `LayoutMode` | Layout mode |
| `frameVisible` | `bool` | Whether the border is visible |

### Core Methods

| Method | Description |
|--------|-------------|
| `setPropertyName(QString)` | Set the property name |
| `setPropertyDescription(QString)` | Set the property description |
| `setPropertyId(int)` | Set the property ID |
| `setEditorWidget(QWidget*)` | Set the editor widget |
| `editorWidget()` | Get the current editor widget |
| `setLayoutMode(LayoutMode)` | Set the layout mode |
| `setNameLabelWidth(int)` | Set the name label width |
| `setFrameVisible(bool)` | Set border visibility |

## References

- [Alignment Editor](alignment-edit.md)
- [Color Menu & Picker](color-menu.md)
