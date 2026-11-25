# DAChipsEdit 标签输入框

`DAChipsEdit` 是一个流式多标签输入框，可独立使用。用户通过回车或分隔符提交标签，Backspace 删除最后一个标签。支持去重、限量、自定义分隔符。

## 效果预览

![DAChipsEdit：已提交 Qt、C++ 两个标签，输入框占满剩余宽度](../images/gallery/widgets/chips-edit.png)

上图中已提交的 `Qt`、`C++` 以标签形式排列，末尾输入框由 `DAFlowLayout` 的末尾扩展特性填满剩余宽度；继续输入并按回车/逗号/分号即可追加标签。下方依次是演示用的 `tagsChanged` 回显与最大数量、去重开关，以及显式打开「粘贴批量拆分 / 超长标签省略 / 数量计数」的第二组输入框。

## 核心特性

**特性**

- ✅ **流式布局**：标签自动换行排列
- ✅ **多分隔符提交**：逗号、分号、空格、回车、Tab（枚举 + 字符串两种设置方式）
- ✅ **去重控制**：可禁止重复标签
- ✅ **数量限制**：可设置最大标签数
- ✅ **独立可用**：无需 DASelectComboBox 即可独立使用
- ✅ **扩展点**：暴露 `inputTextChanged` / `inputFocusChanged` / `inputKeyEvent` 供外部消费
- ✅ **粘贴批量拆分**（可选）：一次粘贴按分隔符拆成多个标签
- ✅ **超长省略**（可选）：限制 chip 宽度，`…` 结尾 + tooltip 显示全文
- ✅ **数量计数**（可选）：右下角显示 `n / maxCount`

## 类关系

```mermaid
classDiagram
    class QFrame
    class DAChipsEdit {
        +setTags(QStringList)
        +appendTag(QString)
        +setSeparators(QList~Separator~)
        +setMaxCount(int)
        +setAllowDuplicate(bool)
    }
    class DAChip {
        +setText(QString)
        +closeRequested()
    }
    class DAFlowLayout {
        +setTrailingExpanding(bool)
    }

    QFrame <|-- DAChipsEdit
    DAChipsEdit --> DAChip : 包含多个标签
    DAChipsEdit --> DAFlowLayout : 使用流式布局
```

## 用法

对应的示例位于：`examples/all_widgets_demo`（选择器/标签 tab）。

### 1. 基本用法

```cpp
#include "DAChipsEdit.h"

auto *edit = new DA::DAChipsEdit(parent);

// 设置占位提示
edit->setPlaceholder("输入标签后回车...");

// 预设标签
edit->setTags({ "C++", "Qt", "Widgets" });

// 禁止重复
edit->setAllowDuplicate(false);

// 限制最多 5 个标签
edit->setMaxCount(5);
```

### 2. 自定义分隔符

`DAChipsEdit` 提供两种设置分隔符的方式：枚举重载（编译期类型安全）和字符串重载（运行时配置）。

```cpp
using namespace DA;

// 方式一：枚举重载（推荐 C++ 使用）
edit->setSeparators({
    DAChipsEdit::Separator::Comma,
    DAChipsEdit::Separator::Enter,
    DAChipsEdit::Separator::Semicolon
});

// 方式二：字符串重载（适合运行时配置）
edit->setSeparators(",;\n");  // 逗号、分号、换行
```

| 枚举值 | 对应字符 | 说明 |
|--------|---------|------|
| `Comma` | `,` | 逗号 |
| `Semicolon` | `;` | 分号 |
| `Space` | ` ` | 空格 |
| `Enter` | `\n` | 回车 |
| `Tab` | `\t` | Tab 键 |

### 3. 信号处理

```cpp
// 标签添加
QObject::connect(edit, &DA::DAChipsEdit::tagAdded,
    [](const QString& tag) {
        qDebug() << "添加标签:" << tag;
    });

// 标签移除
QObject::connect(edit, &DA::DAChipsEdit::tagRemoved,
    [](const QString& tag) {
        qDebug() << "移除标签:" << tag;
    });

// 标签列表变化
QObject::connect(edit, &DA::DAChipsEdit::tagsChanged,
    [](const QStringList& tags) {
        qDebug() << "当前标签:" << tags;
    });
```

### 4. 扩展点

`DAChipsEdit` 暴露三个扩展点供 `DASelectComboBox` 等外部消费者使用：

```cpp
// 输入框文本变化
QObject::connect(edit, &DA::DAChipsEdit::inputTextChanged,
    [](const QString& text) {
        // 可用于实现过滤逻辑
    });

// 输入框焦点变化
QObject::connect(edit, &DA::DAChipsEdit::inputFocusChanged,
    [](bool focused) {
        // 可用于控制弹出列表显隐
    });

// 按键事件拦截
QObject::connect(edit, &DA::DAChipsEdit::inputKeyEvent,
    [](QKeyEvent* event, bool* handled) {
        // 可拦截特定按键，设置 *handled = true 阻止默认处理
    });
```

### 5. 粘贴批量拆分

默认关闭。打开后，`Ctrl+V` / `Shift+Insert`（macOS 为 `Cmd+V`）会把剪贴板文本按当前分隔符集合拆成多个标签；`Enter` 对应换行，因此从表格复制的一整列可以直接粘贴进输入框。

```cpp
using namespace DA;

edit->setPasteSplitEnabled(true);
edit->setSeparators({
    DAChipsEdit::Separator::Comma,
    DAChipsEdit::Separator::Semicolon,
    DAChipsEdit::Separator::Enter
});

// 快捷键、demo 按钮、单元测试共用同一入口
edit->pasteText("红, 绿; 蓝\n黄");  // → 4 个标签
```

- 去重与 `maxCount` 仍然生效；只有被上限挡住的项才发 `pasteRejected("maxCount")`，取不到剪贴板文本时发 `pasteRejected("clipboard")`。
- 一次粘贴只发一次 `pasteSplit(QStringList)`，参数是去重前的 token 列表（已 trim、已丢空串）。
- `allowCreate == false` 时粘贴退化为普通插入，不拆分、不发信号。
- 关闭时 `pasteText()` 等价于把文本插入内嵌输入框；`Ctrl+V` 不被拦截。

### 6. 超长标签省略

```cpp
edit->setTagElideWidth(96);   // 0（默认）= 不省略
```

超过 96px 的标签以 `…` 结尾显示，悬浮显示全文；`DAChip::text()` 与 `tags()` 始终是全文。推荐宽度不小于 64px，过小会挤压关闭按钮。省略开启期间 tooltip 由控件接管（会覆盖调用方设置的 tooltip）。

### 7. 数量计数

```cpp
edit->setShowCount(true);   // 默认 false
edit->setMaxCount(5);       // 已选 3 个时显示 "3 / 5"
```

`maxCount == -1` 时只显示当前数量（如 `3`）。计数标签是右下角的私有子控件，不参与流式排版，最后一行输入框仍占满剩余宽度，不应把计数标签当作可布局子项。

## 核心属性与方法

### Q_PROPERTY

| 属性 | 类型 | 说明 |
|------|------|------|
| `tags` | `QStringList` | 标签列表 |
| `placeholder` | `QString` | 占位提示 |
| `allowCreate` | `bool` | 是否允许创建新标签 |
| `allowDuplicate` | `bool` | 是否允许重复标签 |
| `maxCount` | `int` | 最大标签数（-1 = 无限） |

### 核心方法

| 方法 | 说明 |
|------|------|
| `setTags(QStringList)` | 设置标签列表 |
| `appendTag(QString)` | 追加单个标签 |
| `insertTag(int, QString)` | 在指定位置插入标签 |
| `removeTag(QString)` | 按文本移除标签 |
| `removeAt(int)` | 按索引移除标签 |
| `clear()` | 清除所有标签 |
| `setSeparators(QList<Separator>)` | 枚举方式设置分隔符 |
| `setSeparators(QString)` | 字符串方式设置分隔符 |
| `inputText()` | 获取当前输入框文本 |
| `clearInput()` | 清空输入框 |
| `setPasteSplitEnabled(bool)` | 打开/关闭粘贴批量拆分（默认 `false`） |
| `pasteText(QString)` | 粘贴唯一入口，快捷键/按钮/测试共用 |
| `setTagElideWidth(int)` | chip 最大宽度（px），`0` 不省略（默认） |
| `setShowCount(bool)` | 右下角显示 `n / maxCount`（默认 `false`） |

## 参考

- [DASelectComboBox 下拉多选](select-combobox.md)
- [DAChip 标签胶囊](chip.md)
- [DAFlowLayout 流式布局](flow-layout.md)
- 示例路径：`examples/all_widgets_demo`（选择器/标签 tab）
