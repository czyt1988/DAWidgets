# DATextEditWidget

`DATextEditWidget` is a general-purpose text viewer/editor: it can toggle read-only, monospace, and line wrap, while toolbar actions are injected by the caller. It does not own `OK` / `Cancel`; wrap it in a `QDialog` when modal interaction is needed.

## Preview

![DATextEditWidget: read-only/monospace/wrap toggles, injected "Save As" action, and monospace text](../images/gallery/widgets/text-edit.png)

The toolbar in the screenshot contains only the "Save As" action injected by the demo; the widget only displays it and forwards `triggered`. The four checkboxes below toggle read-only, monospace, line wrap, and toolbar visibility. Monospace keeps columns aligned, which suits logs and config snippets.

## Key Features

- ✅ **Read-only / editable toggle**: `setReadOnly()`
- ✅ **Monospace**: `setMonospace()`, ideal for logs and code
- ✅ **Line wrap**: `setLineWrapEnabled()`
- ✅ **Injectable tool actions**: `addToolAction(QAction*)`; the caller owns their lifetime
- ✅ **Line numbers** (optional): a left gutter, off by default
- ✅ **Find bar** (optional): `Ctrl+F`, wrap-around navigation and all-match highlighting, off by default
- ✅ **No business logic**: no built-in save/close behavior, easy to reuse in dialogs or dock panels

## Usage

```cpp
#include "DATextEditWidget.h"

auto *view = new DA::DATextEditWidget(parent);
view->setText("First line\nSecond line");
view->setPlaceholderText("Type text...");
view->setMonospace(true);
view->setLineWrapEnabled(true);
view->setReadOnly(true);

// Inject a tool action (the widget does not own it)
auto *saveAction = new QAction("Save As", view);
view->addToolAction(saveAction);
view->setToolbarVisible(true);

QObject::connect(saveAction, &QAction::triggered, []() {
    qDebug() << "Save As triggered";
});
QObject::connect(view, &DA::DATextEditWidget::textChanged, []() {
    qDebug() << "User edited the text";
});
```

!!! info "setText does not emit textChanged"
    `setText()` is for programmatic writes and does not emit `textChanged`; only user edits emit it, avoiding feedback loops in the caller.

### Line Numbers

```cpp
view->setLineNumbersVisible(true);   // default false
```

- Numbers are painted in a left gutter, follow scrolling, and the gutter width is recomputed from the total line count (also after `setMonospace()` changes the font).
- **Line number = document block number**: with line wrap enabled, a logical line folded into several visual rows still shows one number.
- When disabled, `viewportMargins` returns to zero and the gutter takes no width.

### Find Bar

```cpp
view->setFindEnabled(true);        // default false: allow Ctrl+F
view->showFindBar("ERROR");        // open and prefill
view->setFindBarVisible(false);    // hide directly
```

- After `setFindEnabled(true)`, pressing `Ctrl+F` while focus is inside the widget opens the find bar; `Esc` closes it and returns focus to the editor.
- The bar contains an input, previous/next, an `n / m` counter, a match-case checkbox and a close button. All matches are highlighted; the current one is darker.
- Wrap-around is supported: next after the last match returns to the first; with `m == 0` the counter shows `0 / 0` and the input gets the `invalid` dynamic property.
- **Works in read-only mode too** (it only moves the cursor and highlights, never edits text).
- If the host also uses `Ctrl+F`, call `setFindEnabled(false)` to yield the shortcut.
- Line numbers and find are pure view state and add no signals; `textChanged()` still fires only for user edits.

## Core Properties & Methods

| Method | Description |
|--------|-------------|
| `setReadOnly(bool)` / `isReadOnly()` | Read-only state |
| `setMonospace(bool)` / `isMonospace()` | Monospace font |
| `setLineWrapEnabled(bool)` / `isLineWrapEnabled()` | Line wrap |
| `setText(QString)` / `text()` | Set/get text (`setText` emits no signal) |
| `setPlaceholderText(QString)` | Empty-state placeholder |
| `addToolAction(QAction*)` | Inject an action into the toolbar |
| `setToolbarVisible(bool)` / `isToolbarVisible()` | Toolbar visibility |
| `setLineNumbersVisible(bool)` / `lineNumbersVisible()` | Left line-number gutter (default off) |
| `setFindEnabled(bool)` / `findEnabled()` | Allow `Ctrl+F` (default off) |
| `setFindBarVisible(bool)` / `findBarVisible()` | Find bar visibility |
| `showFindBar(QString)` / `hideFindBar()` | Open (optionally prefilled) / close the find bar |
| `setFindCaseSensitive(bool)` / `findCaseSensitive()` | Whether find is case-sensitive |

## References

- [DAUndoHistoryWidget](undo-overlay.md)
- Example path: `examples/all_widgets_demo` (Text tab)
