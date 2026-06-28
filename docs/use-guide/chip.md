# DAChip 标签胶囊

`DAChip` 是一个可关闭的标签/胶囊控件，由 `QFrame` + 标签 + 关闭按钮组成。支持 qss 样式定制，可独立使用。

## 核心特性

**特性**

- ✅ **可关闭**：内置关闭按钮，点击发射 `closeRequested` 信号
- ✅ **qss 可定制**：选择器 `DA--DAChip`，子控件 objectName 可定位
- ✅ **独立可用**：可脱离 DAChipsEdit 单独使用
- ✅ **点击信号**：支持点击交互

## 用法

### 1. 基本用法

```cpp
#include "DAChip.h"

// 创建一个可关闭的标签
auto *chip = new DA::DAChip("标签文本", parent);
chip->setClosable(true);

// 点击关闭按钮时移除
QObject::connect(chip, &DA::DAChip::closeRequested, [chip]() {
    chip->deleteLater();
});

// 点击标签
QObject::connect(chip, &DA::DAChip::clicked, []() {
    qDebug() << "标签被点击";
});
```

### 2. qss 样式定制

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
/* 关闭按钮 */
DA--DAChip #DAChipCloseButton {
    border: none;
    background: transparent;
    border-radius: 2px;
}
DA--DAChip #DAChipCloseButton:hover {
    background: #c0c4cc;
}
/* 标签文本 */
DA--DAChip #DAChipLabel {
    color: #606266;
    font-size: 12px;
}
```

### 3. 在 DAChipsEdit 中的使用

`DAChipsEdit` 内部自动创建和管理 `DAChip` 实例，通常不需要手动创建：

```cpp
auto *edit = new DA::DAChipsEdit(parent);
edit->setTags({ "标签1", "标签2" });  // 内部自动创建 DAChip
```

## 核心属性与方法

| 属性/方法 | 类型 | 说明 |
|----------|------|------|
| `text` / `setText()` | `QString` | 标签文本 |
| `closable` / `setClosable()` | `bool` | 是否显示关闭按钮 |

### 信号

| 信号 | 说明 |
|------|------|
| `closeRequested()` | 关闭按钮被点击 |
| `clicked()` | 标签被点击 |
| `textChanged(QString)` | 文本变化 |
| `closableChanged(bool)` | closable 属性变化 |

## 参考

- [DAChipsEdit 标签输入框](chips-edit.md)
- [DASelectComboBox 下拉多选](select-combobox.md)
