# File Path Editor

`DAFilePathEditWidget` provides a file path selection control, consisting of a LineEdit + a browse button, with support for file name filters.

## Key Features

- ✅ **Path input**: Type a path directly in the LineEdit
- ✅ **File selection**: Click the button to open a file dialog
- ✅ **Filters**: Supports setting file name filters
- ✅ **Signal notification**: `selectedPath(QString)`

## Usage

### 1. Basic Usage

```cpp
#include "DAFilePathEditWidget.h"

auto *fileEdit = new DA::DAFilePathEditWidget(parent);

// Set file name filters
fileEdit->setNameFilters({
    "Text files (*.txt)",
    "Image files (*.png *.jpg *.bmp)",
    "All files (*.*)"
});

// Listen for path selection
QObject::connect(fileEdit, &DA::DAFilePathEditWidget::selectedPath,
    [](const QString& path) {
        qDebug() << "Selected file:" << path;
    });
```

### 2. Getting / Setting the Path

```cpp
// Set the current path
fileEdit->setFilePath("/home/user/document.txt");

// Get the current path
QString path = fileEdit->getFilePath();
```

!!! tip "Filter Format"
    Filters follow the Qt convention: `"Description (*.ext1 *.ext2)"`. Multiple filters can be set via `setNameFilters(QStringList)`.

## Core Properties & Methods

| Method | Description |
|--------|-------------|
| `setFilePath(QString)` | Set the file path |
| `getFilePath()` | Get the file path |
| `setNameFilter(QString)` | Set a single file name filter |
| `setNameFilters(QStringList)` | Set multiple file name filters |

## References

- [DAPropertyItemWidget](property-item.md)
