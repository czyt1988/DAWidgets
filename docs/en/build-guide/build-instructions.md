# DAWidgets Build Instructions

This document describes how to build the DAWidgets library, its examples, and test programs using CMake.

## Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `DAWIDGETS_BUILD_SHARED` | `OFF` | `ON` builds a shared library (dll/so/dylib); `OFF` builds a static library |
| `DAWIDGETS_BUILD_DEMO` | `ON` | Build example applications |
| `DAWIDGETS_BUILD_TESTS` | `OFF` | Build test programs |

## Basic Build Steps

1. Ensure Qt 5.14+ or Qt 6.x is installed.
2. Install CMake >= 3.16.
3. Identify your Qt installation path and compiler. Taking Qt 6.7.3 MSVC 2019 as an example:

```shell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64"
cmake --build build --config Release
```

!!! tip "Building against Qt5"
    To target Qt5, simply pass the Qt5 prefix path:
    ```shell
    cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/Qt5.14.2/5.14.2/msvc2017_64"
    ```
    CMake auto-detects Qt5 or Qt6 (preferring Qt6).

## Build Examples

```shell
# Build the library only (no examples or tests)
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" -DDAWIDGETS_BUILD_DEMO=OFF

# Build the library + tests
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" -DDAWIDGETS_BUILD_TESTS=ON

# Build as a shared library
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" -DDAWIDGETS_BUILD_SHARED=ON
```

## MSVC / UTF-8

Source files contain Chinese comments and string literals. The CMake configuration forces the `/utf-8` flag on MSVC so that the compiler decodes sources as UTF-8 (Qt6's CMake configuration adds this flag automatically; Qt5 does not, so it is added explicitly).

!!! warning "Important"
    **Always keep this flag** when editing the build configuration. Without it, MSVC decodes UTF-8 files using code page 936, causing C4819 warnings and C2059 syntax errors.

## Static / Shared Library Export Macros

Export macros are hand-written in `src/DAWidgetsGlobals.h` (no CMake `GenerateExportHeader` dependency, so qmake and other build systems can integrate easily). The `DA_BUILT_AS_STATIC` macro controls expansion:

| Build Mode | Macro Definition | `DAWIDGETS_EXPORT` Expands To |
|------------|-----------------|-------------------------------|
| Static library | `DA_BUILT_AS_STATIC` is `PUBLIC`-defined | nothing (no export) |
| Shared library | `DAWIDGETS_EXPORTS` is `PRIVATE`-defined | `dllexport` (when compiling) / `dllimport` (when consuming) |

!!! danger "Caution"
    Do not define `DA_BUILT_AS_STATIC=0` (a value of "0" still counts as defined and silently breaks exports).

## Resource System

All graphical assets (SVG icons, arrows, etc.) are managed via Qt RCC:

- `src/resources/dawidgetsicons.qrc`, prefix `/DAWidgetsIcons`, containing SVG icons.
- Used by `.ui` widgets (DAAligmentEditWidget, DAFontEditPannelWidget, DAFilePathEditWidget, etc.) and `DASelectComboBox` default styling.
- Static-library consumers must call `Q_INIT_RESOURCE(dawidgetsicons)` in their constructor to ensure the linker keeps the resources.

## Running Tests

Tests use the **QtTest** framework and live under `tests/`:

```shell
# Build tests
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" -DDAWIDGETS_BUILD_TESTS=ON
cmake --build build --config Release

# Run tests
build\tests\logic_test\Release\dawidgets_logic_test.exe
build\tests\combobox_test\Release\dawidgets_combobox_test.exe
build\tests\collapse_test\Release\dawidgets_collapse_test.exe
```

| Test | Coverage |
|------|----------|
| `logic_test` | DAChipsEdit behavior (dedup, maxCount, keyboard commit, Backspace removal) |
| `combobox_test` | DASelectComboBox behavior (filter/select, allowCreate, height mode, arrow pixel-render verification) |
| `collapse_test` | DACollapse/DACollapseItem behavior (expand/collapse, accordion mutual exclusion, animation regression) |

## Troubleshooting

| Problem | Solution |
|---------|----------|
| CMake cannot find Qt | Set `-DCMAKE_PREFIX_PATH` to the correct Qt installation path |
| Wrong Qt version detected | Delete `build/CMakeCache.txt` and `build/CMakeFiles/`, then reconfigure |
| MSVC environment not initialized | Use the "Developer Command Prompt for VS" or initialize via vcvars64.bat |
| Garbled Chinese text in Qt5 build | Confirm the `/utf-8` flag is retained in the CMake configuration |
