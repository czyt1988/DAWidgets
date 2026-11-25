# 更新日志

本文件记录 DAWidgets 的重要变更，版本号遵循[语义化版本](https://semver.org/lang/zh-CN/)。

## 未发布

*暂无未发布的变更。*

## tag:v0.1.0 (2026-11-25)

### 新功能

- **Chips：粘贴拆分与紧凑标签。** `DAChipsEdit` 支持把粘贴文本拆成多个标签、对过长标签做省略并显示 tooltip、显示已选计数，并在输入法组合期间屏蔽 Enter，避免选候选词时误提交标签。
- **折叠面板：键盘导航与外观变体。** `DACollapseWidget` 支持方向键导航、在展开前发射 `expandRequested(QString)`，并提供 `Default`、`Borderless`、`Card` 三种外观；`DACollapseItem` 暴露头部控件并支持换行内容的高度重算。
- **选择器：分组选项与空态。** `DASelectComboBox` 支持分组选项列表、可配置的空态面板（可带创建按钮）、清空全部按钮，并把超出的标签折叠成 `+N`。
- **条目列表：拖拽排序与过滤。** `DAItemListEditWidget` 增加手柄拖拽排序（`orderChanged`）、只隐藏行的过滤框、空态文案，以及「移除所有」的二次点击确认模式。
- **颜色：透明度、最近色与默认色。** `DAColorMenu` 增加带棋盘格预览的透明度滑杆、去重后最多 5 条的最近色策略和「恢复默认」动作；`DAColorGridWidget` 增加可选的键盘导航与半透明色块棋盘格底。
- **文件路径：拖放、校验与最近路径。** `DAFilePathEditWidget` 接受拖入的文件/目录，按模式校验路径（`SaveFile` 允许目标不存在）并显示状态行与 `invalid` 属性，同时提供最近路径下拉。
- **字体编辑：逐项预览与最近字体。** `DAFontEditPannelWidget` 可用面板当前字号/字重/风格渲染字体下拉的每一项，并显示由宿主注入的最近字体菜单；库内不自动记录。
- **过滤代理：供 delegate 使用的命中区间。** `DAStringFilterProxyModel::matchRanges()` 与静态 `matchRangesIn()` 返回过滤词的 `(start, length)` 区间，delegate 可据此自行高亮；demo 提供最小高亮 delegate。
- **勾选列表：Shift 连选与 Ctrl+A。** `DACheckListWidget` 支持 Shift+点击区间连选（跳过 forced、禁用与无复选框条目）以及 Ctrl+A 全选当前可见条目，整段只发一次变更信号。
- **撤销历史：时间、状态点与空命令过滤。** `DAUndoHistoryWidget` 可显示每条命令的本地入栈时间、已应用/可重做状态点，提供 `commandTimestamp()` 查询，并隐藏文本为空的命令。
- **文本编辑：行号与查找条。** `DATextEditWidget` 增加可选行号栏和 Ctrl+F 查找条，支持环绕查找、命中计数、区分大小写、全部命中高亮，只读模式下同样可用。
- **文档与画廊。** 中英文指南覆盖所有新选项，每个控件场景都有双语文档和更新的画廊截图，单文件合并版已重新生成。

### Bug 修复

- **Qt 5 字体预览崩溃。** Qt 5 的 `QComboBox::setItemDelegate()` 会删除旧委托，导致字体预览关闭时无法恢复原生委托；现在直接把预览委托装到 view 上，Qt 的委托得以保留并被精确还原。
- **文件路径编辑器析构。** `DAFilePathEditWidget` 在删除 UI 前断开输入框信号，避免析构期间触发 `editingFinished` 访问已释放控件。
- **流式布局与隐藏项。** `DAFlowLayout` 跳过空 item，隐藏的占位项不再在标签编辑器和弹出空态里占位。

### 文档

- 新增中英文小节：透明度与最近色、文件路径拖放/校验/历史、字体预览与最近字体、过滤命中区间与高亮 delegate、键盘区间连选、撤销时间与空命令兜底、文本编辑行号与查找。
- 更新受影响控件的画廊截图，并写明 `(start, length)` 使用 UTF-16 下标、空过滤返回空区间。

## tag:v0.0.1 (2026-11-23)

### 新功能

- **首个公开发布版本。** 0.0.1 版以 MIT 许可证发布整个库。库使用 C++17 编写，基于 Qt Widgets 与 Qt Svg，使用 CMake 3.16 及以上版本构建，兼容 Qt 5 与 Qt 6（已在 Qt 5.14.2 与 Qt 6.7.3 上验证）。控件交互参考 Element UI 的形态，所有公开类均以 `DA` 为前缀。
- **选择器家族。** `DASelectComboBox` 在 `QComboBox` 基础上提供多标签选择、弹出层过滤、allow-create 与 default-first-option，并提供 `GrowWithTags` 与 `FixedHeightScrollable` 两种高度模式，同时保留原生箭头、边框与键盘助记键。标签显示在 `DAChipsEdit` 中——一个流式布局的标签输入框（回车或分隔符提交、Backspace 删除最后一个标签、去重、数量上限、枚举与字符串两种分隔符重载），由可关闭的 `DAChip` 胶囊组成；`DAFlowLayout` 负责换行并让末尾输入框占满剩余宽度。
- **折叠面板家族。** `DACollapseWidget` 纵向排列 `DACollapseItem`，支持展开/收起动画、手风琴与多开两种模式、`activeNames()` 状态、调用方自定义的标题与内容控件以及禁用项。`DACollapseLayout` 让每个条目保持自然 `sizeHint()`，从而保证动画稳定。
- **属性编辑套件。** 覆盖属性面板常用场景的编辑器：颜色（`DAColorPickerButton`、`DAColorMenu`、`DAColorPaletteGridWidget`、`DAColorGridWidget`）、画笔与画刷（`DAPenEditWidget`、`DABrushEditWidget`，以及用代码生成图标的 `DAPenStyleComboBox`、`DABrushStyleComboBox`）、字体（`DAFontEditPannelWidget`，支持两排与单排两种布局）、形状（`DAShapeEditPannelWidget`）、对齐（`DAAligmentEditWidget`、`DATextAlignmentEditWidget`、`DAAligmentPositionEditWidget`）、时间格式（`DATimeFormatEditWidget`）、文件路径（`DAFilePathEditWidget`，支持打开文件/保存文件/选择目录三种模式）、纯文本（`DATextEditWidget`）、带标签的属性行（`DAPropertyItemWidget`）以及 RAII 光标守卫（`DACursorScoped`、`DAWaitCursorScoped`）。
- **条目选择与预览家族。** `DAItemEntry` 用展示文本与标识数据表示候选/选中条目；`DAItemListEditWidget` 编辑已选列表（行内删除、候选下拉菜单或可搜索的勾选对话框、二次确认的移除所有、可选上下排序）；`DAItemListPickDialog` 封装 `DACheckListWidget`，支持全选、反选与 forced 条目的批量勾选；`DAStringListEditWidget` 编辑纯字符串列表，支持行内追加或由调用方驱动追加；`DAColorScalePreviewWidget` 绘制双色/三色渐变预览条；`DAStringFilterProxyModel` 按展示文本或自定义角色过滤列表与树模型，支持大小写策略、过滤列范围以及树形模型父节点放行。
- **工具组件。** `DAUndoHistoryWidget` 把跟随 `QUndoGroup` 的 `QUndoView` 与撤销深度输入框组合在一起，输入框变化时发出 `undoLimitChangeRequested`（持久化由调用方负责）。`DAAbstractWidgetOverlay` 是向其他控件之上绘制遮罩层的抽象基类，提供 `NoMask`、`MaskHint`、`AlphaMask` 三种遮罩模式与可配置的渲染模式。
- **自包含的 C++ 接口。** 导出宏（`DAWIDGETS_EXPORT`）与 PIMPL 宏（`DA_DECLARE_PRIVATE`、`DA_DECLARE_PUBLIC`、`DA_D`、`DA_Q`）手写在 `DAWidgetsGlobals.h` 中，公开头文件既不依赖 CMake `GenerateExportHeader`，也不依赖构建目录；`DAQt5Qt6Compat.h` 封装了 Qt 5 与 Qt 6 的 API 差异。SVG 图标通过 Qt RCC 内嵌并由 Qt Svg 渲染，运行时不依赖外部图片文件。
- **全控件示例与截图导出。** `examples/all_widgets_demo` 用 9 个标签页展示全部公开控件，页内再用 `DACollapseWidget` 分组；`--export-gallery` 导出主窗口、各标签页、各控件场景以及文档使用的弹层特写，`--shot` 可单独保存某个标签页，`--help` 打印命令行选项。
- **QtTest 测试套件。** 12 套 QtTest 测试接入 ctest，覆盖选择器、折叠面板、属性编辑、条目选择、文本编辑、过滤代理、遮罩层与撤销历史等行为。
- **单文件合并版。** `src-amalgamate/DAWidgets.h` 与 `src-amalgamate/DAWidgets.cpp` 把整个库合并为两个文件，无需 CMake、子模块或预编译库即可直接加入项目；`examples/amalgamate_demo` 只使用 Qt Widgets 与 Qt Svg 编译这对文件，用于验证其可独立使用。

### Bug 修复

- **GCC/Clang 下静态库可以正常链接。** 三个控件构造函数里的 `Q_INIT_RESOURCE(dawidgetsicons)` 展开在 `namespace DA` 内，GCC/Clang 因此声明出 `DA::qInitResources_dawidgetsicons()`，而 rcc 生成的定义是全局符号，静态链接的使用者（包括 Linux CI）会报 undefined reference。现在资源注册统一走 `DA::ensureResourcesInitialized()`（`src/DAWidgetsResources.cpp`），其 `Q_INIT_RESOURCE` 展开位于全局命名空间，并且只执行一次。
- **折叠面板展开/收起不再抖动或闪烁。** 内容高度改用 `heightForWidth()` 计算，自动换行内容能被正确测量，动画结束后再锁定高度；`DACollapseLayout` 按每个条目的自然 `sizeHint()` 布局，不再让 `QVBoxLayout` 在动画过程中压缩或拉伸各行。
- **公开头文件可独立使用。** 此前每个公开头文件都会引用 CMake `GenerateExportHeader` 生成到构建目录中的 `DAWIDGETS_EXPORT.h`；现在导出宏定义在 `DAWidgetsGlobals.h` 里，公开包含路径中也去掉了构建目录，qmake 等其它构建系统可以直接使用这些头文件。
- **过滤时不再丢失行内编辑器。** 给承载行内控件（例如 `QTreeWidget` 行内的 `QKeySequenceEdit`）的视图挂上 `DAStringFilterProxyModel`，此前过滤过程中这些编辑器会丢失。新增的静态方法 `matchesText()` 把代理的匹配规则开放出来，这类视图可以保留自己的模型，并用完全相同的语义进行过滤。
- **水平与垂直对齐可以分别设置。** 单选按钮组无法表达“右对齐 + 顶端对齐”这类组合；`DATextAlignmentEditWidget` 提供两组独立互斥按钮（左/居中/右与顶端/居中/底端），组合为一个 `Qt::Alignment` 值。水平居中改用专用图标，因为原先的居中图标实际表示垂直居中。
- **清理已弃用的 Qt 接口。** Qt 版本判断改为以 5.15 为界，`QButtonGroup` 优先使用 `idClicked` 而非已弃用的 `buttonClicked(int)`；`DASelectComboBox` 不再调用 `QApplication::globalStrut()`（该接口在 Qt 5.15 弃用、Qt 6 移除）。
- **渐变预览重绘不再逐帧分配内存。** `DAColorScalePreviewWidget` 原先在每次 `paintEvent` 中构造 `QLinearGradient` 与画刷；现在缓存画刷，只在颜色或控件尺寸变化时重建。

### 构建

- **支持 CMake 安装与 `find_package`。** 构建系统会安装库、头文件与导出目标，并生成 `DAWidgetsConfig.cmake` 与 `DAWidgetsConfigVersion.cmake`，使用方可在安装目录中通过 `find_package(DAWidgets)` 找到并链接 `DA::DAWidgets`。
- **静态库与动态库两种构建。** `DAWIDGETS_BUILD_SHARED` 选择静态库或动态库；手写导出宏由 `DA_BUILT_AS_STATIC` 与 `DAWIDGETS_EXPORTS` 控制。Windows 动态库构建会把运行时 DLL 复制到每个测试与示例可执行文件旁，`ctest` 无需修改 `PATH` 即可从构建目录运行。
- **Qt 自动探测与编译选项。** CMake 优先探测 Qt 6，找不到时回退到 Qt 5（`find_package(QT NAMES Qt6 Qt5 ...)`），公开链接 Qt Widgets 与 Qt Svg，启用 AUTOMOC、AUTORCC 与 AUTOUIC，要求 C++17，并在 MSVC 下强制 `/utf-8` 以正确处理中文注释与字面量。
- **Git 工作树脚本。** `scripts/worktree-add.ps1` 用于创建、列出与删除 `.worktree/` 下的任务工作树，并从主工作树的 CMake 缓存继承生成器、Qt 路径与 `DAWIDGETS_BUILD_*` 选项，预配置新工作树的 `build/` 目录。
- **GitHub Actions。** CI 工作流负责构建并测试库、部署中英文 mkdocs 文档站，并把发布版本同步到 Gitee。

### 文档

- **中英文 mkdocs 文档站。** 使用 MkDocs Material 主题与 i18n 插件搭建：中文位于根路径，英文位于 `/en/`，包含首页、控件画廊、构建指南、开发指引与使用指南等栏目。
- **使用指南覆盖全部公开控件。** 使用指南中英文各有完整一套页面，覆盖全部公开控件，包含特性列表、API 表格、代码示例与效果截图；文档同时说明单文件合并版的使用与重新生成方式。
- **开发指引与文档编写指南。** 编码规范、注释规范、PIMPL 模式、Qt 5/Qt 6 兼容规则、多工作树并行开发以及文档编写指南，说明了库的开发方式，以及页面、截图与导航如何保持一致。
- **自动生成的控件画廊。** `docs/gallery.md` 与 `docs/en/gallery.md` 汇集主窗口、9 个标签页、各控件场景与弹层特写，全部由 `--export-gallery` 导出，截图随控件更新重新生成，而非手工维护。
- **中英文 README。** `README.md`（英文）与 `README-CN.md`（中文）介绍库的定位、安装方式（`find_package`、`add_subdirectory`、单文件合并版、从源码构建）、快速上手示例，以及文档、GitHub 与 Gitee 链接。
