# DATextEditWidget 文本编辑/查看

`DATextEditWidget` 是一个通用文本查看/编辑控件：可切换只读、等宽字体、自动换行，工具条动作由调用方注入。控件本身不拥有 `OK` / `Cancel`，需要模态交互时由调用方套一个 `QDialog` 壳。

## 效果预览

![DATextEditWidget：只读/等宽/换行开关、注入的“另存为”动作与等宽文本](../images/gallery/widgets/text-edit.png)

截图中的工具条只有一个由外部注入的「另存为」动作，控件只负责展示与转发 `triggered`；下方四个复选框分别演示只读、等宽、换行与工具条显隐。文本区使用等宽字体时列对齐更整齐，适合查看日志、配置片段等。

## 核心特性

**特性**

- ✅ **只读/编辑切换**：`setReadOnly()`
- ✅ **等宽字体**：`setMonospace()`，适合日志与代码
- ✅ **自动换行**：`setLineWrapEnabled()`
- ✅ **可注入工具动作**：`addToolAction(QAction*)`，动作生命周期由调用方管理
- ✅ **不绑定业务**：不内置保存/关闭逻辑，方便复用到对话框或停靠面板

## 用法

```cpp
#include "DATextEditWidget.h"

auto *view = new DA::DATextEditWidget(parent);
view->setText("第一行文本\n第二行文本");
view->setPlaceholderText("输入文本...");
view->setMonospace(true);
view->setLineWrapEnabled(true);
view->setReadOnly(true);

// 注入一个工具动作（控件不拥有它，生命周期由调用方负责）
auto *saveAction = new QAction("另存为", view);
view->addToolAction(saveAction);
view->setToolbarVisible(true);

QObject::connect(saveAction, &QAction::triggered, []() {
    qDebug() << "另存为被触发";
});
QObject::connect(view, &DA::DATextEditWidget::textChanged, []() {
    qDebug() << "用户编辑了文本";
});
```

!!! info "setText 不发射 textChanged"
    `setText()` 用于程序化写入，不会触发 `textChanged`；只有用户编辑才会发射，避免调用方回环刷新。

## 核心属性与方法

| 方法 | 说明 |
|------|------|
| `setReadOnly(bool)` / `isReadOnly()` | 只读状态 |
| `setMonospace(bool)` / `isMonospace()` | 等宽字体 |
| `setLineWrapEnabled(bool)` / `isLineWrapEnabled()` | 自动换行 |
| `setText(QString)` / `text()` | 设置/获取文本（`setText` 不发信号） |
| `setPlaceholderText(QString)` | 空文本占位提示 |
| `addToolAction(QAction*)` | 向工具条注入动作 |
| `setToolbarVisible(bool)` / `isToolbarVisible()` | 工具条显隐 |

## 参考

- [DAUndoHistoryWidget 撤销历史](undo-overlay.md)
- 示例路径：`examples/all_widgets_demo`（文本 tab）
