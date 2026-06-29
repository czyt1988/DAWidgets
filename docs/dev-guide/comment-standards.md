# 注释规范

本文档定义 DAWidgets 项目的代码注释标准，确保文档与代码之间的一致性和完整性。

## 语言要求

!!! info "中文注释"
    DAWidgets 项目源码注释使用**中文**撰写（与项目定位和开发者习惯一致）。Doxygen 文档注释同样使用中文。

    这与 Qwt 等国际化项目的英文注释策略不同——DAWidgets 主要面向中文开发者社区。

## 核心要求

**特性**

- ✅ **Doxygen 格式**：使用 Doxygen 标准格式注释
- ✅ **中文注释**：源码注释使用中文
- ✅ **放置规则**：详细注释放 .cpp，简明注释放 .h
- ✅ **完整性**：类、函数、信号均需完整文档注释

## Doxygen 关键字标准

Doxygen 关键字使用 `@` 前缀（`@brief`、`@param`、`@return`、`@details`）：

```cpp
/**
 * @brief 简要描述
 * @param name 参数描述
 * @return 返回值描述
 */
```

## 注释放置原则

!!! info "核心原则"
    - **详细注释**放在 `.cpp` 文件
    - **简明注释**放在 `.h` 文件
    - 头文件保持干净，避免过多注释内容

## 头文件注释标准

头文件中 `public` 函数声明只写**单行中文简明注释**。

### 简明注释示例

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

!!! warning "注意"
    **不要**在头文件的成员函数中写详细的 Doxygen 注释块。详细内容应放在对应的 `.cpp` 文件中。以下四类情况 **允许** 在头文件的成员函数中写详细的 Doxygen 注释块：类注释、枚举注释、Qt信号函数注释、宏定义的注释。

## 类注释标准

类 Doxygen 注释添加在头文件中，包含 `@brief`、`@details` 和用法示例。

### 类注释模板

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

### 类注释要点

- `@brief`：一句话说明类的用途
- `@details`：详细说明类的行为、模式、特性
- 用法示例：`@code` / ` ```cpp ` 代码块
- 对标说明：如对标 Element UI 的哪个组件

## 源文件注释标准

### 函数注释模板

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

### 参数方向标注

参数方向标注**推荐但不强制**。值传递和 const 引用参数方向明显，可省略 `[in]`。

以下情况**必须**标注方向：

- `@param[out]` — 非 const 引用/指针用于输出结果
- `@param[in,out]` — 非 const 引用/指针既读又写

```cpp
/**
 * @brief 计算内容区自然高度
 * @param[out] naturalHeight 输出计算得到的自然高度
 * @return 是否计算成功
 */
bool calculateContentHeight(int& naturalHeight) const;
```

## 信号注释标准

Qt 信号声明在头文件中，没有对应的 cpp 定义，因此信号注释必须**完整写在头文件**。

### 信号注释示例

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

## 注释风格检查清单

| 检查项 | 位置 | 要求 |
|--------|------|------|
| 类注释 | 头文件 `.h` | 中文 Doxygen，含用法示例 |
| 公开函数详细注释 | 源文件 `.cpp` | 中文 Doxygen |
| 公开函数简明注释 | 头文件 `.h` | 单行中文 |
| 信号注释 | 头文件 `.h` | 中文 Doxygen（完整） |
| 私有/保护函数 | 源文件 `.cpp` | 可选，推荐中文 |
| 行内注释 | 任意位置 | 中文，解释"为什么" |

## 常见错误

### ❌ 错误：头文件中写详细注释

```cpp
// 错误示例 — 头文件中注释过多
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

### ❌ 错误：注释只说"是什么"不说"为什么"

```cpp
// 错误示例 — 无意义的注释
d->options = options;  // 设置 options
```

### ✅ 正确：头文件简明 + cpp 详细

头文件 `DASelectComboBox.h`：

```cpp
class DASelectComboBox : public QComboBox
{
public:
    // 设置候选选项列表
    void setOptions(const QStringList& options);
};
```

源文件 `DASelectComboBox.cpp`：

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

## 相关文档

- [编码规范](coding-standards.md)
- [PIMPL 模式](pimpl-pattern.md)
