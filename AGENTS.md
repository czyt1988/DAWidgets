# QWEN.md

## Project Overview

**DAWidgets** is a Qt **QWidget** UI supplement library, inspired by [Element UI](https://element.eleme.io/). Its goal is to fill gaps in the native Qt Widgets control set by bringing Web-style component interactions to desktop applications. All public classes are prefixed with `DA`.

Currently ships a **Selector** component family (the equivalent of Element UI's `el-select` with `multiple + filterable + allow-create`). More components will be added over time.

- **Language:** C++17
- **Framework:** Qt Widgets (Qt5 / Qt6 dual-compatible)
- **Build system:** CMake (≥ 3.16)
- **Compiler (verified):** MSVC 19.29 (Visual Studio 2019)
- **Qt versions verified:** Qt 6.7.3 (msvc2019_64), Qt 5.14.2 (msvc2017_64)

## Architecture & Key Classes

Source files live directly under `src/` (flat, no subdirectory). The Selector family is composed of four classes:

| Class | Role |
|-------|------|
| `DASelectComboBox` | Public multi-tag dropdown selector. **Inherits `QComboBox`**, keeps native arrow/border/keyboard mnemonics, replaces the edit field with `DAChipsEdit`. Implements filterable popup, allow-create, default-first-option, and two height modes (`GrowWithTags` / `FixedHeightScrollable`). |
| `DAChipsEdit` | Flow-layout tag input box. Enter/separator commits a tag, Backspace deletes the last tag, supports dedup, maxCount, custom separators (enum + string overloads). Independently usable. Exposes extension points (`inputTextChanged` / `inputFocusChanged` / `inputKeyEvent`) consumed by `DASelectComboBox`. |
| `DAChip` | A single closable tag/pill widget (`QFrame` + label + close button). qss-styleable, independently usable. |
| `DAFlowLayout` | Flow layout that wraps items; the trailing item (input field) expands to fill remaining width. |

`dawidgets_resources.{h,cpp}` is **auto-generated** (see Resources below) and embeds PNG assets as byte arrays.

## Build

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<Qt install path>
cmake --build build --config Release
```

CMake auto-detects Qt5 or Qt6 (prefers Qt6).

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `DAWIDGETS_BUILD_SHARED` | `OFF` | `ON` → shared lib (dll/so/dylib); `OFF` → static lib |
| `DAWIDGETS_BUILD_DEMO`   | `ON`  | Build example applications under `examples/` |
| `DAWIDGETS_BUILD_TESTS`  | `ON`  | Build tests under `tests/` |

Example:
```bash
cmake -S . -B build -DDAWIDGETS_BUILD_SHARED=ON -DDAWIDGETS_BUILD_TESTS=OFF
```

### MSVC / UTF-8

Source files contain Chinese comments/literals. The CMake config forces `/utf-8` on MSVC so the compiler decodes sources as UTF-8 (Qt6's CMake adds this automatically; Qt5 does not, so it is added explicitly). **Always keep this flag** when editing the build config.

### Static vs Shared export macro

Export macros are hand-written in `src/DAWidgetsGlobals.h` (no CMake `GenerateExportHeader` dependency, so qmake and other build systems can integrate easily). The `DA_BUILT_AS_STATIC` macro controls expansion:
- **Static build:** `DA_BUILT_AS_STATIC` is `PUBLIC`-defined on the target → `DAWIDGETS_EXPORT` expands to nothing.
- **Shared build:** `DAWIDGETS_EXPORTS` is `PRIVATE`-defined → `DAWIDGETS_EXPORT` is `dllexport` when compiling the lib, `dllimport` for consumers.

Do not define `DA_BUILT_AS_STATIC=0` (a value of "0" still counts as defined and silently breaks exports).

## Test

Tests use **QtTest** and live under `tests/`:
- `tests/logic_test/` — `DAChipsEdit` behavior (dedup, maxCount, keyboard commit, backspace removal).
- `tests/combobox_test/` — `DASelectComboBox` behavior (filter/select, allow-create, height-mode switch, arrow geometry via pixel-render assertions).

Run after building:
```bash
build\tests\logic_test\Release\dawidgets_logic_test.exe
build\tests\combobox_test\Release\dawidgets_combobox_test.exe
```

`testArrowGeometry` renders the widget to a `QPixmap` and counts dark pixels in the arrow subcontrol rect — this is how GUI rendering regressions are caught without a human in the loop. Keep this pattern when adding GUI tests.

## Resources (tools/ workflow)

Embedded resources (e.g. the combobox arrow PNG) are **not** managed via Qt RCC, because RCC resources in a static library are not automatically propagated to executables that link it.

Instead:
- Raw PNGs live in `tools/` (e.g. `tools/arrow_down.png`).
- `tools/generate_resources.py` scans `tools/*.png` and generates `src/dawidgets_resources.{h,cpp}` (byte arrays + a `dawidgets::resources::load(name)` helper).
- The generated files are in `.gitignore` (do not commit them).
- CMake auto-runs the script at configure time if the generated files are missing (requires Python3), so a fresh clone builds out of the box.

**To change/add a resource:**
1. Replace or add a PNG in `tools/`.
2. Run `python tools/generate_resources.py`.
3. Rebuild.

## Development Conventions

- **Qt5/Qt6 dual-compatibility** is a hard requirement. Use `#if QT_VERSION >= QT_VERSION_CHECK(...)` guards for APIs that differ between versions (e.g. `QString::SkipEmptyParts` vs `Qt::SkipEmptyParts`, `QApplication::globalStrut()` removed in Qt6, `horizontalAdvance` vs `fm.width`).
- **API style:** Provide **enum overloads** (compile-time type safety, IDE discoverability) in addition to string overloads for fixed-value parameters. Example: `DAChipsEdit::Separator` enum alongside the `setSeparators(const QString&)` string overload.
- **Comments:** Chinese comments are used in headers and implementation. Keep them concise and explain *why*, not *what*. Do not translate existing comments unless asked.
- **File encoding (critical):** All source files are UTF-8. When editing files, **always use the `edit` / `write_file` tools** — never use PowerShell `Set-Content` / `Get-Content` or `sed` / `awk` to modify files, because these tools default to the system ANSI code page (GBK on Chinese Windows) and will silently corrupt UTF-8 Chinese characters. If batch processing is needed, use the `edit` tool (preserves encoding) or write a Python/Node script that explicitly opens files with `encoding='utf-8'`.
- **Includes:** Public headers use `"DAWidgetsGlobals.h"` (in `src/`, provides export macros + PIMPL macros) and are designed to be included without a `DA/` prefix (flat `src/` include path). Internal headers reference each other by bare filename (`"DAChip.h"`).
- **No RCC, no external image files at runtime** — all graphical assets are embedded via the tools/ generator.
- **Default styling:** `DASelectComboBox` applies a modern default qss (rounded border + embedded PNG arrow) in its constructor so it looks good out of the box without user-supplied stylesheets. A known Qt pitfall to avoid: styling `QComboBox::drop-down` via qss without providing a `::down-arrow` `image` suppresses the native arrow entirely.

## Build Environments

The library is **Qt5 / Qt6 dual-compatible** and verified against MSVC on Windows. Do not assume a fixed Qt install path — it varies per machine. Locate Qt before building.

To find available Qt installations, run any of:
```bash
# 1. qmake on PATH reports its Qt version and install prefix
qmake -query QT_VERSION
qmake -query QT_INSTALL_PREFIX

# 2. List common Windows install locations
dir /b C:\Qt                          # e.g. 6.7.3, Qt5.14.2
dir /b "C:\Qt\6.7.3"                  # e.g. msvc2019_64
dir /b "C:\Qt\Qt5.14.2\5.14.2"        # e.g. msvc2017_64, mingw73_64

# 3. macOS / Linux
ls /opt/Qt                            # or brew --prefix qt, ls ~/Qt
```

Then pass the discovered path to CMake:
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<discovered Qt path>
```

If multiple Qt versions are installed, the most recent one on PATH is usually the default. To explicitly target Qt5 when Qt6 is also present, pass the Qt5 prefix path via `-DCMAKE_PREFIX_PATH`.
