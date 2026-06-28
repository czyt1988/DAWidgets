# 最近文件管理器

`DARecentFilesManager` 管理最近打开的文件列表，支持菜单集成和持久化存储到 `QSettings`。

## 核心特性

**特性**

- ✅ **菜单集成**：`attachToMenu()` 一行挂载到 QMenu
- ✅ **持久化**：自动保存/加载到 QSettings（orgName/appName）
- ✅ **FIFO 管理**：新文件添加到列表头部，超过上限自动移除尾部
- ✅ **可配置上限**：默认最多 8 条
- ✅ **信号通知**：`fileSelected(QString)`

## 用法

### 1. 基本用法

```cpp
#include "DARecentFilesManager.h"
#include <QMenuBar>

// 创建管理器
auto *recentManager = new DA::DARecentFilesManager(this);

// 附加到菜单
QMenu *fileMenu = menuBar()->addMenu("文件");
recentManager->attachToMenu(fileMenu, "最近打开");

// 监听文件选择
QObject::connect(recentManager, &DA::DARecentFilesManager::fileSelected,
    [this](const QString& path) {
        openFile(path);
    });
```

### 2. 添加文件

```cpp
// 用户打开了一个文件
recentManager->addFile("/path/to/file.dat");
// 文件会被添加到列表头部，如果已存在则移至头部
// 超过上限（默认 8）时自动移除尾部
```

### 3. 清除列表

```cpp
// 用户点击"清除最近文件"
recentManager->clear();
```

### 4. 重新扫描

```cpp
// 检查列表中的文件是否仍然存在
recentManager->rescan();
```

## 核心方法

| 方法 | 说明 |
|------|------|
| `attachToMenu(QMenu*, title)` | 挂载到菜单 |
| `addFile(QString)` | 添加文件到列表 |
| `clear()` | 清除所有记录 |
| `rescan()` | 重新扫描（移除不存在的文件） |

## 参考

- [DAPropertyItemWidget 属性项](property-item.md)
