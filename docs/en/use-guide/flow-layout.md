# DAFlowLayout

`DAFlowLayout` is a flow layout that arranges child widgets left-to-right, top-to-bottom, automatically wrapping to the next line when the width is exceeded. The trailing item can be set to expand and fill the remaining space.

## Key Features

- ✅ **Auto-wrap**: Child widgets automatically wrap to the next line when exceeding container width
- ✅ **Trailing expansion**: `setTrailingExpanding(true)` makes the last item fill the remaining width
- ✅ **heightForWidth support**: Dynamically calculates required height based on width
- ✅ **Independently usable**: Can be used in any scenario requiring flow arrangement

## Usage

### 1. Basic Usage

```cpp
#include "DAFlowLayout.h"

auto *layout = new DA::DAFlowLayout;

// Add multiple widgets — they will wrap automatically
for (int i = 0; i < 10; ++i) {
    layout->addWidget(new QPushButton(QString("Button %1").arg(i)));
}

container->setLayout(layout);
```

### 2. Trailing Expansion Mode

In `DAChipsEdit`, the input field is the trailing item and needs to fill the remaining width:

```cpp
auto *layout = new DA::DAFlowLayout;

// Enable trailing expansion
layout->setTrailingExpanding(true);

// Add tag widgets...
layout->addWidget(chip1);
layout->addWidget(chip2);

// Add the input field last — it fills the remaining width on its line
layout->addWidget(inputField);
```

!!! info "How It Works"
    When `trailingExpanding` is true, the layout calculates the remaining width on each line and sets the trailing item's width to fill it. If the trailing item is alone on its line, it occupies the full line width.

### 3. Spacing and Margins

```cpp
auto *layout = new DA::DAFlowLayout;

// Set widget spacing
layout->setSpacing(8);

// Set margins
layout->setContentsMargins(4, 4, 4, 4);
```

## Core Properties & Methods

| Method | Description |
|--------|-------------|
| `setTrailingExpanding(bool)` | Enable/disable trailing item expansion |
| `trailingExpanding()` | Get trailing expansion state |
| `setSpacing(int)` | Set widget spacing |
| `heightForWidth(int)` | Calculate required height for a given width |

## References

- [DAChipsEdit](chips-edit.md)
- [DAChip](chip.md)
