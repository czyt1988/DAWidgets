# Comment Standards

This document defines the code comment standards for the DAWidgets project, ensuring consistency and completeness between documentation and code.

## Language Requirement

!!! info "Chinese Comments"
    The DAWidgets project uses **Chinese** for source code comments and Doxygen documentation comments.

    This differs from the English-comment strategy used by internationally-oriented projects such as Qwt — DAWidgets is primarily targeted at the Chinese developer community. The rationale:

    - The project's primary audience and maintainer community are Chinese-speaking developers.
    - Comments explain *why* decisions were made, and expressing the reasoning in the maintainer's native language reduces ambiguity and improves review quality.
    - The English documentation (in `docs/en/`) is a translation layer for API consumers; the in-source comments remain Chinese so that the original intent is preserved without loss during translation.

    **Practical implication for contributors:** When writing or modifying code, write all inline comments, Doxygen blocks, and class documentation in Chinese. Do not translate existing Chinese comments to English unless explicitly asked.

## Core Requirements

**Features**

- ✅ **Doxygen Format**: Use Doxygen standard format for comments
- ✅ **Chinese Comments**: Source code comments are written in Chinese
- ✅ **Placement Rules**: Detailed comments in `.cpp`, concise comments in `.h`
- ✅ **Completeness**: Classes, functions, and signals all require complete documentation comments

## Doxygen Keyword Standard

Doxygen keywords use the `@` prefix (`@brief`, `@param`, `@return`, `@details`):

```cpp
/**
 * @brief Brief description
 * @param name Parameter description
 * @return Return value description
 */
```

## Comment Placement Principles

!!! info "Core Principle"
    - **Detailed comments** go in `.cpp` files
    - **Concise comments** go in `.h` files
    - Headers should stay clean, avoiding excessive comment content

## Header File Comment Standards

In header files, `public` function declarations only carry **single-line concise Chinese comments**.

### Concise Comment Example

```cpp
class DASelectComboBox : public QComboBox
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DASelectComboBox)
public:
    // 构造函数
    explicit DASelectComboBox(QWidget* parent = nullptr);

    // 设置候选选项列表
    void setOptions(const QStringList& options);

    // 获取已选标签列表
    QStringList tags() const;
};
```

!!! warning "Note"
    **Do not** write detailed Doxygen comment blocks in header files. Detailed content should go in the corresponding `.cpp` file.

## Class Comment Standards

Class Doxygen comments are placed in the header file, containing `@brief`, `@details`, and a usage example.

### Class Comment Template

```cpp
/**
 * @brief 折叠面板容器，对标 Element UI el-collapse
 *
 * @details 纵向排列多个 DACollapseItem，并协调展开状态：
 *  - 非手风琴模式（默认）：多个面板可同时展开
 *  - 手风琴模式：同时只能展开一个
 *
 * 用法：
 * ```cpp
 * auto *c = new DA::DACollapse;
 * c->addItem(QStringLiteral("一致性"), QStringLiteral("1"));
 * c->setActiveNames({ QStringLiteral("1") });
 * ```
 */
class DAWIDGETS_EXPORT DACollapse : public QWidget
{
    Q_OBJECT
    // ...
};
```

### Class Comment Key Points

- `@brief`: One sentence describing the class's purpose
- `@details`: Detailed description of the class's behavior, modes, and features
- Usage example: `@code` / ` ```cpp ` code block
- Component mapping: If applicable, note which Element UI component it corresponds to

## Source File Comment Standards

### Function Comment Template

```cpp
/**
 * @brief 设置候选选项列表
 * @param options 候选选项字符串列表
 * @details 此方法替换当前的所有候选选项。如果已选标签中有不在新候选列表中的，
 *          这些标签会被保留（不会自动移除）。调用后会刷新弹出列表。
 */
void DASelectComboBox::setOptions(const QStringList& options)
{
    DA_D(d);
    d->options = options;
    // ...
}
```

### Parameter Direction Annotations

Parameter direction annotations are **recommended but not mandatory**. For pass-by-value and const reference parameters, the direction is obvious and `[in]` can be omitted.

Direction **must** be annotated in the following cases:

- `@param[out]` — non-const reference/pointer used for output results
- `@param[in,out]` — non-const reference/pointer that is both read and written

```cpp
/**
 * @brief 计算内容区自然高度
 * @param[out] naturalHeight 输出计算得到的自然高度
 * @return 是否计算成功
 */
bool calculateContentHeight(int& naturalHeight) const;
```

## Signal Comment Standards

Qt signals are declared in header files and have no corresponding cpp definition, so signal comments must be **written completely in the header file**.

### Signal Comment Example

```cpp
class DASelectComboBox : public QComboBox
{
Q_SIGNALS:
    /**
     * @brief 标签列表变化时发射
     * @param tags 当前的标签列表
     */
    void tagsChanged(const QStringList& tags);

    /**
     * @brief 用户选中一个候选选项时发射
     * @param option 被选中的选项文本
     */
    void optionSelected(const QString& option);
};
```

## Comment Style Checklist

| Check Item | Location | Requirement |
|------------|----------|-------------|
| Class comment | Header `.h` | Chinese Doxygen, includes usage example |
| Public function detailed comment | Source `.cpp` | Chinese Doxygen |
| Public function concise comment | Header `.h` | Single-line Chinese |
| Signal comment | Header `.h` | Chinese Doxygen (complete) |
| Private/protected functions | Source `.cpp` | Optional, Chinese recommended |
| Inline comments | Any location | Chinese, explain "why" |

## Common Mistakes

### ❌ Wrong: Detailed comments in header file

```cpp
// Wrong example — too many comments in header
class MyClass {
public:
    /**
     * @brief 构造函数
     * @param parent 父控件指针
     * @details 此构造函数初始化控件并设置内部数据结构...
     */
    MyClass(QWidget* parent = nullptr);
};
```

### ❌ Wrong: Comments that say "what" not "why"

```cpp
// Wrong example — meaningless comment
d->options = options;  // 设置 options
```

### ✅ Correct: Concise header + detailed cpp

Header `DASelectComboBox.h`:

```cpp
class DASelectComboBox : public QComboBox
{
public:
    // 设置候选选项列表
    void setOptions(const QStringList& options);
};
```

Source `DASelectComboBox.cpp`:

```cpp
/**
 * @brief 设置候选选项列表
 * @param options 候选选项字符串列表
 * @details 替换当前所有候选选项。已选标签中不在新列表内的会保留。
 *          调用后刷新弹出列表。
 */
void DASelectComboBox::setOptions(const QStringList& options)
{
    DA_D(d);
    d->options = options;
}
```

## Related Documents

- [Coding Standards](coding-standards.md)
- [PIMPL Pattern](pimpl-pattern.md)
