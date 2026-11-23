# Changelog

Notable changes in DAWidgets are recorded in this file; version numbers follow [Semantic Versioning](https://semver.org/).

## Unreleased

*No unreleased changes yet.*

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
