# DAWidgets

Qt **QWidget** 界面补充库，灵感来源于 [Element UI](https://element.eleme.io/)。目标是补齐 Qt Widgets 原生控件缺失的常见交互形态，让桌面端也能用上 Web 端习以为常的组件体验。

支持 **Qt5 / Qt6**，使用 **CMake** 构建，纯 C++ 实现。所有公开类以 `DA` 前缀命名。

## 当前组件

### 选择器（Selector）

对标 Element UI 的 `el-select`（`multiple + filterable + allow-create`）。

| 类 | 说明 |
|----|------|
| `DASelectComboBox` | 多标签下拉选择框，**继承 QComboBox**：保留原生箭头/边框/键盘助记键，编辑区替换为 `DAChipsEdit`；filterable 过滤 + allow-create 创建 + default-first-option；高度可随标签增高或固定滚动 |
| `DAChipsEdit` | 流式多标签输入文本框：回车/分隔符提交、Backspace 删除、去重、限量、分隔符枚举+字符串双重载；可独立使用 |
| `DAChip` | 单个可关闭的标签胶囊控件（可独立使用、可 qss 定制） |
| `DAFlowLayout` | 流式布局，末尾可嵌入自适应宽度的输入框 |

> 后续会持续补充其它 Qt 缺失的界面组件。

## 构建

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<Qt安装路径>
cmake --build build --config Release
```

CMake 自动探测 Qt5 或 Qt6（优先 Qt6）。

### 构建选项

| 选项 | 默认 | 说明 |
|------|------|------|
| `DAWIDGETS_BUILD_SHARED` | `OFF` | `ON` 构建为动态库（dll/so/dylib），`OFF` 为静态库 |
| `DAWIDGETS_BUILD_DEMO` | `ON` | 是否编译示例程序 |
| `DAWIDGETS_BUILD_TESTS` | `ON` | 是否编译并运行测试 |

示例：
```bash
cmake -S . -B build -DDAWIDGETS_BUILD_SHARED=ON -DDAWIDGETS_BUILD_TESTS=OFF
```

MSVC 下自动加 `/utf-8`（源码含中文注释）。

## 用法

### DASelectComboBox（filterable 下拉 + allow-create）

```cpp
#include <DASelectComboBox.h>

auto *cb = new DASelectComboBox(this);
cb->setPlaceholder(QStringLiteral("请选择文章标签"));
cb->setOptions({ QStringLiteral("Qt"), QStringLiteral("C++"),
                 QStringLiteral("Python"), QStringLiteral("QML") });
cb->setAllowCreate(true);   // 不在列表中可回车创建
cb->setFilterable(true);    // 输入即过滤（默认 true）
cb->setTags({ QStringLiteral("Qt") });

// 高度策略（可选）
cb->setHeightMode(DASelectComboBox::HeightMode::GrowWithTags);          // 默认：随标签换行增高
// 或
cb->setHeightMode(DASelectComboBox::HeightMode::FixedHeightScrollable); // 固定行数，超出滚动
cb->setFixedRowCount(1);

QObject::connect(cb, &DASelectComboBox::tagsChanged,
                 [](const QStringList &t){ qDebug() << "tags:" << t; });
```

### DAChipsEdit（纯标签输入，可独立使用）

```cpp
#include <DAChipsEdit.h>

auto *edit = new DAChipsEdit(this);
edit->setPlaceholder(QStringLiteral("请选择文章标签"));
edit->setSeparators({ DAChipsEdit::Separator::Comma,
                      DAChipsEdit::Separator::Enter });
edit->setAllowDuplicate(false);
edit->setMaxCount(10);

QObject::connect(edit, &DAChipsEdit::tagAdded,
                 [](const QString &t){ qDebug() << "added:" << t; });
```

## 样式

控件均可通过 qss 定制，参考 `examples/*/main.cpp` 中的样式表。
`DASelectComboBox` 构造时自带现代化默认样式（圆角边框 + PNG 箭头），无需额外 qss 即可使用。

## 资源管理

内嵌资源（如箭头 PNG）由 `tools/generate_resources.py` 从 `tools/*.png` 生成到
`src/dawidgets_resources.{h,cpp}`，避免依赖 Qt RCC（静态库的资源不会自动传递给链接它的可执行文件）。

更换/新增资源：
1. 替换或新增 `tools/` 下的 PNG 文件
2. 运行 `python tools/generate_resources.py`
3. 重新构建

## License

MIT
