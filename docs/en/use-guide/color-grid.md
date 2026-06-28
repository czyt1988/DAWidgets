# Color Grid & Palette

`DAColorGridWidget` provides a basic color grid, and `DAColorPaletteGridWidget` builds on it to create an Office-style color palette (primary colors + auto-generated lighter/darker rows).

## DAColorGridWidget

### Key Features

- ✅ **Grid layout**: QGridLayout + QButtonGroup
- ✅ **Configurable column count**: `setColumnCount(int)`
- ✅ **Custom color list**: `setColorList(QList<QColor>)`
- ✅ **Checkable**: `setColorCheckable(bool)`
- ✅ **Adjustable size**: `setColorIconSize(QSize)`, `setSpacing(int)`

### Usage

```cpp
#include "DAColorGridWidget.h"

auto *grid = new DA::DAColorGridWidget(parent);

// Set the column count
grid->setColumnCount(8);

// Set the color list
grid->setColorList({
    Qt::red, Qt::green, Qt::blue, Qt::yellow,
    Qt::cyan, Qt::magenta, Qt::gray, Qt::black
});

// Enable checkable selection
grid->setColorCheckable(true);

// Listen for clicks
QObject::connect(grid, &DA::DAColorGridWidget::colorClicked,
    [](const QColor& color) {
        qDebug() << "Clicked color:" << color;
    });
```

## DAColorPaletteGridWidget

### Key Features

- ✅ **Office style**: Primary color row + auto-generated lighter rows + darker rows
- ✅ **Light/dark factors**: `setFactor(QList<int>)` controls the number of generated rows and their lightness/darkness
- ✅ **Composed of two grids**: Internally uses two `DAColorGridWidget` instances

### Usage

```cpp
#include "DAColorPaletteGridWidget.h"

auto *palette = new DA::DAColorPaletteGridWidget(parent);

// Set the primary color list
palette->setColorList({
    QColor("#C00000"), QColor("#FF0000"), QColor("#FFC000"),
    QColor("#FFFF00"), QColor("#92D050"), QColor("#00B050"),
    QColor("#00B0F0"), QColor("#0070C0"), QColor("#002060"), QColor("#7030A0")
});

// Light/dark generation factors (percentage; positive = lighter, negative = darker)
palette->setFactor({ 80, 60, 40, -25, -50 });

// Listen for color clicks
QObject::connect(palette, &DA::DAColorPaletteGridWidget::colorClicked,
    [](const QColor& color) {
        qDebug() << "Selected color:" << color.name();
    });
```

### How It Works

```text
┌─────────────────────────────────────────┐
│  Primary row (original colors)           │
│  ■ ■ ■ ■ ■ ■ ■ ■ ■ ■                    │
├─────────────────────────────────────────┤
│  Lighter row 1 (+80%)                    │
│  ░ ░ ░ ░ ░ ░ ░ ░ ░ ░                    │
├─────────────────────────────────────────┤
│  ...more lighter rows...                 │
├─────────────────────────────────────────┤
│  Darker row 1 (-25%)                     │
│  ▓ ▓ ▓ ▓ ▓ ▓ ▓ ▓ ▓ ▓                    │
└─────────────────────────────────────────┘
```

!!! info "Factor Values"
    Each factor in `setFactor()` is a percentage. Positive values lighten the color (toward white); negative values darken it (toward black). The order of factors determines the order of rows in the palette.

## Core Properties & Methods

### DAColorGridWidget

| Method | Description |
|--------|-------------|
| `setColumnCount(int)` | Set the column count |
| `setColorList(QList<QColor>)` | Set the color list |
| `setColorIconSize(QSize)` | Set the color block size |
| `setSpacing(int)` | Set the spacing |
| `setColorCheckable(bool)` | Enable/disable checkable selection |
| `currentCheckedColor()` | Get the currently checked color |

### DAColorPaletteGridWidget

| Method | Description |
|--------|-------------|
| `setColorList(QList<QColor>)` | Set the primary color list |
| `setFactor(QList<int>)` | Set the light/dark factors |
| `setColorIconSize(QSize)` | Set the color block size |
| `setColorCheckable(bool)` | Enable/disable checkable selection |

## References

- [Color Menu & Picker](color-menu.md)
