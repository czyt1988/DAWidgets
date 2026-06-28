# Color Menu & Picker

`DAColorMenu` provides a color selection menu (theme palette + custom colors + None), and `DAColorPickerButton` is a self-drawing color picker button.

## DAColorMenu

### Key Features

- ✅ **Complete menu structure**: Title → theme palette → "None" action → "Custom Color" action → custom color grid
- ✅ **None color**: `enableNoneColorAction` controls the None option
- ✅ **Custom color management**: Up to 10 custom colors, FIFO eviction
- ✅ **Exposed sub-controls**: `colorPaletteGridWidget()`, `customColorsWidget()` can be accessed directly

### Usage

```cpp
#include "DAColorMenu.h"

auto *menu = new DA::DAColorMenu(parent);

// Enable the None color option
menu->enableNoneColorAction(true);

// Listen for color selection
QObject::connect(menu, &DA::DAColorMenu::selectedColor,
    [](const QColor& color) {
        if (color.isValid()) {
            qDebug() << "Selected color:" << color.name();
        } else {
            qDebug() << "Selected None";
        }
    });

// Show the menu
menu->exec(QCursor::pos());
```

## DAColorPickerButton

### Key Features

- ✅ **Self-drawing color block**: Draws the current color on the button
- ✅ **Invalid color display**: An invalid color (`QColor()`) shows a red slash
- ✅ **Two modes**: `WithColorMenu` (built-in color menu) / `NoColorMenu` (display only)
- ✅ **Static method**: `paintNoneColor()` can be used by other widgets to draw a None color block

### Usage

```cpp
#include "DAColorPickerButton.h"

// Method 1: With built-in color menu
auto *btn = new DA::DAColorPickerButton(parent);
btn->setColorToolButtonStyle(DA::DAColorPickerButton::ColorToolButtonStyle::WithColorMenu);
btn->setColor(Qt::red);

// Listen for color changes
QObject::connect(btn, &DA::DAColorPickerButton::colorChanged,
    [](const QColor& color) {
        qDebug() << "Color changed:" << color.name();
    });

// Method 2: Display only (no menu popup)
auto *display = new DA::DAColorPickerButton(parent);
display->setColorToolButtonStyle(DA::DAColorPickerButton::ColorToolButtonStyle::NoColorMenu);
display->setColor(Qt::blue);
```

### Custom Color Menu

```cpp
auto *btn = new DA::DAColorPickerButton(parent);

// Get or create the color menu
DA::DAColorMenu *menu = btn->colorMenu();
if (!menu) {
    menu = btn->createColorMenu();
}

// Customize the theme palette
menu->colorPaletteGridWidget()->setColorList({
    QColor("#FF6B6B"), QColor("#4ECDC4"), QColor("#45B7D1"),
    QColor("#FFA07A"), QColor("#98D8C8"), QColor("#F7DC6F")
});
```

!!! tip "Custom Color FIFO"
    When the user picks a custom color, it is added to the custom color grid. The grid holds at most 10 entries — when full, the oldest entry is evicted (FIFO). This mirrors the behavior of Office color pickers.

## Core Properties & Methods

### DAColorMenu

| Method | Description |
|--------|-------------|
| `enableNoneColorAction(bool)` | Enable/disable the None option |
| `colorPaletteGridWidget()` | Get the theme palette widget |
| `customColorsWidget()` | Get the custom color grid widget |

### DAColorPickerButton

| Method | Description |
|--------|-------------|
| `color()` / `setColor(QColor)` | Get/set the color |
| `setColorToolButtonStyle(Style)` | Set the button mode |
| `colorMenu()` | Get the color menu (may be null) |
| `createColorMenu()` | Create a color menu |
| `margins` | Color block margins |
| `paintNoneColor(QPainter, rect)` | Static method — paint a None color block |

## References

- [Color Grid & Palette](color-grid.md)
- [Pen Style Editor](pen-edit.md)
