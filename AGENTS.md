# QWEN.md

## Project Overview

**DAWidgets** is a Qt **QWidget** UI supplement library, inspired by [Element UI](https://element.eleme.io/). Its goal is to fill gaps in the native Qt Widgets control set by bringing Web-style component interactions to desktop applications. All public classes are prefixed with `DA`.

Currently ships three component families:
- **Selector** — the equivalent of Element UI's `el-select` with `multiple + filterable + allow-create`.
- **Collapse** — the equivalent of Element UI's `el-collapse` with animated expand/collapse and accordion mode.
- **Property Editing** — a suite of property-panel editors (color, pen, brush, font, alignment, file path, etc.).

- **Language:** C++17
- **Framework:** Qt Widgets + Qt Svg (Qt5 / Qt6 dual-compatible). Qt Svg is required for rendering SVG icons used by the property-editing widgets.
- **Build system:** CMake (≥ 3.16)
- **Compiler (verified):** MSVC 19.29 (Visual Studio 2019)
- **Qt versions verified:** Qt 6.7.3 (msvc2019_64), Qt 5.14.2 (msvc2017_64)

## Architecture & Key Classes

Source files live directly under `src/` (flat, no subdirectory). SVG icons live under `src/resources/`.

### Selector Family

| Class | Role |
|-------|------|
| `DASelectComboBox` | Public multi-tag dropdown selector. **Inherits `QComboBox`**, keeps native arrow/border/keyboard mnemonics, replaces the edit field with `DAChipsEdit`. Implements filterable popup, allow-create, default-first-option, and two height modes (`GrowWithTags` / `FixedHeightScrollable`). |
| `DAChipsEdit` | Flow-layout tag input box. Enter/separator commits a tag, Backspace deletes the last tag, supports dedup, maxCount, custom separators (enum + string overloads). Independently usable. Exposes extension points (`inputTextChanged` / `inputFocusChanged` / `inputKeyEvent`) consumed by `DASelectComboBox`. |
| `DAChip` | A single closable tag/pill widget (`QFrame` + label + close button). qss-styleable, independently usable. |
| `DAFlowLayout` | Flow layout that wraps items; the trailing item (input field) expands to fill remaining width. |

### Collapse Family

| Class | Role |
|-------|------|
| `DACollapseWidget` | Collapse panel container (equivalent of `el-collapse`). Arranges `DACollapseItem`s vertically, coordinates expand state. Supports accordion mode (only one panel open) and non-accordion (multiple open). Key API: `accordion()`/`setAccordion()`, `activeNames()`/`setActiveNames()`, `addItem()`, `removeItem()`, `clear()`. |
| `DACollapseItem` | Single collapse item (equivalent of `el-collapse-item`). Header (clickable title) + content area. Animated expand/collapse via `QPropertyAnimation`. Key API: `title()`/`setTitle()`, `name()`/`setName()`, `disabled()`/`setDisabled()`, `expanded()`/`setExpanded()`, `animationEnabled()`/`setAnimationEnabled()`, `setTitleWidget()`, `setContentWidget()`, `contentLayout()`. |
| `DACollapseLayout` | Custom vertical `QLayout` for the collapse container. Each item takes its `sizeHint()` natural height — no compression, no stretching, no `qGeomCalc()`. Eliminates jitter during expand/collapse animation. |

### Property Editing Suite

| Class | Role |
|-------|------|
| `DAColorPickerButton` | Self-painted color picker `QToolButton`. Two modes: `WithColorMenu` (auto-creates `DAColorMenu`) and `NoColorMenu` (emits `colorClicked`). |
| `DAColorMenu` | Color selection `QMenu`: theme palette → optional "None" → "Custom Color" (opens `QColorDialog`) → custom color record grid (max 10, FIFO). |
| `DAColorPaletteGridWidget` | Office-style color palette. Composes two `DAColorGridWidget`s (main colors + auto-generated lighter/darker shades). |
| `DAColorGridWidget` | Basic color grid (`QGridLayout` + `QButtonGroup`). |
| `DAPenEditWidget` | Pen editor: color button + line style combo + line width. |
| `DAPenStyleComboBox` | `Qt::PenStyle` selector as a `QComboBox` with auto-generated icons. |
| `DABrushEditWidget` | Brush editor: color + style. Supports solid/pattern/gradient (Linear/Radial/Conical)/texture. |
| `DABrushStyleComboBox` | `Qt::BrushStyle` selector as a `QComboBox` with auto-generated icons. |
| `DAFontEditPannelWidget` | Font editing panel: family + size + bold/italic/underline + color. Two layout modes: `TwoRowsLayout` (default) / `SingleRowLayout` (all controls in one row, for toolbar embedding). |
| `DAShapeEditPannelWidget` | Shape editor: composes `DABrushEditWidget` + `DAPenEditWidget`. |
| `DAAligmentEditWidget` | Alignment editor (horizontal/vertical). |
| `DAAligmentPositionEditWidget` | 9-grid alignment position editor. |
| `DATimeFormatEditWidget` | Time format string editor with preset format options. |
| `DAFilePathEditWidget` | File path editor: line edit + browse button. |
| `DAPropertyItemWidget` | Property row wrapper: label + editor in `InlineLayout` or `BelowLayout` mode. |
| `DARecentFilesManager` | Recent files list manager. Persists to QSettings, auto-filters invalid paths. |
| `DACursorScoped` / `DAWaitCursorScoped` | RAII cursor guards (exception-safe). |

### Infrastructure

| File | Role |
|------|------|
| `DAWidgetsGlobals.h` | Hand-written export macros (`DAWIDGETS_EXPORT`) and PIMPL macros (`DA_DECLARE_PRIVATE`, `DA_DECLARE_PUBLIC`, `DA_D`, `DA_Q`). |
| `DAQt5Qt6Compat.h` | Qt5/Qt6 compatibility macros (`pos()`/`position()`, `width`/`horizontalAdvance`, `buttonClicked`/`idClicked`). |

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
| `DAWIDGETS_BUILD_TESTS`  | `OFF` | Build tests under `tests/` |

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
- `tests/collapse_test/` — `DACollapseWidget` behavior (accordion mode, expand/collapse, active names management).

Run after building:
```bash
build\tests\logic_test\Release\dawidgets_logic_test.exe
build\tests\combobox_test\Release\dawidgets_combobox_test.exe
build\tests\collapse_test\Release\dawidgets_collapse_test.exe
```

`testArrowGeometry` renders the widget to a `QPixmap` and counts dark pixels in the arrow subcontrol rect — this is how GUI rendering regressions are caught without a human in the loop. Keep this pattern when adding GUI tests.

## Resources

All graphical assets are managed via Qt RCC (`src/resources/dawidgetsicons.qrc`), prefix `/DAWidgetsIcons`. Static-library consumers must call `Q_INIT_RESOURCE(dawidgetsicons)` to ensure the linker keeps the resources (see `DASelectComboBox`, `DACollapseItem`, `DAFilePathEditWidget` constructors for the pattern). CMake links **Qt::Svg** as a PUBLIC dependency to render SVGs at runtime.

**To change/add a resource:**
1. Add the SVG/PNG file to `src/resources/`.
2. Register it in `src/resources/dawidgetsicons.qrc`.
3. Rebuild.

## Development Conventions

- **Qt5/Qt6 dual-compatibility** is a hard requirement. Use `#if QT_VERSION >= QT_VERSION_CHECK(...)` guards for APIs that differ between versions (e.g. `QString::SkipEmptyParts` vs `Qt::SkipEmptyParts`, `QApplication::globalStrut()` removed in Qt6, `horizontalAdvance` vs `fm.width`).
- **API style:** Provide **enum overloads** (compile-time type safety, IDE discoverability) in addition to string overloads for fixed-value parameters. Example: `DAChipsEdit::Separator` enum alongside the `setSeparators(const QString&)` string overload.
- **PIMPL:** Most public classes use the PIMPL idiom via macros from `DAWidgetsGlobals.h` (`DA_DECLARE_PRIVATE`, `DA_DECLARE_PUBLIC`, `DA_D`, `DA_Q`). Keep private implementation details in the `.cpp` file.
- **Qt signals/slots:** Use `Q_SIGNALS` / `Q_SLOTS` / `Q_EMIT` (capitalized forms) instead of lowercase `signals` / `slots` / `emit` for consistency across the codebase.
- **Comments:** Chinese comments are used in headers and implementation. Keep them concise and explain *why*, not *what*. Do not translate existing comments unless asked.
- **File encoding (critical):** All source files are UTF-8. When editing files, **always use the `edit` / `write_file` tools** — never use PowerShell `Set-Content` / `Get-Content` or `sed` / `awk` to modify files, because these tools default to the system ANSI code page (GBK on Chinese Windows) and will silently corrupt UTF-8 Chinese characters. If batch processing is needed, use the `edit` tool (preserves encoding) or write a Python/Node script that explicitly opens files with `encoding='utf-8'`.
- **Includes:** Public headers use `"DAWidgetsGlobals.h"` (in `src/`, provides export macros + PIMPL macros) and are designed to be included without a `DA/` prefix (flat `src/` include path). Internal headers reference each other by bare filename (`"DAChip.h"`).
- **No external image files at runtime** — all graphical assets are embedded via Qt RCC (`src/resources/dawidgetsicons.qrc`).
- **Default styling:** `DASelectComboBox` applies a modern default qss (rounded border + qrc SVG arrow) in its constructor so it looks good out of the box without user-supplied stylesheets. A known Qt pitfall to avoid: styling `QComboBox::drop-down` via qss without providing a `::down-arrow` `image` suppresses the native arrow entirely.

## Examples

| Directory | Description |
|-----------|-------------|
| `examples/all_widgets_demo/` | Comprehensive multi-tab demo exercising every public widget (Selector, Color, DrawingStyle, Alignment, Utility). `DACollapseWidget` is used as a per-section collapsible organizer within each tab. |

## Documentation (mkdocs)

A full mkdocs documentation site lives under `docs/` with Chinese and English content. Configuration is in `mkdocs.yml`. To serve locally:

```bash
pip install -r requirements-docs.txt
mkdocs serve
```

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
