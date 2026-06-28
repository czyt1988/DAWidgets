# 编码规范

本文档定义 DAWidgets 项目的编码规范，确保代码风格一致和质量。

## 核心要求

**特性**

- ✅ **代码格式化**：使用 `.clang-format` 配置自动格式化
- ✅ **现代 C++ 风格**：使用 `override`、`nullptr`、`using` 等现代语法
- ✅ **Qt 最佳实践**：遵循 Qt 开发指南
- ✅ **Qt5/Qt6 双兼容**：通过兼容宏处理版本差异

## 代码格式化

项目使用 `.clang-format` 配置文件进行代码格式化。

!!! tip "格式化建议"
    - 提交代码前运行 clang-format
    - 在 IDE 中配置保存时自动格式化
    - 保持与现有代码风格一致

## 关键编码规范

### 1. 使用 Q_SIGNALS / Q_SLOTS

使用 Qt 官方推荐的宏替代小写关键字：

```cpp
// ❌ 错误：使用小写关键字
signals:
    void valueChanged(int value);
public slots:
    void updateValue(int value);

// ✅ 正确：使用大写宏
Q_SIGNALS:
    void valueChanged(int value);
public Q_SLOTS:
    void updateValue(int value);
```

!!! warning "禁止"
    不要使用小写 `signals` / `slots`，某些编译器可能出问题。

### 2. 使用 override 和 final

```cpp
// ❌ 错误
virtual void resizeEvent(QResizeEvent* event);

// ✅ 正确
void resizeEvent(QResizeEvent* event) override;
```

### 3. 使用 nullptr 替代 NULL

```cpp
// ❌ 错误
QWidget* widget = NULL;

// ✅ 正确
QWidget* widget = nullptr;
```

### 4. 使用 static_cast 替代 C 风格转换

```cpp
// ❌ 错误：C 风格转换
int value = (int)doubleValue;

// ✅ 正确：static_cast
int value = static_cast<int>(doubleValue);
```

### 5. 使用 using 替代 typedef

```cpp
// ❌ 错误：旧式 typedef
typedef QVector<QString> StringVector;

// ✅ 正确：using 别名
using StringVector = QVector<QString>;
```

## Qt 开发最佳实践

### 信号槽语法

使用 Qt5+ 的函数指针语法：

```cpp
// ❌ 错误：使用旧 SIGNAL/SLOT 宏
connect(sender, SIGNAL(valueChanged(int)), receiver, SLOT(updateValue(int)));

// ✅ 正确：使用函数指针语法
connect(sender, &Sender::valueChanged, receiver, &Receiver::updateValue);
```

### Q_PROPERTY 使用

```cpp
class DAChip : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool closable READ closable WRITE setClosable NOTIFY closableChanged)

public:
    QString text() const;
    void setText(const QString& text);

    bool closable() const;
    void setClosable(bool on);

Q_SIGNALS:
    void textChanged(const QString& text);
    void closableChanged(bool on);
};
```

## 命名约定

### 类名

- 使用 `DA` 前缀 + 大驼峰：`DASelectComboBox`、`DACollapseItem`
- 所有类放入 `DA` 命名空间

```cpp
namespace DA
{
class DASelectComboBox : public QComboBox
{
    Q_OBJECT
    // ...
};
}  // namespace DA
```

### 方法名

- 小驼峰，首字母小写
- Getter 不加 `get` 前缀：`tags()`、`options()`
- Setter 加 `set` 前缀：`setTags()`、`setOptions()`

```cpp
QStringList tags() const;
void setTags(const QStringList& tags);
```

### 变量名

- 成员变量（PIMPL 私有数据类中）无特殊前缀：`title`、`value`
- 局部变量小驼峰：`sampleCount`
- 常量全大写：`MAX_COUNT`

### 枚举

- 枚举类（`enum class`），大驼峰命名
- 枚举值大驼峰

```cpp
enum class HeightMode
{
    GrowWithTags,
    FixedHeightScrollable
};
Q_ENUM(HeightMode)
```

## qss 选择器约定

DA 命名空间下的自定义控件，qss 选择器使用 `DA--ClassName` 形式（Qt 用 `--` 替代 `::`）：

```css
DA--DAChip {
    border: 1px solid #dcdfe6;
    border-radius: 4px;
}
DA--DACollapseItem::header {
    background: #fafafa;
}
```

## API 设计约定

### 枚举优先

固定取值的参数提供**枚举重载** + **字符串重载**：

```cpp
// 枚举重载（编译期类型安全，IDE 可自动补全）
void setSeparators(QList<Separator> separators);

// 字符串重载（运行时配置、UI 下拉框、序列化场景）
void setSeparators(const QString& separators);
```

!!! info "设计理念"
    枚举带来编译期类型安全，用户无需翻文档就知道支持哪些取值。字符串 API 保留给运行时配置场景。

## 相关文档

- [注释规范](comment-standards.md)
- [PIMPL 模式](pimpl-pattern.md)
- [Qt5/Qt6 兼容](qt5-qt6-compat.md)
