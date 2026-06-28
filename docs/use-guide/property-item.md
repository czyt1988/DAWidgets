# DAPropertyItemWidget 属性项

`DAPropertyItemWidget` 是属性编辑面板的基础容器控件，提供属性名称、描述和编辑器区域的统一布局，支持 Inline（名称与编辑器同行）和 Below（名称在上、编辑器在下）两种布局模式。

## 核心特性

**特性**

- ✅ **双布局模式**：InlineLayout / BelowLayout
- ✅ **属性描述**：支持属性名 + 描述文本
- ✅ **任意编辑器**：通过 `setEditorWidget()` 嵌入任意编辑控件
- ✅ **propertyId 机制**：每个属性项有唯一 ID，便于批量管理
- ✅ **边框可见性**：可控制是否显示边框

## 用法

### 1. 基本用法

```cpp
#include "DAPropertyItemWidget.h"

// 创建属性项
auto *prop = new DA::DAPropertyItemWidget(parent);
prop->setPropertyName("字体大小");
prop->setPropertyDescription("设置文本的字体大小（磅）");
prop->setPropertyId(1001);

// 嵌入编辑器（任意 QWidget 子类）
auto *spinBox = new QSpinBox;
spinBox->setRange(6, 72);
spinBox->setValue(12);
prop->setEditorWidget(spinBox);

// 值变化信号
QObject::connect(prop, &DA::DAPropertyItemWidget::valueChanged,
    [](int propertyId) {
        qDebug() << "属性" << propertyId << "值变化";
    });
```

### 2. 布局模式

```cpp
using namespace DA;

// 模式一：Inline — 名称和编辑器在同一行
prop->setLayoutMode(DAPropertyItemWidget::LayoutMode::InlineLayout);

// 模式二：Below — 名称在上，编辑器在下
prop->setLayoutMode(DAPropertyItemWidget::LayoutMode::BelowLayout);
```

| 模式 | 布局 | 适用场景 |
|------|------|---------|
| `InlineLayout` | 名称与编辑器同行 | 简单编辑器（输入框、下拉框） |
| `BelowLayout` | 名称在上，编辑器在下 | 复杂编辑器（颜色选择、多控件组合） |

### 3. 构建属性面板

```cpp
// 典型属性面板构建
auto *panel = new QWidget;
auto *layout = new QVBoxLayout(panel);

// 属性 1：字体
auto *fontProp = new DA::DAPropertyItemWidget;
fontProp->setPropertyName("字体");
fontProp->setPropertyId(1);
fontProp->setEditorWidget(new QFontComboBox);
layout->addWidget(fontProp);

// 属性 2：颜色
auto *colorProp = new DA::DAPropertyItemWidget;
colorProp->setPropertyName("颜色");
colorProp->setPropertyId(2);
colorProp->setEditorWidget(new DA::DAColorPickerButton);
layout->addWidget(colorProp);

layout->addStretch();
```

## 核心属性与方法

### Q_PROPERTY

| 属性 | 类型 | 说明 |
|------|------|------|
| `propertyId` | `int` | 属性唯一标识 |
| `propertyName` | `QString` | 属性名称 |
| `propertyDescription` | `QString` | 属性描述 |
| `layoutMode` | `LayoutMode` | 布局模式 |
| `frameVisible` | `bool` | 边框是否可见 |

### 核心方法

| 方法 | 说明 |
|------|------|
| `setEditorWidget(QWidget*)` | 设置编辑器控件 |
| `editorWidget()` | 获取当前编辑器 |
| `setLayoutMode(LayoutMode)` | 设置布局模式 |
| `setNameLabelWidth(int)` | 设置名称标签宽度 |
| `setFrameVisible(bool)` | 设置边框可见性 |

## 参考

- [对齐编辑器](alignment-edit.md)
- [颜色菜单与拾取按钮](color-menu.md)
