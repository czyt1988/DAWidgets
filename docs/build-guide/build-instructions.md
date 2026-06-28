# DAWidgets 构建说明

本文详细介绍如何使用 CMake 构建 DAWidgets 库及其示例和测试程序。

## 构建选项

| 构建选项 | 默认值 | 说明 |
|---------|--------|------|
| `DAWIDGETS_BUILD_SHARED` | `OFF` | `ON` 生成动态库（dll/so/dylib）；`OFF` 生成静态库 |
| `DAWIDGETS_BUILD_DEMO` | `ON` | 构建示例程序 |
| `DAWIDGETS_BUILD_TESTS` | `OFF` | 构建测试程序 |

## 基本构建步骤

1. 确保已安装 Qt 5.14+ 或 Qt 6.x。
2. 安装 CMake ≥ 3.16。
3. 确认 Qt 安装路径和编译器。以 Qt 6.7.3 MSVC 2019 为例：

```shell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64"
cmake --build build --config Release
```

!!! tip "Qt5 构建"
    若需针对 Qt5 构建，传入 Qt5 前缀路径即可：
    ```shell
    cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/Qt5.14.2/5.14.2/msvc2017_64"
    ```
    CMake 会自动检测 Qt5 或 Qt6（优先 Qt6）。

## 构建示例

```shell
# 仅构建库（不含示例和测试）
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" -DDAWIDGETS_BUILD_DEMO=OFF

# 构建库 + 测试
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" -DDAWIDGETS_BUILD_TESTS=ON

# 构建动态库
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" -DDAWIDGETS_BUILD_SHARED=ON
```

## MSVC / UTF-8

源文件包含中文注释和字符串字面量。CMake 配置在 MSVC 上强制使用 `/utf-8` 标志，使编译器将源码解码为 UTF-8（Qt6 的 CMake 配置会自动添加此标志；Qt5 不会，因此已显式添加）。

!!! warning "重要"
    编辑构建配置时**务必保留此标志**，否则 MSVC 会用代码页 936 解码 UTF-8 文件，引发 C4819 警告与 C2059 语法错误。

## 静态/动态库导出宏

`DAWIDGETS_EXPORT.h` 由 CMake 的 `GenerateExportHeader` 生成到构建目录。`DA_BUILT_AS_STATIC` 宏控制展开：

| 构建模式 | 宏定义 | `DAWIDGETS_EXPORT` 展开 |
|---------|--------|------------------------|
| 静态库 | `DA_BUILT_AS_STATIC` 被 `PUBLIC` 定义 | 空（无导出） |
| 动态库 | `DAWIDGETS_EXPORTS` 被 `PRIVATE` 定义（CMake 自动） | `dllexport`（编译时）/ `dllimport`（消费时） |

!!! danger "注意"
    不要定义 `DA_BUILT_AS_STATIC=0`（值 "0" 仍算作已定义，会静默破坏导出）。

## 资源系统

DAWidgets 使用**两套**资源机制：

### PNG 字节内嵌

- 原始 PNG 存放在 `tools/` 目录（如 `tools/arrow_down.png`）。
- `tools/generate_resources.py` 扫描 `tools/*.png` 生成 `src/dawidgets_resources.{h,cpp}`（字节数组 + `dawidgets::resources::load(name)` 辅助函数）。
- 生成的文件在 `.gitignore` 中（不提交）。
- CMake 在配置时自动运行脚本（若生成文件缺失且 Python3 可用），因此全新克隆即可构建。

### Qt RCC（SVG 图标）

- `src/resources/dawidgetsicons.qrc`，前缀 `/DAWidgetsIcons`，包含 24 个 SVG 图标。
- 供 `.ui` 控件（DAAligmentEditWidget、DAFontEditPannelWidget、DAFilePathEditWidget 等）使用。

## 运行测试

测试使用 **QtTest** 框架，位于 `tests/` 目录：

```shell
# 构建测试
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" -DDAWIDGETS_BUILD_TESTS=ON
cmake --build build --config Release

# 运行测试
build\tests\logic_test\Release\dawidgets_logic_test.exe
build\tests\combobox_test\Release\dawidgets_combobox_test.exe
build\tests\collapse_test\Release\dawidgets_collapse_test.exe
```

| 测试 | 覆盖内容 |
|------|---------|
| `logic_test` | DAChipsEdit 行为（去重、maxCount、键盘提交、Backspace 删除） |
| `combobox_test` | DASelectComboBox 行为（过滤选择、allowCreate、高度模式、箭头像素渲染验证） |
| `collapse_test` | DACollapse/DACollapseItem 行为（展开折叠、手风琴互斥、动画回归） |

## 故障排除

| 问题 | 解决方案 |
|------|---------|
| CMake 找不到 Qt | 设置 `-DCMAKE_PREFIX_PATH` 为正确的 Qt 安装路径 |
| 检测到错误的 Qt 版本 | 删除 `build/CMakeCache.txt` 和 `build/CMakeFiles/`，重新配置 |
| MSVC 环境未初始化 | 使用 "Developer Command Prompt for VS" 或通过 vcvars64.bat 初始化 |
| Qt5 构建中文乱码 | 确认 CMake 配置中保留了 `/utf-8` 标志 |
