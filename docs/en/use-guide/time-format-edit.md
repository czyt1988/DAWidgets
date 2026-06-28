# Time Format Editor

`DATimeFormatEditWidget` provides a time format setting control that allows the user to select or type a time display format string.

## Key Features

- ✅ **Format presets**: Provides common time format presets
- ✅ **Custom input**: Supports manually typing a format string
- ✅ **Signal notification**: Emits a signal when the format changes

## Usage

```cpp
#include "DATimeFormatEditWidget.h"

auto *timeEdit = new DA::DATimeFormatEditWidget(parent);

// Set the time format
timeEdit->setTimeFormatText("yyyy-MM-dd HH:mm:ss");

// Get the current format
QString format = timeEdit->getTimeFormat();
```

!!! info "Format Syntax"
    The format string uses Qt's date/time format tokens: `yyyy` (4-digit year), `MM` (2-digit month), `dd` (2-digit day), `HH` (24-hour), `mm` (minutes), `ss` (seconds). See the [Qt documentation](https://doc.qt.io/qt/qdatetime.html#toString) for the full list.

## Core Properties & Methods

| Method | Description |
|--------|-------------|
| `getTimeFormat()` | Get the current time format string |
| `setTimeFormatText(QString)` | Set the time format string |

## References

- [DAPropertyItemWidget](property-item.md)
