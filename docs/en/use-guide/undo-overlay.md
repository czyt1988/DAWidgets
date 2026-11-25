# Undo History & Overlay

`DAUndoHistoryWidget` is an undo history panel: a `QUndoView` following the active stack of a `QUndoGroup`, plus an undo-depth entry. `DAAbstractWidgetOverlay` is the abstract base for lightweight overlays drawn on top of a target widget.

## Preview

![DAUndoHistoryWidget: QUndoView history and undo-depth spin box](../images/gallery/widgets/undo-history.png)

The history list comes from three commands pre-pushed by the demo; clicking any row moves back/forward to that state. The spin box at the top is the depth entry: changes only emit `undoLimitChangeRequested`, and persistence plus application to each stack stay in the caller.

## DAUndoHistoryWidget

### Key Features

- ✅ **Follows the active stack**: after `setUndoGroup(QUndoGroup*)`, it shows the current page's history
- ✅ **Click to undo/redo**: reuses `QUndoView` interaction
- ✅ **Depth entry**: `undoLimit()` / `setUndoLimit()`; spin-box changes only emit a signal
- ✅ **Timestamps and state dots** (optional): push time and applied/re-doable state per row, off by default
- ✅ **Empty-command fallback** (optional): hide rows with empty text, off by default
- ✅ **Clean layering**: the widget does not read configuration or walk data-page stacks

### Usage

```cpp
#include "DAUndoHistoryWidget.h"
#include <QUndoGroup>
#include <QUndoStack>

auto *stack = new QUndoStack(parent);
auto *group = new QUndoGroup(parent);
group->addStack(stack);
group->setActiveStack(stack);

auto *history = new DA::DAUndoHistoryWidget(parent);
history->setUndoGroup(group);

// User changed the depth: the caller persists it and applies it to each stack
QObject::connect(history, &DA::DAUndoHistoryWidget::undoLimitChangeRequested,
                 [](int limit) { qDebug() << "new undo depth:" << limit; });
```

### Timestamps and State Dots

```cpp
history->setTimestampsVisible(true);   // default false: push time on the right of every row
history->setIconsVisible(true);        // default false: filled = applied, hollow = redoable

// Indexed by QUndoStack index (not the view row)
QDateTime t = history->commandTimestamp(0);
```

- Time uses the fixed `HH:mm:ss` format in local time and lives only in this process's memory; it is never written to `QSettings`.
- The time is recorded when a command is pushed: undo/redo only move the pointer and never change existing timestamps; when a new command truncates the redo branch, the old timestamps are dropped with it.
- With both switches off nothing changes — no pixels, no row height. When on, the widget only paints in the left/right margins of each row.
- State dots are derived from `QUndoStack::index()`; the host does not need to attach data to commands.

### Empty Command Fallback

```cpp
history->setEmptyCommandFilterEnabled(true);   // default false
```

- When enabled, rows whose `text().trimmed().isEmpty()` are hidden, enforcing the existing "no blank entries in the panel" rule.
- The initial-state row (the first `QUndoView` row) is never hidden; hiding is recomputed after a stack switch or model reset.
- Off by default; without the call the behavior is identical to before.

### Core Methods

| Method | Description |
|--------|-------------|
| `setUndoGroup(QUndoGroup*)` / `undoGroup()` | Bind/get the undo group |
| `undoLimit()` / `setUndoLimit(int)` | Read/inject depth (`setUndoLimit` emits no signal) |
| `undoLimitChangeRequested(int)` | Emitted when the user changes the depth |
| `setTimestampsVisible(bool)` / `timestampsVisible()` | Show push time per row (default off) |
| `setIconsVisible(bool)` / `iconsVisible()` | Show state dots per row (default off) |
| `setEmptyCommandFilterEnabled(bool)` / `isEmptyCommandFilterEnabled()` | Hide empty-text commands (default off) |
| `commandTimestamp(int)` | Push time of a command; invalid when out of range |

## DAAbstractWidgetOverlay

![DAAbstractWidgetOverlay: translucent overlay covering a target widget](../images/gallery/widgets/overlay.png)

The overlay sits on top of a target widget (a QLabel in the screenshot) and follows its resize. `MaskMode` controls which region of the underlying window must repaint (`NoMask` / `MaskHint` / `AlphaMask`), and `RenderMode` controls whether the alpha buffer is reused; call `updateOverlay()` after switching.

### Key Features

- ✅ **Less repainting**: only the masked region of the target repaints
- ✅ **Three mask modes**: `NoMask`, `MaskHint` (default), `AlphaMask`
- ✅ **Three render modes**: `AutoRenderMode`, `CopyAlphaMask`, `DrawOverlay`
- ✅ **Subclasses only paint**: override `drawOverlay(QPainter*)`

### Usage

```cpp
#include "DAAbstractWidgetOverlay.h"

class SelectionOverlay : public DA::DAAbstractWidgetOverlay
{
public:
    explicit SelectionOverlay(QWidget *target)
        : DA::DAAbstractWidgetOverlay(target)
    {
        resize(target->size());
        show();
        raise();
    }

protected:
    void drawOverlay(QPainter *painter) const override
    {
        painter->fillRect(rect(), QColor(64, 158, 255, 90));
        painter->drawRect(rect().adjusted(1, 1, -2, -2));
    }
};

auto *overlay = new SelectionOverlay(targetWidget);
overlay->setMaskMode(DA::DAAbstractWidgetOverlay::MaskHint);
overlay->updateOverlay();
```

!!! info "An overlay is not a layout child"
    The overlay only paints on top of the target and does not participate in layout. After the target resizes, resize the overlay and call `updateOverlay()`.

## References

- [DATextEditWidget](text-edit.md)
- Example path: `examples/all_widgets_demo` (Utility tab)
