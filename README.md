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

### 折叠面板（Collapse）

对标 Element UI 的 `el-collapse`。

| 类 | 说明 |
|----|------|
| `DACollapseWidget` | 折叠面板容器，管理多个 `DACollapseItem`；支持手风琴模式（仅展开一个）与非手风琴模式（多个同时展开） |
| `DACollapseItem` | 单个折叠项：可点击标题栏 + 内容区域，展开/收起带高度动画；支持禁用、自定义标题控件、自定义内容控件 |
| `DACollapseLayout` | 专为折叠容器设计的自定义 `QLayout`：每个项取其 `sizeHint` 自然高度，不压缩不拉伸，从根本上消除动画过程中的布局抖动 |

### 属性编辑控件（Property Editing）

一组用于属性面板的编辑控件，覆盖颜色、画笔、画刷、字体、对齐、文件路径等常见场景。

| 类 | 说明 |
|----|------|
| `DAColorPickerButton` | 自绘颜色选择按钮，支持调色板菜单与无菜单两种模式 |
| `DAColorMenu` | 颜色选择菜单：标准色板 + 自定义颜色对话框 + 最近使用记录 |
| `DAColorPaletteGridWidget` | Office 风格调色板网格 |
| `DAColorGridWidget` | 基础颜色网格控件 |
| `DAPenEditWidget` | 画笔编辑器：颜色 + 线型 + 线宽 |
| `DAPenStyleComboBox` | `Qt::PenStyle` 选择下拉框 |
| `DABrushEditWidget` | 画刷编辑器：颜色 + 填充样式（实色/图案/渐变/纹理） |
| `DABrushStyleComboBox` | `Qt::BrushStyle` 选择下拉框 |
| `DAFontEditPannelWidget` | 字体编辑面板：字体 + 字号 + 粗斜体下划线 + 颜色 |
| `DAShapeEditPannelWidget` | 形状编辑器：组合画刷（背景）+ 画笔（边框） |
| `DAAligmentEditWidget` | 对齐方式编辑器（水平 + 垂直） |
| `DAAligmentPositionEditWidget` | 九宫格位置对齐编辑器 |
| `DATimeFormatEditWidget` | 时间格式字符串编辑器（含预设格式） |
| `DAFilePathEditWidget` | 文件路径编辑器：文本框 + 浏览按钮 |
| `DAPropertyItemWidget` | 属性行控件：标签 + 编辑器的带标签包装器 |
| `DARecentFilesManager` | 最近文件管理器：持久化到 QSettings，自动过滤无效路径 |
| `DACursorScoped` / `DAWaitCursorScoped` | RAII 光标守卫，异常安全 |

> 后续会持续补充其它 Qt 缺失的界面组件。

## 构建

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<Qt安装路径>
cmake --build build --config Release
```

CMake 自动探测 Qt5 或 Qt6（优先 Qt6）。依赖 **Qt Widgets** 和 **Qt Svg**（用于渲染 SVG 图标）。

### 构建选项

| 选项 | 默认 | 说明 |
|------|------|------|
| `DAWIDGETS_BUILD_SHARED` | `OFF` | `ON` 构建为动态库（dll/so/dylib），`OFF` 为静态库 |
| `DAWIDGETS_BUILD_DEMO` | `ON` | 是否编译示例程序 |
| `DAWIDGETS_BUILD_TESTS` | `OFF` | 是否编译并运行测试 |

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

### DACollapseWidget（折叠面板）

```cpp
#include <DACollapseWidget.h>

auto *collapse = new DACollapseWidget(this);
collapse->setAccordion(false);  // 非手风琴模式，可同时展开多个

auto *item1 = collapse->addItem(QStringLiteral("基本信息"), QStringLiteral("basic"));
auto *item2 = collapse->addItem(QStringLiteral("高级设置"), QStringLiteral("advanced"));

// 在内容区添加自定义控件
item1->contentLayout()->addWidget(new QLabel(QStringLiteral("姓名：")));
item1->contentLayout()->addWidget(new QLineEdit);

// 展开第一个面板
collapse->setActiveNames({ QStringLiteral("basic") });

// 监听展开状态变化
QObject::connect(collapse, &DACollapseWidget::activeNamesChanged,
                 [](const QStringList &names){ qDebug() << "active:" << names; });
```

## 样式

控件均可通过 qss 定制，参考 `examples/*/main.cpp` 中的样式表。
`DASelectComboBox` 构造时自带现代化默认样式（圆角边框 + SVG 箭头），无需额外 qss 即可使用。

## 资源管理

所有图形资源（SVG 图标、箭头等）放在 `src/resources/` 下，通过 Qt RCC `.qrc` 文件统一管理，
CMake 自动链接 **Qt::Svg** 模块以支持 SVG 渲染。
静态库使用者通过 `Q_INIT_RESOURCE(dawidgetsicons)` 显式注册资源，确保链接器保留。

更换/新增资源：
1. 将 SVG/PNG 文件放入 `src/resources/`
2. 更新 `src/resources/dawidgetsicons.qrc`
3. 重新构建

## 示例程序

| 目录 | 说明 |
|------|------|
| `examples/chipsedit_demo/` | `DAChipsEdit` 独立演示 |
| `examples/selectcombobox_demo/` | `DASelectComboBox` 独立演示 |
| `examples/collapse_demo/` | `DACollapseWidget` 独立演示 |
| `examples/all_widgets_demo/` | 全组件综合演示（多标签页：选择器、折叠面板、颜色、画笔画刷、对齐、工具） |

## License

MIT
