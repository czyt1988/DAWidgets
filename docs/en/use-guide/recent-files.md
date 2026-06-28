# Recent Files Manager

`DARecentFilesManager` manages a list of recently opened files, supporting menu integration and persistent storage via `QSettings`.

## Key Features

- ✅ **Menu integration**: `attachToMenu()` mounts onto a QMenu in one line
- ✅ **Persistence**: Automatically saves/loads to QSettings (orgName/appName)
- ✅ **FIFO management**: New files are added to the head of the list; the tail is removed when the limit is exceeded
- ✅ **Configurable limit**: Default maximum of 8 entries
- ✅ **Signal notification**: `fileSelected(QString)`

## Usage

### 1. Basic Usage

```cpp
#include "DARecentFilesManager.h"
#include <QMenuBar>

// Create the manager
auto *recentManager = new DA::DARecentFilesManager(this);

// Attach to a menu
QMenu *fileMenu = menuBar()->addMenu("File");
recentManager->attachToMenu(fileMenu, "Recent Files");

// Listen for file selection
QObject::connect(recentManager, &DA::DARecentFilesManager::fileSelected,
    [this](const QString& path) {
        openFile(path);
    });
```

### 2. Adding Files

```cpp
// The user opened a file
recentManager->addFile("/path/to/file.dat");
// The file is added to the head of the list.
// If it already exists, it is moved to the head.
// When the limit (default 8) is exceeded, the tail entry is removed.
```

### 3. Clearing the List

```cpp
// User clicks "Clear Recent Files"
recentManager->clear();
```

### 4. Rescanning

```cpp
// Check whether files in the list still exist on disk
recentManager->rescan();
```

!!! info "Persistence"
    The list is stored in `QSettings` using the application's organization name and application name. On startup, the manager automatically loads the saved list. Files that no longer exist on disk are retained until `rescan()` is called.

## Core Properties & Methods

| Method | Description |
|--------|-------------|
| `attachToMenu(QMenu*, title)` | Mount onto a menu |
| `addFile(QString)` | Add a file to the list |
| `clear()` | Clear all entries |
| `rescan()` | Rescan (remove files that no longer exist) |

## References

- [DAPropertyItemWidget](property-item.md)
