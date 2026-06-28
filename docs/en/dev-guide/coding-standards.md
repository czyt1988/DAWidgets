# Coding Standards

This document defines the coding standards for the DAWidgets project, ensuring consistent code style and quality.

## Core Requirements

**Features**

- ✅ **Code Formatting**: Automatic formatting via `.clang-format` configuration
- ✅ **Modern C++ Style**: Uses `override`, `nullptr`, `using`, and other modern syntax
- ✅ **Qt Best Practices**: Follows Qt development guidelines
- ✅ **Qt5/Qt6 Dual Compatibility**: Version differences handled via compatibility macros

## Code Formatting

The project uses a `.clang-format` configuration file for code formatting.

!!! tip "Formatting Recommendations"
    - Run clang-format before committing code
    - Configure your IDE to format on save
    - Stay consistent with existing code style

## Key Coding Standards

### 1. Use Q_SIGNALS / Q_SLOTS

Use the Qt-recommended macros instead of lowercase keywords:

```cpp
// ❌ Wrong: using lowercase keywords
signals:
    void valueChanged(int value);
public slots:
    void updateValue(int value);

// ✅ Correct: using uppercase macros
Q_SIGNALS:
    void valueChanged(int value);
public Q_SLOTS:
    void updateValue(int value);
```

!!! warning "Prohibited"
    Do not use lowercase `signals` / `slots` — some compilers may have issues with them.

### 2. Use override and final

```cpp
// ❌ Wrong
virtual void resizeEvent(QResizeEvent* event);

// ✅ Correct
void resizeEvent(QResizeEvent* event) override;
```

### 3. Use nullptr Instead of NULL

```cpp
// ❌ Wrong
QWidget* widget = NULL;

// ✅ Correct
QWidget* widget = nullptr;
```

### 4. Use static_cast Instead of C-Style Casts

```cpp
// ❌ Wrong: C-style cast
int value = (int)doubleValue;

// ✅ Correct: static_cast
int value = static_cast<int>(doubleValue);
```

### 5. Use using Instead of typedef

```cpp
// ❌ Wrong: old-style typedef
typedef QVector<QString> StringVector;

// ✅ Correct: using alias
using StringVector = QVector<QString>;
```

## Qt Development Best Practices

### Signal-Slot Syntax

Use the Qt5+ function pointer syntax:

```cpp
// ❌ Wrong: using old SIGNAL/SLOT macros
connect(sender, SIGNAL(valueChanged(int)), receiver, SLOT(updateValue(int)));

// ✅ Correct: using function pointer syntax
connect(sender, &Sender::valueChanged, receiver, &Receiver::updateValue);
```

### Q_PROPERTY Usage

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

## Naming Conventions

### Class Names

- Use the `DA` prefix + PascalCase: `DASelectComboBox`, `DACollapseItem`
- All classes are placed in the `DA` namespace

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

### Method Names

- camelCase, first letter lowercase
- Getters omit the `get` prefix: `tags()`, `options()`
- Setters use the `set` prefix: `setTags()`, `setOptions()`

```cpp
QStringList tags() const;
void setTags(const QStringList& tags);
```

### Variable Names

- Member variables (in PIMPL private data classes) have no special prefix: `title`, `value`
- Local variables use camelCase: `sampleCount`
- Constants are ALL_CAPS: `MAX_COUNT`

### Enumerations

- Use `enum class`, PascalCase naming
- Enum values are PascalCase

```cpp
enum class HeightMode
{
    GrowWithTags,
    FixedHeightScrollable
};
Q_ENUM(HeightMode)
```

## qss Selector Convention

For custom widgets under the `DA` namespace, qss selectors use the `DA--ClassName` form (Qt uses `--` in place of `::`):

```css
DA--DAChip {
    border: 1px solid #dcdfe6;
    border-radius: 4px;
}
DA--DACollapseItem::header {
    background: #fafafa;
}
```

## API Design Conventions

### Enum-First

Fixed-value parameters provide both an **enum overload** and a **string overload**:

```cpp
// Enum overload (compile-time type safety, IDE auto-completion)
void setSeparators(QList<Separator> separators);

// String overload (runtime configuration, UI dropdowns, serialization scenarios)
void setSeparators(const QString& separators);
```

!!! info "Design Philosophy"
    Enums provide compile-time type safety — users know the supported values without consulting documentation. String APIs are retained for runtime configuration scenarios.

## Related Documents

- [Comment Standards](comment-standards.md)
- [PIMPL Pattern](pimpl-pattern.md)
- [Qt5/Qt6 Compatibility](qt5-qt6-compat.md)
