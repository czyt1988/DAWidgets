# Font Edit Panel

`DAFontEditPannelWidget` provides a complete font editing panel, including font selection, font size, bold/italic/underline toggles, and font color.

## Key Features

- ✅ **Font selection**: QFontComboBox
- ✅ **Font size**: QSpinBox
- ✅ **Style toggles**: Bold, italic, underline
- ✅ **Font color**: DAColorPickerButton integration
- ✅ **Signal notification**: `currentFontChanged`, `currentFontColorChanged`

## Usage

### 1. Basic Usage

```cpp
#include "DAFontEditPannelWidget.h"

auto *fontPanel = new DA::DAFontEditPannelWidget(parent);

// Set the current font
QFont font("Microsoft YaHei", 12);
font.setBold(true);
fontPanel->setCurrentFont(font);

// Set the font color
fontPanel->setCurrentFontColor(Qt::red);

// Get the current font
QFont currentFont = fontPanel->getCurrentFont();
```

### 2. Signal Handling

```cpp
// Font changed
QObject::connect(fontPanel, &DA::DAFontEditPannelWidget::currentFontChanged,
    [](const QFont& font) {
        qDebug() << "Font:" << font.family() << font.pointSize();
    });

// Font color changed
QObject::connect(fontPanel, &DA::DAFontEditPannelWidget::currentFontColorChanged,
    [](const QColor& color) {
        qDebug() << "Font color:" << color.name();
    });
```

### 3. Font Weight

```cpp
// Set the font weight (Qt6 QFont::Weight enum)
fontPanel->setFontWeight(QFont::DemiBold);
```

!!! warning "Qt5/Qt6 Weight Enum"
    `QFont::Weight` was redefined between Qt5 and Qt6. In Qt5, the enum uses `QFont::Light`, `QFont::Normal`, `QFont::DemiBold`, `QFont::Bold`, `QFont::Black`. In Qt6, weight is an integer (0–1000) with named constants like `QFont::Medium`. DAWidgets handles this difference internally — always use the `setFontWeight()` API rather than directly manipulating the QFont.

## Core Properties & Methods

| Method | Description |
|--------|-------------|
| `getCurrentFont()` / `setCurrentFont(QFont)` | Get/set the font |
| `getCurrentFontColor()` / `setCurrentFontColor(QColor)` | Get/set the font color |
| `getFontWeight()` / `setFontWeight(QFont::Weight)` | Get/set the font weight |

## References

- [Color Menu & Picker](color-menu.md)
- [Shape Edit Panel](shape-edit.md)
