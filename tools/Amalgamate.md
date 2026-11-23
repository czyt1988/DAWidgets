# amalgamate（把整个工程合并为一个 .h + 一个 .cpp）

`src-amalgamate/` 下的两个文件把整个 DAWidgets 合并为**单一头文件 + 单一实现文件**，
把这两个文件加入工程即可直接使用，不需要构建/安装库，也不需要配置 `find_package`。
这种形式适合：示例程序、插件宿主、单文件工具，以及不想为一个小依赖引入构建系统的项目。

本工具照搬自参考项目 **qwt 7** 的 `tools/Amalgamate.*`（脚本、模板组织方式一致）。
合并器本身是 [vinniefalco/Amalgamate](https://github.com/vinniefalco/Amalgamate)。

## 产物

| 文件 | 说明 |
|------|------|
| `src-amalgamate/DAWidgets.h` | 全部公开头文件（含全部 `Q_OBJECT` 声明）合并而成 |
| `src-amalgamate/DAWidgets.cpp` | 全部实现 + `uic` 生成的 `ui_*.h` + `rcc` 生成的图标资源，合并而成 |

两个文件**自动生成，不要手工编辑**。

## 生成命令

Windows（推荐，PowerShell）：

```powershell
pwsh -File tools\Amalgamate.ps1
# 或者显式指定 Qt：
pwsh -File tools\Amalgamate.ps1 -QtBin "D:\Qt\6.7.3\msvc2019_64\bin"
```

Git Bash / MSYS2：

```bash
./tools/Amalgamate.sh
QT_BIN=/d/Qt/6.7.3/msvc2019_64/bin ./tools/Amalgamate.sh
```

脚本会依次：

1. 用 Qt 的 `uic` 把 `src/*.ui` 生成 `ui_*.h`（输出到 `tools/amalgamate/generated/`，已被 .gitignore 忽略）；
2. 用 Qt 的 `rcc` 把 `src/resources/dawidgetsicons.qrc` 生成 `qrc_dawidgetsicons.cpp`；
3. 用 `tools/Amalgamate.exe` 按模板合并出上面两个文件。

没有配置过 CMake 时必须用 `-QtBin` / `QT_BIN` 指定 Qt（脚本默认从 `build*/CMakeCache.txt` 推断）。

## 模板

| 文件 | 作用 |
|------|------|
| `tools/amalgamate/DAWidgetsAmalgamTemplate.h` | 头文件模板，只包含公共头文件清单 |
| `tools/amalgamate/DAWidgetsAmalgamTemplatePublicHeaders.h` | 列举所有公开头文件（新增类要改这里） |
| `tools/amalgamate/DAWidgetsAmalgamTemplateHeaderGlue.h` | 给 cpp 模板的 `@remap` 用的中间层 |
| `tools/amalgamate/DAWidgetsAmalgamTemplate.cpp` | 实现模板，`#include` 所有 .cpp；`@remap` 让公共头文件在 cpp 里只落成 `#include "DAWidgets.h"` |

`.cpp` 模板末尾还内联了 `qrc_dawidgetsicons.cpp`，并给 Qt5 补了
`QT_RCC_MANGLE_NAMESPACE` / `QT_RCC_PREPEND_NAMESPACE` 的 fallback 定义，因此
**用 Qt 6 生成的合成文件也能在 Qt 5 下编译**。

## 增删源码时

1. 新增公开头文件：加入 `DAWidgetsAmalgamTemplatePublicHeaders.h`；
2. 新增 `.cpp`：加入 `DAWidgetsAmalgamTemplate.cpp`；
3. 重新运行 `tools/Amalgamate.ps1`；
4. 用 `-DDAWIDGETS_BUILD_AMALGAMATE_DEMO=ON` 编译验证：

   ```bash
   cmake -S . -B build-amalgamate -DCMAKE_PREFIX_PATH=<Qt> \
         -DDAWIDGETS_BUILD_AMALGAMATE_DEMO=ON -DDAWIDGETS_BUILD_DEMO=OFF
   cmake --build build-amalgamate --config Release --target dawidgets_amalgamate_demo
   ```

参考目标：`examples/amalgamate_demo/`。

## 使用限制

- 合成文件内含 `.ui` 与 `.qrc` 的生成结果，因此**与生成时使用的 Qt 大版本/路径相关**；
  切换 Qt 版本（Qt5 ↔ Qt6）或更换 Qt 安装后应重新生成。
- 单一翻译单元会显著增加首次编译时间；MSVC 需要 `/bigobj`（示例工程已加）。
- `Q_INIT_RESOURCE(dawidgetsicons)` 仍由库代码内部调用，资源已内联，使用者无需额外操作。
- 合成文件不导出 DLL：使用时需要 `DA_BUILT_AS_STATIC`（示例工程已定义）。
