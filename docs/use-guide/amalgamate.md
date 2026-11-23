# 单文件合并（src-amalgamate）

除了常规的 CMake 库目标，DAWidgets 还把整个库合并成**一个头文件 + 一个实现文件**，放在 `src-amalgamate/` 下：

| 文件 | 内容 |
|------|------|
| `src-amalgamate/DAWidgets.h` | 全部公开头文件（包含所有 `Q_OBJECT` 声明） |
| `src-amalgamate/DAWidgets.cpp` | 全部实现 + `uic` 生成的 `ui_*.h` + `rcc` 生成的图标资源 |

把这两个文件加入工程即可使用完整功能，**不需要构建或安装 DAWidgets，也不需要 `find_package`**。合并器沿用参考项目 qwt 7 的工具链（[vinniefalco/Amalgamate](https://github.com/vinniefalco/Amalgamate)），由 `tools/Amalgamate.ps1` / `tools/Amalgamate.sh` 驱动。

## 适用场景

- 单文件小工具、示例程序、插件宿主，不想为一个依赖引入构建系统；
- 需要把全部源码放进同一个仓库/离线包，避免库版本与二进制匹配问题；
- 想直接阅读、调试库源码，而不是面对预编译库。

不适合的场景：多个工程共享同一份库、需要动态库升级、对首次编译时间敏感的大型工程——这些情况请用常规 CMake 安装方式（见 [构建说明](../build-guide/build-instructions.md)）。

## 加入 CMake 工程

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)   # DAWidgets.h 里的 Q_OBJECT 需要 moc

add_executable(my_app
    main.cpp
    path/to/src-amalgamate/DAWidgets.h    # 必须列进源文件，AUTOMOC 才会为它生成 moc
    path/to/src-amalgamate/DAWidgets.cpp
)

target_include_directories(my_app PRIVATE path/to/src-amalgamate)

# 合成文件不是 DLL，必须定义该宏，DAWIDGETS_EXPORT 才会展开为空
target_compile_definitions(my_app PRIVATE DA_BUILT_AS_STATIC)

target_link_libraries(my_app PRIVATE
    Qt6::Widgets Qt6::Svg)   # Qt5 工程改成 Qt5::Widgets Qt5::Svg

if(MSVC)
    # 单一翻译单元会超出 COFF 默认 65536 个 section 的限制（C1128）
    target_compile_options(my_app PRIVATE /bigobj /utf-8)
endif()
```

要点：

1. **`DAWidgets.h` 必须出现在源文件列表里**，否则 AUTOMOC 不会处理其中的 `Q_OBJECT` 类，链接阶段会报 `undefined reference to vtable`。
2. **`DA_BUILT_AS_STATIC` 必须定义**，让导出宏展开为空；漏掉时 Windows 上会展开成 `dllimport`，导致链接失败。
3. 只需链接 **Qt Widgets** 和 **Qt Svg**（属性编辑控件用 Qt Svg 渲染 SVG 图标）。
4. 包含头文件时直接 `#include "DAWidgets.h"`，类在 `DA` 命名空间下，例如 `DA::DASelectComboBox`。

## 加入 qmake 工程（.pro）

```qmake
QT += widgets svg
CONFIG += c++17

INCLUDEPATH += path/to/src-amalgamate
HEADERS += path/to/src-amalgamate/DAWidgets.h
SOURCES += path/to/src-amalgamate/DAWidgets.cpp
DEFINES += DA_BUILT_AS_STATIC

win32-msvc* {
    QMAKE_CXXFLAGS += /bigobj /utf-8
}
```

`HEADERS` 里的文件会被 qmake 自动交给 moc 处理，因此不需要手写 `moc` 规则。Qt5 下请确认 `QT += widgets svg` 用的就是目标 Qt 版本。

## 资源说明

`.ui` 文件与 `src/resources/` 下的 SVG 图标（`dawidgetsicons.qrc`）在生成时就已分别通过 `uic` / `rcc` 内联进 `DAWidgets.cpp`，因此：

- 工程里**不需要再加 `.qrc` 文件或图片文件**，也没有运行期外部资源依赖；
- 不需要在应用代码里调用 `Q_INIT_RESOURCE`；
- 升级 Qt 版本后图标行为一致，但仍建议按下一节重新生成（见「使用限制」）。

## MSVC 编译说明

合成后的 `DAWidgets.cpp` 是一个几百 KB 到数 MB 的单一翻译单元，MSVC 默认的 COFF section 数量上限会被突破，出现：

```text
fatal error C1128: number of sections exceeded object file format limit
```

解决办法就是给使用它的目标加 `/bigobj`（上文的 CMake / qmake 示例已包含）。同时保留 `/utf-8`：源码含中文注释与字面量，MSVC 需要它以 UTF-8 解码源文件。

## 重新生成

Windows（PowerShell）：

```powershell
pwsh -File tools/Amalgamate.ps1

# 没配置过 CMake 时显式指定 Qt 的 bin 目录（含 uic.exe / rcc.exe）
pwsh -File tools/Amalgamate.ps1 -QtBin "D:\Qt\6.7.3\msvc2019_64\bin"
```

Git Bash / MSYS2：

```bash
./tools/Amalgamate.sh
QT_BIN=/d/Qt/6.7.3/msvc2019_64/bin ./tools/Amalgamate.sh
```

脚本的流程：

1. 用 Qt 的 `uic` 把 `src/*.ui` 生成 `ui_*.h`；
2. 用 Qt 的 `rcc` 把 `src/resources/dawidgetsicons.qrc` 生成 `qrc_dawidgetsicons.cpp`；
3. 用 `tools/Amalgamate.exe` 按 `tools/amalgamate/` 下的模板合并出上面两个文件；
4. 去掉生成文件里的本机绝对路径，统一换行与编码。

脚本默认从已配置的 `build*/CMakeCache.txt` 推断 Qt 路径，推断失败时才需要 `-QtBin` / `QT_BIN`。完整的工具说明见 [tools/Amalgamate.md](https://github.com/czyt1988/DAWidgets/blob/master/tools/Amalgamate.md)。

!!! danger "不要手工编辑生成文件"
    两个文件都是自动生成的。新增/删除源码时，先更新
    `tools/amalgamate/DAWidgetsAmalgamTemplatePublicHeaders.h`（公开头文件清单）与
    `tools/amalgamate/DAWidgetsAmalgamTemplate.cpp`（实现清单），再重新运行脚本；
    手工改动会在下次生成时被覆盖。

## 参考示例

仓库里的 [`examples/amalgamate_demo`](https://github.com/czyt1988/DAWidgets/tree/master/examples/amalgamate_demo) 就是最小可用的单文件工程：它不链接 `DA::DAWidgets`，只编译两个合并文件，并演示 Selector 与 Collapse 两个组件家族。

```bash
cmake -S . -B build-amalgamate -DCMAKE_PREFIX_PATH=<Qt 安装路径> \
      -DDAWIDGETS_BUILD_AMALGAMATE_DEMO=ON -DDAWIDGETS_BUILD_DEMO=OFF
cmake --build build-amalgamate --config Release --target dawidgets_amalgamate_demo

# 无界面冒烟（CI 用）
./build-amalgamate/examples/amalgamate_demo/Release/dawidgets_amalgamate_demo --smoke
```

`DAWIDGETS_BUILD_AMALGAMATE_DEMO` 默认关闭，因为合并文件是一个大翻译单元，日常开发没必要每次编译。

## 使用限制

- **与生成时的 Qt 版本相关**：`.ui`、`.qrc` 的生成结果会被内联，因此切换 Qt 大版本或更换 Qt 安装后应重新生成。仓库内提交的合并文件用 Qt 6 生成，并已验证可在 Qt 6.7.3 与 Qt 5.14.2 下编译通过。
- **首次编译更慢**：整个库是一个翻译单元，增量构建的便利换来了首编时间增加；MSVC 需要 `/bigobj`。
- **不是动态库**：合并文件被直接编译进你的目标，不能作为 DLL 单独升级。
- **不要手工编辑**：改的是 `src/` 源文件与合并模板，然后重新生成。
