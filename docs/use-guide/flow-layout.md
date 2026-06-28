# DAFlowLayout 流式布局

`DAFlowLayout` 是一个流式布局，子控件按从左到右、从上到下的顺序排列，超出宽度自动换行。末尾项可设置为自适应宽度填满剩余空间。

## 核心特性

**特性**

- ✅ **自动换行**：子控件超出容器宽度时自动换到下一行
- ✅ **末尾扩展**：`setTrailingExpanding(true)` 使末尾项填满剩余宽度
- ✅ **heightForWidth 支持**：根据宽度动态计算所需高度
- ✅ **独立可用**：可用于任何需要流式排列的场景

## 用法

### 1. 基本用法

```cpp
#include "DAFlowLayout.h"

auto *layout = new DA::DAFlowLayout;

// 添加多个控件，自动换行排列
for (int i = 0; i < 10; ++i) {
    layout->addWidget(new QPushButton(QString("按钮 %1").arg(i)));
}

container->setLayout(layout);
```

### 2. 末尾扩展模式

在 `DAChipsEdit` 中，输入框作为末尾项需要填满剩余宽度：

```cpp
auto *layout = new DA::DAFlowLayout;

// 启用末尾扩展
layout->setTrailingExpanding(true);

// 添加标签控件...
layout->addWidget(chip1);
layout->addWidget(chip2);

// 最后添加输入框 — 它会填满当前行的剩余宽度
layout->addWidget(inputField);
```

!!! info "工作原理"
    当 `trailingExpanding` 为 true 时，布局会计算每行剩余宽度，将末尾项的宽度设置为剩余宽度（如果末尾项在该行）。如果末尾项独占一行，则占满整行宽度。

### 3. 间距与对齐

```cpp
auto *layout = new DA::DAFlowLayout;

// 设置控件间距
layout->setSpacing(8);

// 设置边距
layout->setContentsMargins(4, 4, 4, 4);
```

## 核心方法

| 方法 | 说明 |
|------|------|
| `setTrailingExpanding(bool)` | 启用/禁用末尾项扩展 |
| `trailingExpanding()` | 获取末尾扩展状态 |
| `setSpacing(int)` | 设置控件间距 |
| `heightForWidth(int)` | 根据宽度计算所需高度 |

## 参考

- [DAChipsEdit 标签输入框](chips-edit.md)
- [DAChip 标签胶囊](chip.md)
