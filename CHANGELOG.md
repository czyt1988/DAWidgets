# Changelog

Notable changes in DAWidgets are recorded in this file; version numbers follow [Semantic Versioning](https://semver.org/).

## Unreleased

*No unreleased changes yet.*

## tag:v0.1.0 (2026-11-25)

### New Features

- **Chips: paste splitting and compact tags.** `DAChipsEdit` can split pasted text into multiple tags, elide long tag labels with a tooltip, show a selected-count label and guard IME composition so Enter during candidate selection does not commit a tag.
- **Collapse: keyboard navigation and variants.** `DACollapseWidget` supports arrow-key navigation, emits `expandRequested(QString)` before expanding and offers `Default`, `Borderless` and `Card` visual variants; `DACollapseItem` exposes its header widget and refreshes content height for wrapped content.
- **Select: grouped options and empty states.** `DASelectComboBox` accepts grouped option lists, shows a configurable empty panel with an optional create button, supports a clear-all button, and collapses excess tags into a `+N` summary.
- **Item list: drag reordering and filtering.** `DAItemListEditWidget` gains handle-based drag reordering (`orderChanged`), a filter box that only hides rows, an empty-state label and a two-click confirm mode for "Remove All".
- **Color: opacity, recent colors and default color.** `DAColorMenu` adds an opacity slider with checkerboard preview, a deduplicated 5-entry recent-color policy, and a "Restore Default" action; `DAColorGridWidget` gains optional keyboard navigation and checkerboard backgrounds for translucent swatches.
- **File path: drag and drop, validation and recent paths.** `DAFilePathEditWidget` accepts dropped files/folders, validates paths per mode (including missing targets in `SaveFile`) with a status row and `invalid` property, and offers a recent-path dropdown.
- **Font edit: per-item preview and recent families.** `DAFontEditPannelWidget` can render every font-family row with the panel's current size/weight/style, and shows a host-supplied recent-font menu without recording anything on its own.
- **Filter proxy: match ranges for delegates.** `DAStringFilterProxyModel::matchRanges()` and the static `matchRangesIn()` return `(start, length)` ranges of the filter term so a delegate can paint its own highlights; the demo ships a minimal delegate.
- **Check list: Shift range selection and Ctrl+A.** `DACheckListWidget` can select a range with Shift+click (skipping forced, disabled and uncheckable entries) and select all currently visible entries with Ctrl+A, emitting a single change signal per range.
- **Undo history: timestamps, state dots and empty-command filtering.** `DAUndoHistoryWidget` can show each command's local push time, draw applied/redoable state dots, expose `commandTimestamp()`, and hide commands whose text is empty.
- **Text edit: line numbers and find bar.** `DATextEditWidget` gains an optional line-number gutter and a Ctrl+F find bar with wrap-around search, match counter, case sensitivity, all-match highlighting and read-only support.
- **Documentation and gallery.** Both the Chinese and English guides cover every new option, each widget scene has bilingual documentation and refreshed gallery screenshots, and the single-file amalgamation was regenerated.

### Bug Fixes

- **Qt 5 font-preview crash.** Toggling the font preview on Qt 5 deleted the delegate saved for restoration because `QComboBox::setItemDelegate()` takes ownership; the preview delegate is now installed directly on the view, so Qt's delegate survives and is restored exactly.
- **File path editor destruction.** `DAFilePathEditWidget` disconnected its line edit before deleting the UI, preventing `editingFinished` from firing during destruction and touching freed widgets.
- **Flow layout and hidden items.** `DAFlowLayout` ignores empty items, so hidden placeholders no longer reserve space in the chips editor and popup empty states.

### Documentation

- Added Chinese and English sections for opacity and recent colors, file-path drag/validation/history, font preview and recent families, filter match ranges and a highlight delegate, keyboard range selection, undo timestamps and the empty-command fallback, and text-editor line numbers and find.
- Refreshed the gallery screenshots for the affected widgets and documented the `(start, length)` UTF-16 range semantics and the "empty filter returns no ranges" rule.

## tag:v0.0.1 (2026-11-23)

### New Features

- **First public release.** Version 0.0.1 ships the whole library under the MIT license. It is written in C++17 against Qt Widgets and Qt Svg, builds with CMake 3.16 or newer, and is compatible with Qt 5 and Qt 6 (verified with Qt 5.14.2 and Qt 6.7.3). The components follow the interaction patterns of Element UI while all public classes keep the `DA` prefix.
- **Selector family.** `DASelectComboBox` extends `QComboBox` with multi-tag selection, a filterable popup, allow-create and default-first-option, and offers two height modes (`GrowWithTags` and `FixedHeightScrollable`) while keeping the native arrow, border and keyboard mnemonics. The tags live in `DAChipsEdit`, a flow-layout tag input (Enter or a separator commits, Backspace removes the last tag, deduplication, max count, enum and string separator overloads) made of closable `DAChip` pills; `DAFlowLayout` wraps the tags and lets the trailing input fill the remaining width.
- **Collapse family.** `DACollapseWidget` stacks `DACollapseItem` panels with animated expand/collapse, accordion and multi-open modes, `activeNames()` state, caller-supplied title and content widgets and disabled items. `DACollapseLayout` keeps each item at its natural `sizeHint()` so the animation stays stable.
- **Property editing suite.** Editors for color (`DAColorPickerButton`, `DAColorMenu`, `DAColorPaletteGridWidget`, `DAColorGridWidget`), pen and brush (`DAPenEditWidget`, `DABrushEditWidget` plus the icon-generated `DAPenStyleComboBox` and `DABrushStyleComboBox`), font (`DAFontEditPannelWidget`, with two-row and single-row layouts), shape (`DAShapeEditPannelWidget`), alignment (`DAAligmentEditWidget`, `DATextAlignmentEditWidget`, `DAAligmentPositionEditWidget`), time format (`DATimeFormatEditWidget`), file paths (`DAFilePathEditWidget`, with open/save/directory modes), plain text (`DATextEditWidget`), labeled property rows (`DAPropertyItemWidget`) and RAII cursor guards (`DACursorScoped`, `DAWaitCursorScoped`).
- **Item selection and preview family.** `DAItemEntry` pairs display text with identifying data; `DAItemListEditWidget` edits a selected list (inline remove, candidate dropdown or searchable pick dialog, confirmed remove-all, optional up/down reordering); `DAItemListPickDialog` wraps `DACheckListWidget` for batch picking with select-all, invert and forced-checked entries; `DAStringListEditWidget` edits plain string lists with inline or caller-driven add; `DAColorScalePreviewWidget` paints a two/three-color gradient preview; `DAStringFilterProxyModel` filters list and tree models by display text or a custom role, with case sensitivity, column scope and parent passthrough for trees.
- **Utility widgets.** `DAUndoHistoryWidget` combines a `QUndoView` that follows a `QUndoGroup` with an undo-depth spin box that emits `undoLimitChangeRequested` (persistence stays with the caller). `DAAbstractWidgetOverlay` is an abstract base for painting an overlay on top of another widget, with `NoMask`, `MaskHint` and `AlphaMask` modes and configurable render modes.
- **Self-contained C++ interface.** Export (`DAWIDGETS_EXPORT`) and PIMPL (`DA_DECLARE_PRIVATE`, `DA_DECLARE_PUBLIC`, `DA_D`, `DA_Q`) macros are hand-written in `DAWidgetsGlobals.h`, so public headers no longer depend on CMake `GenerateExportHeader` output or a build-directory include path; `DAQt5Qt6Compat.h` hides the Qt 5/Qt 6 API differences. SVG icons are embedded through Qt RCC and rendered with Qt Svg, so no external image files are needed at runtime.
- **All-widgets demo with screenshot export.** `examples/all_widgets_demo` shows every public widget in nine tabs organized with `DACollapseWidget`; `--export-gallery` exports the main window, each tab, each widget scene and the popup close-ups used by the documentation, `--shot` saves a single tab and `--help` prints the command line options.
- **QtTest suites.** Twelve QtTest suites are registered with ctest and cover the selector, collapse, property editing, item selection, text editing, filter proxy, overlay and undo history behavior.
- **Single-file amalgamation.** `src-amalgamate/DAWidgets.h` and `src-amalgamate/DAWidgets.cpp` merge the entire library into two files that can be added to a project without CMake, a submodule or a prebuilt library; `examples/amalgamate_demo` verifies the pair by compiling it against Qt Widgets and Qt Svg only.

### Bug Fixes

- **Static-library builds link on GCC/Clang.** `Q_INIT_RESOURCE(dawidgetsicons)` was expanded inside `namespace DA` in three widget constructors, so GCC/Clang declared a `DA::qInitResources_dawidgetsicons()` symbol while the rcc-generated definition is global; static-link consumers (including the Linux CI job) failed with undefined references. Resource registration now goes through `DA::ensureResourcesInitialized()` (`src/DAWidgetsResources.cpp`), whose `Q_INIT_RESOURCE` expansion lives in the global namespace, and it runs only once.
- **Collapse animation no longer jitters or flickers.** The content height is calculated with `heightForWidth()` so wrapped content is measured correctly, locked once the animation ends, and `DACollapseLayout` gives every item its natural `sizeHint()` instead of letting `QVBoxLayout` compress or stretch rows mid-animation.
- **Public headers are self-contained.** Every header used to include a `DAWIDGETS_EXPORT.h` generated by CMake `GenerateExportHeader` in the build directory; the export macro now lives in `DAWidgetsGlobals.h` and the build directory was removed from the public include path, so qmake and other build systems can consume the headers.
- **Filtering keeps inline editors alive.** Attaching `DAStringFilterProxyModel` to a view that hosts inline widgets (for example a `QTreeWidget` row with a `QKeySequenceEdit`) lost those editors while filtering. The new static `matchesText()` exposes the proxy's matching rule, so such a view can keep its own model and filter with exactly the same semantics.
- **Horizontal and vertical alignment are set independently.** A single-choice button group cannot express combinations such as right + top; `DATextAlignmentEditWidget` provides two exclusive rows (left/center/right and top/middle/bottom) that map to one combined `Qt::Alignment`, and horizontal centering got its own icon because the previous center icon actually meant vertical centering.
- **Deprecated Qt APIs removed.** The Qt version checks now target Qt 5.15, `QButtonGroup::idClicked` is preferred over the deprecated `buttonClicked(int)`, and `DASelectComboBox` no longer uses `QApplication::globalStrut()`, which Qt 5.15 deprecated and Qt 6 removed.
- **Gradient preview paints without per-frame allocations.** `DAColorScalePreviewWidget` constructed a `QLinearGradient` and a brush on every `paintEvent`; the brush is now cached and rebuilt only when the colors or the widget size change.

### Build

- **CMake install and `find_package` support.** The build installs the library, its headers and an export target and generates `DAWidgetsConfig.cmake` and `DAWidgetsConfigVersion.cmake`, so consumers can call `find_package(DAWidgets)` and link `DA::DAWidgets` from an install tree.
- **Static and shared builds.** `DAWIDGETS_BUILD_SHARED` selects a static or a shared library; the hand-written export macro is driven by `DA_BUILT_AS_STATIC` and `DAWIDGETS_EXPORTS`, and on Windows shared builds the runtime DLL is copied next to each test and demo executable so `ctest` runs from the build directory without changing `PATH`.
- **Qt auto-detection and compiler flags.** CMake probes Qt 6 first and falls back to Qt 5 (`find_package(QT NAMES Qt6 Qt5 ...)`), links Qt Widgets and Qt Svg publicly, enables AUTOMOC, AUTORCC and AUTOUIC, requires C++17 and forces `/utf-8` on MSVC for the Chinese comments and literals.
- **Git worktree helper.** `scripts/worktree-add.ps1` creates, lists and removes task worktrees under `.worktree/`, and preconfigures each new worktree's `build/` directory from the main worktree's CMake cache (generator, Qt path and `DAWIDGETS_BUILD_*` options).
- **GitHub Actions.** CI workflows build and test the library, deploy the bilingual mkdocs site, and synchronize releases to Gitee.

### Documentation

- **Bilingual mkdocs site.** A documentation site built with MkDocs Material and the i18n plugin: Chinese at the root and English under `/en/`, with home, gallery, build guide, development guide and use guide sections.
- **Use guide for every widget.** The use guide documents every public widget in both languages with feature lists, API tables, code samples and effect screenshots; the documentation also covers the single-file amalgamation and its regeneration.
- **Developer guide and doc-writing guide.** Coding standards, comment standards, the PIMPL pattern, Qt 5/Qt 6 compatibility rules, multi-worktree development and a documentation writing guide describe how the library is built and how pages, screenshots and navigation are kept consistent.
- **Generated gallery.** `docs/gallery.md` and `docs/en/gallery.md` collect the main window, the nine tabs, per-widget scenes and popup close-ups, all exported by `--export-gallery` so screenshots are regenerated rather than hand-maintained.
- **Bilingual README.** `README.md` (English) and `README-CN.md` (Chinese) present the library, the installation options (`find_package`, `add_subdirectory`, single-file amalgamation, build from source), quick-start samples and links to the documentation, GitHub and Gitee.
