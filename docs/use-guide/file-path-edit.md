# 文件路径编辑器

`DAFilePathEditWidget` 提供文件路径选择控件，由 LineEdit + 打开按钮组成，支持文件过滤器。

## 核心特性

**特性**

- ✅ **路径输入**：LineEdit 直接输入路径
- ✅ **文件选择**：点击按钮弹出文件对话框
- ✅ **过滤器**：支持设置文件名过滤器
- ✅ **信号通知**：`selectedPath(QString)`

## 用法

### 1. 基本用法

```cpp
#include "DAFilePathEditWidget.h"

auto *fileEdit = new DA::DAFilePathEditWidget(parent);

// 设置文件过滤器
fileEdit->setNameFilters({
    "文本文件 (*.txt)",
    "图片文件 (*.png *.jpg *.bmp)",
    "所有文件 (*.*)"
});

// 监听路径选择
QObject::connect(fileEdit, &DA::DAFilePathEditWidget::selectedPath,
    [](const QString& path) {
        qDebug() << "选择的文件:" << path;
    });
```

### 2. 获取/设置路径

```cpp
// 设置当前路径
fileEdit->setFilePath("/home/user/document.txt");

// 获取当前路径
QString path = fileEdit->getFilePath();
```

## 核心方法

| 方法 | 说明 |
|------|------|
| `setFilePath(QString)` | 设置文件路径 |
| `getFilePath()` | 获取文件路径 |
| `setNameFilter(QString)` | 设置单个文件过滤器 |
| `setNameFilters(QStringList)` | 设置多个文件过滤器 |

## 参考

- [DAPropertyItemWidget 属性项](property-item.md)
