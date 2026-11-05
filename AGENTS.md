# QWEN.md

## Project Overview

**DAWidgets** is a Qt **QWidget** UI supplement library, inspired by [Element UI](https://element.eleme.io/). Its goal is to fill gaps in the native Qt Widgets control set by bringing Web-style component interactions to desktop applications. All public classes are prefixed with `DA`.

Currently ships four component families:
- **Selector** — the equivalent of Element UI's `el-select` with `multiple + filterable + allow-create`.
- **Collapse** — the equivalent of Element UI's `el-collapse` with animated expand/collapse and accordion mode.
- **Property Editing** — a suite of property-panel editors (color, pen, brush, font, alignment, file path, etc.).
- **Item Selection** — reusable "pick some entries from a candidate set" widgets (selected-list editor + search/multi-check dialog) plus small preview widgets (e.g. 2/3-color gradient bar).

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
| `DATextAlignmentEditWidget` | Text alignment editor with two independent exclusive rows: horizontal (left/center/right) + vertical (top/middle/bottom); value is a combined `Qt::Alignment`. |
| `DAAligmentPositionEditWidget` | 9-grid alignment position editor. |
| `DATimeFormatEditWidget` | Time format string editor with preset format options. |
| `DAFilePathEditWidget` | File/directory path editor: line edit + browse button. Modes `OpenFile` / `SaveFile` / `ExistingDirectory`, plus start directory, name filters and default suffix. |
| `DATextEditWidget` | Plain-text viewer/editor: read-only, monospace, line wrap, placeholder and caller-injected toolbar actions. Does not own OK/Cancel. |
| `DAPropertyItemWidget` | Property row wrapper: label + editor in `InlineLayout` or `BelowLayout` mode. |
| `DACursorScoped` / `DAWaitCursorScoped` | RAII cursor guards (exception-safe). |

### Item Selection & Preview Family

| Class | Role |
|-------|------|
| `DAItemEntry` | Generic candidate/selected entry: display `text` + identifying `data` (falls back to text equality when `data` is invalid). |
| `DAItemListEditWidget` | "Selected items" list editor: rows with an inline remove button; a `MenuButtonPopup` add tool button (arrow = candidate dropdown menu, body = pick dialog); remove-all with confirmation; optional Up/Down reorder buttons (`setReorderEnabled`). Emits `selectedItemsChanged()` on every mutation. Typical reuse: table column selection. |
| `DAItemListPickDialog` | Item check dialog: search filter (substring, state preserved across filtering) + checkable list; `checkedEntries()` returns the batch to add. A thin shell over `DACheckListWidget`. |
| `DACheckListWidget` | Checkable list with search + optional Select All / Invert. Forced-checked entries can never be unchecked (select-all/invert skip them); filtering only hides rows, never changes check state. |
| `DAStringListEditWidget` | Plain `QStringList` editor: inline editable rows (`AddMode::Inline`) or caller-supplied add (`AddMode::External` + `addRequested()`), optional remove/up/down, max count, placeholder. |
| `DAColorScalePreviewWidget` | 2/3-color horizontal linear gradient preview bar (paints stops at 0 / 0.5 / 1); blank state when colors are invalid. |
| `DAStringFilterProxyModel` | Display-text filter proxy for list/tree models: case sensitivity, single/all columns, and optional "include parents of matches" for trees. |

### Utility Widgets

| Class | Role |
|-------|------|
| `DAUndoHistoryWidget` | Undo history panel: `QUndoView` following a `QUndoGroup` plus an undo-depth spin box that emits `undoLimitChangeRequested` (persistence stays in the caller). |
| `DAAbstractWidgetOverlay` | Abstract overlay base for painting on top of another widget (mask modes `NoMask`/`MaskHint`/`AlphaMask`, render modes); subclasses implement `drawOverlay()`. |

### Infrastructure

| File | Role |
|------|------|
| `DAWidgetsGlobals.h` | Hand-written export macros (`DAWIDGETS_EXPORT`) and PIMPL macros (`DA_DECLARE_PRIVATE`, `DA_DECLARE_PUBLIC`, `DA_D`, `DA_Q`). |
| `DAQt5Qt6Compat.h` | Qt5/Qt6 compatibility macros (`pos()`/`position()`, `width`/`horizontalAdvance`, `buttonClicked`/`idClicked`). |

## Change Workflow (read before touching DAWidgets)

DAWidgets is consumed by the main project as a **prebuilt** shared library. A change is only finished when all four steps are done, in this order:

0. **Run the module tests and smoke the demo** (this step is the responsibility of the person changing DAWidgets):
   ```powershell
   cmake -S src/3rdparty/DAWidgets -B build-dawidgets -G "Visual Studio 16 2019" -A x64 `
         -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64" `
         -DDAWIDGETS_BUILD_TESTS=ON -DDAWIDGETS_BUILD_DEMO=ON
   cmake --build build-dawidgets --config Release --parallel
   ctest --test-dir build-dawidgets -C Release --output-on-failure
   ./build-dawidgets/examples/all_widgets_demo/Release/dawidgets_all_widgets_demo.exe
   ```
1. Commit in the DAWidgets submodule.
2. Rebuild the 3rdparty tree in the **main worktree** and `cmake --install` (never install from a task worktree — `bin_*` is shared).
3. Sync the submodule commit into the task worktree and commit the submodule pointer there.

Rules:

- **The 3rdparty build and CI do not run these tests and do not build the demo** (`DAWIDGETS_BUILD_TESTS` / `DAWIDGETS_BUILD_DEMO` are forced `OFF` in `src/3rdparty/CMakeLists.txt`). The module tests and demo are the modifier's own quality gate.
- Every new/migrated class ships with tests in `tests/<name>_test/` (registered in the `DAWIDGETS_BUILD_TESTS` block) **in the same commit** — no "add tests later".
- Every new/extended widget must have a visible entry in `examples/all_widgets_demo` (a section inside an existing tab or a new `Tab*.cpp` registered in `main.cpp`). Tests cover logic/API; the demo covers visuals/interaction. Both are required.
- Each test covers at least: construction, the key API, signals, and one boundary. Pure visual widgets may use `grab()` as a smoke test.
- New UI strings use `tr("English") //cn:中文`; add the translation string to the main project's `.ts` by hand (never run lupdate over the whole tree).

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
| `examples/all_widgets_demo/` | Comprehensive multi-tab demo exercising every public widget (Selector, Color, DrawingStyle, Alignment, Utility, ItemSelection, Text, Filter). `DACollapseWidget` is used as a per-section collapsible organizer within each tab. Pass `--shot <png> [tabIndex]` to save a screenshot of one tab. |

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
