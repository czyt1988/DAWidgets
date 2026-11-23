# QWEN.md

## Project Overview

**DAWidgets** is a Qt **QWidget** UI supplement library, inspired by [Element UI](https://element.eleme.io/). Its goal is to fill gaps in the native Qt Widgets control set by bringing Web-style component interactions to desktop applications. All public classes are prefixed with `DA`.

Currently ships four component families:
- **Selector** — the equivalent of Element UI's `el-select` with `multiple + filterable + allow-create`.
- **Collapse** — the equivalent of Element UI's `el-collapse` with animated expand/collapse and accordion mode.
- **Property Editing** — a suite of property-panel editors (color, pen, brush, font, alignment, file path, etc.).
- **Item Selection** — reusable "pick some entries from a candidate set" widgets (selected-list editor + search/multi-check dialog) plus small preview widgets (e.g. 2/3-color gradient bar).

All public classes are declared in **`namespace DA`** and prefixed with `DA` (e.g. `DA::DASelectComboBox`).

- **License:** MIT (see `LICENSE`)
- **Language:** C++17
- **Framework:** Qt Widgets + Qt Svg (Qt5 / Qt6 dual-compatible). Qt Svg is required for rendering SVG icons used by the property-editing widgets.
- **Build system:** CMake (≥ 3.16)
- **Compiler (verified):** MSVC 19.29 (Visual Studio 2019)
- **Qt versions verified:** Qt 6.7.3 (msvc2019_64), Qt 5.14.2 (msvc2017_64)

> **🔴 Mandatory for coding tasks: work in a dedicated git worktree under `.worktree/` — never
> develop or commit on `master` in the main worktree.** Create one with
> `pwsh -File scripts\worktree-add.ps1 feat/<scope>-<topic>` and follow
> [§ Branch & Parallel Development (Git Worktrees)](#branch--parallel-development-git-worktrees).

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

> **Every change starts in a task worktree** — see [§ Branch & Parallel Development](#branch--parallel-development-git-worktrees).
> The four steps below describe how a finished change reaches data-workbench. When you develop
> DAWidgets itself in this repository, step 0 + a commit on the task branch + merge to `master`
> is the definition of done; data-workbench then picks it up as a submodule-pointer bump.

DAWidgets is consumed by the main project as a **prebuilt** shared library. A change is only finished when all four steps are done, in this order:

0. **Run the module tests and smoke the demo** in the task worktree (this step is the responsibility of the person changing DAWidgets):
   ```powershell
   cmake --build build --config Release --parallel
   ctest --test-dir build -C Release --output-on-failure
   ./build/examples/all_widgets_demo/Release/dawidgets_all_widgets_demo.exe
   ```
   `scripts\worktree-add.ps1` preconfigures `build/` with the main worktree's Qt path and
   `DAWIDGETS_BUILD_TESTS=ON`; if you configure by hand, pass `-DDAWIDGETS_BUILD_TESTS=ON
   -DDAWIDGETS_BUILD_DEMO=ON` (the 3rdparty build and CI force both `OFF`).
1. Commit in the DAWidgets repository — the task worktree's branch here, or the submodule commit when working from data-workbench.
2. Rebuild the 3rdparty tree in data-workbench's **main worktree** and `cmake --install` (never install from a task worktree — `bin_*` is shared).
3. Sync the submodule commit into data-workbench's task worktree and commit the submodule pointer there.

Rules:

- **The 3rdparty build and CI do not run these tests and do not build the demo** (`DAWIDGETS_BUILD_TESTS` / `DAWIDGETS_BUILD_DEMO` are forced `OFF` in `src/3rdparty/CMakeLists.txt`). The module tests and demo are the modifier's own quality gate.
- Every new/migrated class ships with tests in `tests/<name>_test/` (registered in the `DAWIDGETS_BUILD_TESTS` block) **in the same commit** — no "add tests later".
- Every new/extended widget must have a visible entry in `examples/all_widgets_demo` (a section inside an existing tab or a new `Tab*.cpp` registered in `main.cpp`). Tests cover logic/API; the demo covers visuals/interaction. Both are required.
- Each test covers at least: construction, the key API, signals, and one boundary. Pure visual widgets may use `grab()` as a smoke test.
- New UI strings use `tr("English") //cn:中文`; add the translation string to the main project's `.ts` by hand (never run lupdate over the whole tree).

## Branch & Parallel Development (Git Worktrees)

> **🔴 Hard rule: every coding task runs in a worktree under `.worktree/`; never develop or commit on `master` in the main worktree.**

Development often runs several agent sessions against this repository at once. Sharing one
working tree means branch switches, staged files and uncommitted edits collide. Each task gets
its own git worktree; the main worktree stays on `master` and is only used to integrate
finished branches.

### Branch model

- `master` is the integration branch. This repository has no separate `dev`: data-workbench's
  `dev` already plays that role, and this repo is consumed there as a submodule. `-Base`
  therefore defaults to `master`.
- Use `feat/<domain>-<topic>`, `fix/<topic>`, `docs/<topic>` or `chore/<topic>`; delete the
  branch after merging it back to `master`.
- The main worktree may contain the user's own in-flight edits — never commit, revert or stash
  them on the user's behalf.

### Create a worktree

```powershell
# From any worktree of this repo; directory = <main worktree>\.worktree\<branch, / -> ->
pwsh -File scripts\worktree-add.ps1 feat/table-column-picker
pwsh -File scripts\worktree-add.ps1 fix/collapse-jitter master   # explicit base branch
pwsh -File scripts\worktree-add.ps1 -List                        # list worktrees
pwsh -File scripts\worktree-add.ps1 -Remove feat-table-column-picker
```

Use the script instead of a hand-written `git worktree add`. It:

- resolves the **main** worktree from wherever it runs, so new worktrees always land in
  `<main worktree>/.worktree/` and never nest inside another worktree;
- preconfigures the new worktree's `build/` from the main worktree's `build*/CMakeCache.txt`
  (generator, VS instance, `CMAKE_PREFIX_PATH`, `DAWIDGETS_BUILD_*`), preferring a cache with
  `DAWIDGETS_BUILD_TESTS=ON` so `ctest` works out of the box. Pass `-FromBase build-qt5` (any
  configured build directory) to inherit another Qt/option set, or `-NoConfigure` to skip;
- refuses to run inside a submodule checkout — a `data-workbench/src/3rdparty/DAWidgets`
  worktree would pollute that project's source tree and its shared `bin_*` install flow;
- removes safely on Windows: if `git worktree remove --force` leaves a locked shell directory,
  it confirms git de-registered the tree before retrying, and it refuses to delete the main
  worktree or directories it does not recognise as registered worktrees.

`.worktree/` is git-ignored, so worktree directories and their build output never show up in
`git status`.

### Work inside the worktree

Build and test exactly as in the main worktree (the script prints the inherited build directory):

```powershell
cmake --build <worktree>\build --config Release --parallel
ctest --test-dir <worktree>\build -C Release --output-on-failure
<worktree>\build\examples\all_widgets_demo\Release\dawidgets_all_widgets_demo.exe
```

- The first build in a fresh worktree is a full build; later builds are incremental.
- The preconfigured `build/` inherits **one** Qt/option set. To verify Qt5 and Qt6 in the same
  task, add a second build directory by hand (`cmake -S . -B build-qt5 -DCMAKE_PREFIX_PATH=<Qt5>`).
- Never run `cmake --install` from a worktree: the install prefix is shared/global and is not
  owned by the task. Install from the main worktree when needed.
- New or extended widgets need tests and a demo entry in the same commit (see the rules above).

### Finish: merge back and clean up

1. Merge the latest `master` into the task branch early and again before finishing, so conflicts
   surface while context is fresh.
2. Rebuild and rerun the module tests in the task worktree — they are the quality gate.
3. Merge into `master` from the main worktree (`git -C <main worktree> merge <branch>`). If
   `master` is checked out somewhere dirty, use a temporary worktree or coordinate with the user;
   never switch or clean someone else's tree.
4. Delete the branch (`git branch -d <branch>`) and the worktree
   (`pwsh -File scripts\worktree-add.ps1 -Remove <dir-name>`).

**"Task done" includes "merged back to `master` and the worktree removed."** Do not leave
conflicts, diverged branches or orphan worktrees for the next task.

### Relationship to data-workbench

When DAWidgets is changed from **inside data-workbench** (`src/3rdparty/DAWidgets`), that
project's rules win: edit the submodule in its **main worktree**, commit the submodule, rebuild
3rdparty and `cmake --install` there (a task worktree's `bin_*` is a shared junction — never
install from it). Do not create a nested worktree inside the submodule checkout; this script
refuses to, and the data-workbench `AGENTS.md` "Change Workflow" covers the four-step sync.

### Commit conventions

- Conventional Commits with Chinese descriptions: `type(scope): 中文描述`, where `type` is one
  of `feat` / `fix` / `docs` / `style` / `refactor` / `perf` / `test` / `build` / `ci` / `chore`
  / `revert`; `(scope)` is optional.
- Commit only files that belong to the task — never `git add -A` (the main worktree can hold
  other sessions' edits).
- Prefer small atomic commits that build and pass tests on their own.
- Push only when the user asks; the default remote is `origin` (`master`).

## Build

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<Qt install path>
cmake --build build --config Release
```

CMake auto-detects Qt5 or Qt6 (prefers Qt6).

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `DAWIDGETS_BUILD_SHARED` | `ON` | `ON` → shared lib (dll/so/dylib); `OFF` → static lib |
| `DAWIDGETS_BUILD_DEMO`   | `ON`  | Build example applications under `examples/` |
| `DAWIDGETS_BUILD_TESTS`  | `OFF` | Build tests under `tests/` |
| `DAWIDGETS_BUILD_AMALGAMATE_DEMO` | `OFF` | Build `examples/amalgamate_demo` — compiles `src-amalgamate/DAWidgets.{h,cpp}` as a single translation unit without linking the library |

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

Tests use **QtTest** and live under `tests/` (12 suites, all registered in the `DAWIDGETS_BUILD_TESTS` block):
- `tests/logic_test/` — `DAChipsEdit` behavior (dedup, maxCount, keyboard commit, backspace removal).
- `tests/combobox_test/` — `DASelectComboBox` behavior (filter/select, allow-create, height-mode switch, arrow geometry via pixel-render assertions).
- `tests/collapse_test/` — `DACollapseWidget` behavior (accordion mode, expand/collapse, active names management).
- `tests/file_path_edit_test/`, `tests/check_list_test/`, `tests/string_list_test/`, `tests/text_edit_test/`,
  `tests/filter_proxy_test/`, `tests/undo_history_test/`, `tests/overlay_test/`,
  `tests/item_list_edit_test/`, `tests/text_alignment_test/` — the matching widgets' API/signals/boundaries.

Run after building (all suites at once):
```bash
ctest --test-dir build -C Release --output-on-failure
```

`testArrowGeometry` renders the widget to a `QPixmap` and counts dark pixels in the arrow subcontrol rect — this is how GUI rendering regressions are caught without a human in the loop. Keep this pattern when adding GUI tests.

## Resources

All graphical assets are managed via Qt RCC (`src/resources/dawidgetsicons.qrc`), prefix `/DAWidgetsIcons`. Resources are registered once through `DA::ensureResourcesInitialized()` (`src/DAWidgetsResources.cpp`), called from the constructors of the widgets that use icons; the `Q_INIT_RESOURCE(dawidgetsicons)` expansion lives in a **global-scope** helper there, because GCC/Clang would otherwise declare a `DA::qInitResources_dawidgetsicons()` symbol that static linking cannot resolve. Consumers do not need to call anything. CMake links **Qt::Svg** as a PUBLIC dependency to render SVGs at runtime.

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
| `examples/all_widgets_demo/` | Comprehensive multi-tab demo exercising every public widget (Selector, Collapse, Color, DrawingStyle, Alignment, Utility, ItemSelection, Text, Filter). Each tab is built from a `DemoScene` list in `Tab*.cpp`, and `DACollapseWidget` is used as a per-section organizer. Pass `--export-gallery <dir>` to export all tab/widget/popup screenshots used by the docs, or `--shot <png> [tabIndex]` to save one tab. |
| `examples/amalgamate_demo/` | Single-file build reference: compiles `src-amalgamate/DAWidgets.{h,cpp}` straight into an executable without linking the library. Enabled with `-DDAWIDGETS_BUILD_AMALGAMATE_DEMO=ON`; pass `--smoke` for a no-window CI smoke run. |

## Single-file amalgamation (`src-amalgamate/`)

Besides the normal CMake library, the whole library is merged into **one header + one source
file** so consumers can drop it into a project without building/installing DAWidgets:

| File | Meaning |
|------|---------|
| `src-amalgamate/DAWidgets.h` | every public header (`Q_OBJECT` declarations included) |
| `src-amalgamate/DAWidgets.cpp` | every implementation + `uic` output + `rcc` icon resources |

- The toolchain is copied from the reference project **qwt 7**: `tools/Amalgamate.exe`
  (vinniefalco/Amalgamate) + templates under `tools/amalgamate/`, driven by
  `tools/Amalgamate.ps1` (Windows) and `tools/Amalgamate.sh` (Git Bash/MSYS2).
  See [`tools/Amalgamate.md`](./tools/Amalgamate.md) for the full description.
- **Both output files are generated — never edit them by hand.** Regenerate with
  `pwsh -File tools/Amalgamate.ps1` (or `./tools/Amalgamate.sh`). The script runs `uic`/`rcc`
  first, then Amalgamate, then normalizes paths/line endings.
- When adding/removing a source file, update
  `tools/amalgamate/DAWidgetsAmalgamTemplatePublicHeaders.h` (headers) and
  `tools/amalgamate/DAWidgetsAmalgamTemplate.cpp` (implementations) **in the same commit**.
- Regeneration is part of the release checklist (see [§ Release process](#release-process-release)).
- Verification: build `examples/amalgamate_demo` with `-DDAWIDGETS_BUILD_AMALGAMATE_DEMO=ON`
  and run it with `--smoke`; the committed amalgamation is generated with Qt 6 and verified to
  compile against Qt 6.7.3 and Qt 5.14.2.

## CI / GitHub Actions

| Workflow | Trigger | Purpose |
|----------|---------|---------|
| `.github/workflows/build.yml` | push/PR to `master`, manual | Builds the library and runs all QtTest suites on Windows (MSVC, Qt 6.8) and Linux (GCC, Qt 6.8) with `QT_QPA_PLATFORM=offscreen`; on Windows it also compiles `src-amalgamate/DAWidgets.{h,cpp}` via `examples/amalgamate_demo` and runs it with `--smoke`. |
| `.github/workflows/page.yml` | push to `master` touching `docs/**` or `mkdocs.yml`, manual | Builds the MkDocs site and deploys it to GitHub Pages. |
| `.github/workflows/sync-gitee.yml` | GitHub Release published, manual | Pushes `master` + all tags to the Gitee mirror, creates the matching Gitee Release with the same bilingual body, and attaches `src-amalgamate/DAWidgets.{h,cpp}`. |

Gitee sync requires two repository secrets: `GITEE_USERNAME` and `GITEE_TOKEN` (a Gitee
personal access token with repository write access). Without them the workflow only emits a
warning and skips, so a release still succeeds on GitHub; configure the secrets and re-run
**Sync Release to Gitee** (workflow_dispatch, input = tag) to backfill.

## Documentation (mkdocs)

A full mkdocs documentation site lives under `docs/` with Chinese and English content. Configuration is in `mkdocs.yml`. To serve locally:

```bash
pip install -r requirements-docs.txt
mkdocs serve
```

- Chinese site: <https://czyt1988.github.io/DAWidgets/> · English site: <https://czyt1988.github.io/DAWidgets/en/>
- Deployed automatically by `.github/workflows/page.yml`; build it locally with `mkdocs build --clean` before pushing docs changes.
- Use guides live under `docs/use-guide/` + `docs/en/use-guide/`; the single-file build is documented in
  [`docs/use-guide/amalgamate.md`](./docs/use-guide/amalgamate.md) and
  [`docs/en/use-guide/amalgamate.md`](./docs/en/use-guide/amalgamate.md).

## Release process (Release)

Version numbers follow **SemVer** (`MAJOR.MINOR.PATCH`): a notable feature addition bumps MINOR, a
bug-fix-only release bumps PATCH. A release is complete only when every step below is done.

1. **Bump the version** — `CMakeLists.txt` → `project(DAWidgets VERSION X.Y.Z ...)`. This version
   flows into `find_package(DAWidgets X.Y)` via the generated `DAWidgetsConfigVersion.cmake`.
2. **Finalize the changelog** — in both `CHANGELOG.md` and `CHANGELOG-CN.md`, replace the
   `## Unreleased` heading with `## tag:vX.Y.Z (YYYY-MM-DD)` and make sure every commit since the
   previous tag is covered (`git log --oneline --no-merges <previous-tag>..HEAD`), grouped under
   `### New Features` / `### Bug Fixes` / `### Build` / `### Documentation`
   (Chinese: `### 新功能` / `### Bug 修复` / `### 构建` / `### 文档`). The two files must keep
   one-to-one identical sections and bullets. Move the now-empty `## Unreleased` back to the top.
3. **Regenerate the amalgamation** — `pwsh -File tools/Amalgamate.ps1`, then compile-verify
   `examples/amalgamate_demo` against both Qt 6 and Qt 5 (the committed files are generated with
   Qt 6; they must still compile under Qt 5) and run it with `--smoke`.
4. **Verify** — in the task worktree: `cmake --build build --config Release --parallel`,
   `ctest --test-dir build -C Release --output-on-failure`, and smoke the demo. Merge the branch
   into `master` with a clean worktree.
5. **Tag and push** — on the release commit in `master`:
   ```bash
   git tag -a vX.Y.Z -m "DAWidgets vX.Y.Z"
   git push github master --follow-tags    # GitHub (HTTPS)
   git push origin master --follow-tags    # Gitee (SSH)
   ```
   Both remotes must end up with the same `master` and tag: `github` is the primary repository and
   `origin` is the Gitee mirror.
6. **Publish the GitHub Release** — title `DAWidgets vX.Y.Z`, `target_commitish=master`, body in
   the bilingual format below. Gitee is updated automatically by
   `.github/workflows/sync-gitee.yml` (branch, tags, Release body and the amalgamated files). If
   the Gitee secrets are not configured yet, add them and re-run the workflow with the tag.

### Release notes format

Release notes are **mixed Chinese/English, English first**: the English part is the complete
content of this version from `CHANGELOG.md`, the Chinese part is the complete content from
`CHANGELOG-CN.md`, placed at the very bottom in a single collapsed `<details>` block whose summary
is `中文说明（点击展开）`. There must be a blank line between the `<summary>` line and the Chinese
Markdown, otherwise GitHub will not render it:

```markdown
## New Features
...（English content, from CHANGELOG.md）...

## Documentation
...

<details>
<summary>中文说明（点击展开）</summary>

## 新功能
...（Chinese content, from CHANGELOG-CN.md）...

## 文档
...

</details>
```

Section headings inside release notes use level 2 (`##`) because the release page has no
`## tag:vX.Y.Z` heading level; the changelog files use level 3 (`###`) for the same sections.

### Repository metadata

Keep the GitHub repository **About** block in sync with the project: a one-sentence English
description ("Qt Widgets supplement library inspired by Element UI — multi-select, collapse,
property editors and more. Qt 5/6, CMake, MIT."), the docs homepage
<https://czyt1988.github.io/DAWidgets/>, and the topics `qt`, `qt5`, `qt6`, `qt-widgets`,
`widgets`, `element-ui`, `cpp`, `cmake`, `gui`, `component-library`.

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
