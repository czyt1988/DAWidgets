# DAChip

`DAChip` is a closable tag/pill widget composed of a `QFrame` + label + close button. It supports qss styling and can be used independently.

## Key Features

- ✅ **Closable**: Built-in close button that emits `closeRequested` on click
- ✅ **qss customizable**: Selector `DA--DAChip`, sub-controls locatable by objectName
- ✅ **Independently usable**: Can be used without DAChipsEdit
- ✅ **Click signal**: Supports click interaction

## Usage

### 1. Basic Usage

```cpp
#include "DAChip.h"

// Create a closable tag
auto *chip = new DA::DAChip("Tag text", parent);
chip->setClosable(true);

// Remove when the close button is clicked
QObject::connect(chip, &DA::DAChip::closeRequested, [chip]() {
    chip->deleteLater();
});

// Tag clicked
QObject::connect(chip, &DA::DAChip::clicked, []() {
    qDebug() << "Tag clicked";
});
```

### 2. qss Styling

```css
DA--DAChip {
    border: 1px solid #dcdfe6;
    border-radius: 4px;
    background: #f4f4f5;
    padding: 2px 8px;
}
DA--DAChip:hover {
    background: #e9e9eb;
}
/* Close button */
DA--DAChip #DAChipCloseButton {
    border: none;
    background: transparent;
    border-radius: 2px;
}
DA--DAChip #DAChipCloseButton:hover {
    background: #c0c4cc;
}
/* Tag text */
DA--DAChip #DAChipLabel {
    color: #606266;
    font-size: 12px;
}
```

### 3. Usage Inside DAChipsEdit

`DAChipsEdit` automatically creates and manages `DAChip` instances internally, so manual creation is usually unnecessary:

```cpp
auto *edit = new DA::DAChipsEdit(parent);
edit->setTags({ "Tag1", "Tag2" });  // DAChip instances are created internally
```

!!! info "Sub-control Selectors"
    The close button uses objectName `DAChipCloseButton` and the label uses `DAChipLabel`. These can be targeted in qss for fine-grained styling.

## Core Properties & Methods

| Property / Method | Type | Description |
|-------------------|------|-------------|
| `text` / `setText()` | `QString` | Tag text |
| `closable` / `setClosable()` | `bool` | Whether to show the close button |

### Signals

| Signal | Description |
|--------|-------------|
| `closeRequested()` | Close button was clicked |
| `clicked()` | Tag was clicked |
| `textChanged(QString)` | Text changed |
| `closableChanged(bool)` | closable property changed |

## References

- [DAChipsEdit](chips-edit.md)
- [DASelectComboBox](select-combobox.md)
