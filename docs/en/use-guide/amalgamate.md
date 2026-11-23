# Single-file amalgamation (src-amalgamate)

Besides the normal CMake library target, DAWidgets ships the whole library as **one header + one source file** under `src-amalgamate/`:

| File | Contents |
|------|----------|
| `src-amalgamate/DAWidgets.h` | Every public header, including all `Q_OBJECT` declarations |
| `src-amalgamate/DAWidgets.cpp` | Every implementation plus the `uic`-generated `ui_*.h` files and `rcc`-generated icon resources |

Drop the two files into your project and you have the complete library — **no DAWidgets build, no install step and no `find_package`**. The toolchain follows the reference project qwt 7 and is built on [vinniefalco/Amalgamate](https://github.com/vinniefalco/Amalgamate), driven by `tools/Amalgamate.ps1` / `tools/Amalgamate.sh`.

## When to use it

- Single-file tools, samples or plugin hosts where pulling in a dependency and its build system is overkill;
- projects that vendor their dependencies and want a self-contained, offline-friendly copy;
- anyone who prefers reading and debugging the real library sources instead of a prebuilt binary.

It is not the right choice when several projects share one library install, when the library must be upgraded as a DLL, or when first-build time is critical — use the regular CMake install flow instead (see [Build instructions](../build-guide/build-instructions.md)).

## Adding it to a CMake project

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)   # the Q_OBJECT classes in DAWidgets.h need moc

add_executable(my_app
    main.cpp
    path/to/src-amalgamate/DAWidgets.h    # listed on purpose: AUTOMOC must see it
    path/to/src-amalgamate/DAWidgets.cpp
)

target_include_directories(my_app PRIVATE path/to/src-amalgamate)

# The amalgamated files are not a DLL: define this so DAWIDGETS_EXPORT expands to nothing
target_compile_definitions(my_app PRIVATE DA_BUILT_AS_STATIC)

target_link_libraries(my_app PRIVATE
    Qt6::Widgets Qt6::Svg)   # Qt5 projects use Qt5::Widgets Qt5::Svg

if(MSVC)
    # One translation unit can exceed the COFF limit of 65536 sections (C1128)
    target_compile_options(my_app PRIVATE /bigobj /utf-8)
endif()
```

Key points:

1. **`DAWidgets.h` must be listed in the target sources.** Otherwise AUTOMOC never processes the `Q_OBJECT` classes and linking fails with `undefined reference to vtable`.
2. **`DA_BUILT_AS_STATIC` must be defined** so the export macro expands to nothing. Without it, Windows expands it to `dllimport` and the link fails.
3. Link only **Qt Widgets** and **Qt Svg** — the property-editing widgets render SVG icons through Qt Svg.
4. Include it as `#include "DAWidgets.h"`; all classes live in `namespace DA`, e.g. `DA::DASelectComboBox`.

## Adding it to a qmake project (.pro)

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

qmake runs moc automatically for files listed in `HEADERS`, so no hand-written moc rule is needed. With Qt 5, make sure `QT += widgets svg` resolves to the Qt version you actually target.

## Resources

The `.ui` files and the SVG icons registered in `src/resources/dawidgetsicons.qrc` are baked into `DAWidgets.cpp` (through `uic` / `rcc`) when the amalgamation is generated. Therefore:

- you do **not** need to add a `.qrc` file or any image files to your project, and there is no runtime dependency on external assets;
- your application code does **not** need to call `Q_INIT_RESOURCE`;
- you should still regenerate after switching Qt versions (see [Limitations](#limitations)).

## MSVC note

The generated `DAWidgets.cpp` is a single translation unit of a few hundred KB to several MB. MSVC's default COFF section limit is easily exceeded and the compiler reports:

```text
fatal error C1128: number of sections exceeded object file format limit
```

Add `/bigobj` to the target that compiles the amalgamated files (both examples above do). Keep `/utf-8` too: the sources contain Chinese comments and literals, which MSVC must decode as UTF-8.

## Regenerating

Windows (PowerShell):

```powershell
pwsh -File tools/Amalgamate.ps1

# If no CMake build directory has been configured, point at the Qt bin directory
# that contains uic.exe / rcc.exe:
pwsh -File tools/Amalgamate.ps1 -QtBin "D:\Qt\6.7.3\msvc2019_64\bin"
```

Git Bash / MSYS2:

```bash
./tools/Amalgamate.sh
QT_BIN=/d/Qt/6.7.3/msvc2019_64/bin ./tools/Amalgamate.sh
```

The script:

1. runs Qt's `uic` over `src/*.ui` to produce `ui_*.h`;
2. runs Qt's `rcc` over `src/resources/dawidgetsicons.qrc` to produce `qrc_dawidgetsicons.cpp`;
3. runs `tools/Amalgamate.exe` with the templates under `tools/amalgamate/` to emit the two files;
4. strips local absolute paths and normalizes line endings / encoding.

Qt is auto-detected from a configured `build*/CMakeCache.txt`; `-QtBin` / `QT_BIN` is only needed when no build directory exists. See [tools/Amalgamate.md](https://github.com/czyt1988/DAWidgets/blob/master/tools/Amalgamate.md) for the full tool description.

!!! danger "Never edit the generated files by hand"
    Both files are generated. When adding or removing a source file, first update
    `tools/amalgamate/DAWidgetsAmalgamTemplatePublicHeaders.h` (public headers) and
    `tools/amalgamate/DAWidgetsAmalgamTemplate.cpp` (implementations), then rerun the script.
    Manual edits are overwritten on the next generation.

## Reference example

[`examples/amalgamate_demo`](https://github.com/czyt1988/DAWidgets/tree/master/examples/amalgamate_demo) is the minimal single-file project: it does not link `DA::DAWidgets` at all, only compiles the two amalgamated files, and demonstrates the Selector and Collapse families.

```bash
cmake -S . -B build-amalgamate -DCMAKE_PREFIX_PATH=<Qt install path> \
      -DDAWIDGETS_BUILD_AMALGAMATE_DEMO=ON -DDAWIDGETS_BUILD_DEMO=OFF
cmake --build build-amalgamate --config Release --target dawidgets_amalgamate_demo

# Headless smoke run (used by CI)
./build-amalgamate/examples/amalgamate_demo/Release/dawidgets_amalgamate_demo --smoke
```

`DAWIDGETS_BUILD_AMALGAMATE_DEMO` is `OFF` by default because the amalgamated file is one large translation unit — there is no reason to build it on every development iteration.

## Limitations

- **Tied to the Qt version used at generation time.** The `uic` and `rcc` output is inlined, so switching Qt major versions or Qt installations means regenerating. The copies committed in this repository are generated with Qt 6 and verified to compile with Qt 6.7.3 and Qt 5.14.2.
- **Slower first build.** The whole library compiles as one translation unit; MSVC additionally requires `/bigobj`.
- **Not a shared library.** The amalgamated files are compiled into your target and cannot be upgraded as a DLL.
- **Generated, not hand-maintained.** Change `src/` and the amalgamation templates, then regenerate.
