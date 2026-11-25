# DAStringFilterProxyModel

`DAStringFilterProxyModel` is a general text-filtering proxy for lists and trees: it matches `Qt::DisplayRole` (or a custom role), is case-insensitive by default, can be restricted to a column, and — for tree models — can keep the parents of matching children visible.

## Preview

![DAStringFilterProxyModel: tree filtering keeps parents of matching children](../images/gallery/widgets/filter-tree.png)

The tree has three groups. Typing a filter (for example `ban`, `胡萝`, or `wrench`) keeps only matching leaves and their parent chain; clearing the input restores everything. The demo shows the filtered top-level count versus the source count below.

## Key Features

- ✅ **Display-text filtering**: `Qt::DisplayRole` by default; change it with `setFilterRole()`
- ✅ **Case policy**: `setFilterCaseSensitivity()`, insensitive by default
- ✅ **Single/all columns**: `setFilterColumn(-1)` matches all columns
- ✅ **Parent pass-through**: `setIncludeParentsOfMatches(true)` keeps parents of matching children
- ✅ **Reusable matching**: static `matchesText()` shares the exact semantics with views that cannot use a proxy
- ✅ **Match ranges**: `matchRanges()` / `matchRangesIn()` so a delegate can paint its own highlights

## Usage

```cpp
#include "DAStringFilterProxyModel.h"
#include <QStandardItemModel>
#include <QTreeView>

auto *model = new QStandardItemModel(parent);
auto *fruit = new QStandardItem("Fruit");
fruit->appendRow(new QStandardItem("Apple"));
fruit->appendRow(new QStandardItem("Banana"));
model->appendRow(fruit);

auto *proxy = new DA::DAStringFilterProxyModel(parent);
proxy->setSourceModel(model);
proxy->setIncludeParentsOfMatches(true);   // required for trees
proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

auto *view = new QTreeView(parent);
view->setModel(proxy);
view->setHeaderHidden(true);
view->expandAll();

// Update the filter text as the user types
QObject::connect(searchEdit, &QLineEdit::textChanged,
                 proxy, &DA::DAStringFilterProxyModel::setFilterText);
```

!!! warning "Enable includeParentsOfMatches for trees"
    The proxy checks each row on its own by default. If a parent's text does not contain the filter term, the parent is hidden and its children become unreachable. For tree data, set `setIncludeParentsOfMatches(true)`.

## Match Ranges and Highlighting

![DAStringFilterProxyModel: match highlighting in a tree and a multi-column table](../images/gallery/widgets/filter-highlight.png)

### Range Semantics

```cpp
// Character ranges of the filter term inside one cell; each element is (start, length)
const QVector<QPair<int,int>> ranges = proxy->matchRanges(proxy->index(row, column));

// Views without a proxy can use the pure-text version
const auto same = DA::DAStringFilterProxyModel::matchRangesIn(text, filter, Qt::CaseInsensitive);
```

- `start` / `length` are **UTF-16 code unit** offsets; `text.mid(start, length)` is the matched substring.
- Ranges are left-to-right and **never overlap**: `"aaaa"` + `"aa"` yields `{(0,2),(2,2)}`, not three ranges.
- **An empty filter returns an empty vector** (nothing highlighted), while `matchesText()` returns `true` for an empty filter (everything passes) — "passes" and "highlighted" are different questions.
- With `setFilterColumn(n)`, only that column has ranges; invalid indexes or indexes that do not belong to this proxy return an empty vector.
- In trees, a parent kept only because a child matches has **no ranges of its own** (its own text truly did not match).
- Ranges come from the `filterRole` text. Only when that text equals the displayed `DisplayRole` text can the offsets be painted directly; otherwise they will be misaligned.

### Minimal Delegate Example

```cpp
class HighlightDelegate : public QStyledItemDelegate
{
public:
    explicit HighlightDelegate(DA::DAStringFilterProxyModel* proxy)
        : mProxy(proxy) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        const auto ranges = mProxy->matchRanges(index);
        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);

        // No ranges / selected / too wide: fall back to native painting
        if (ranges.isEmpty() || (opt.state & QStyle::State_Selected)) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        const QString text = opt.text;
        opt.text.clear();
        opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        const QRect r = opt.widget->style()->subElementRect(
            QStyle::SE_ItemViewItemText, &opt, opt.widget);
        const QFontMetrics fm(opt.font);
        const int baseline = r.top() + (r.height() - fm.height()) / 2 + fm.ascent();

        int x = r.left() + 2, last = 0;
        for (const auto& hit : ranges) {
            const QString before = text.mid(last, hit.first - last);
            painter->drawText(QPoint(x, baseline), before);
            x += fm.horizontalAdvance(before);
            painter->fillRect(QRect(x, r.top() + 1,
                                    fm.horizontalAdvance(text.mid(hit.first, hit.second)),
                                    r.height() - 2),
                              QColor(255, 214, 102, 130));   // highlight background
            painter->drawText(QPoint(x, baseline), text.mid(hit.first, hit.second));
            x += fm.horizontalAdvance(text.mid(hit.first, hit.second));
            last = hit.first + hit.second;
        }
        painter->drawText(QPoint(x, baseline), text.mid(last));
    }

private:
    DA::DAStringFilterProxyModel* mProxy;
};

view->setItemDelegate(new HighlightDelegate(proxy));
```

- The complete implementation (baseline, clipping, fallbacks) is the demo's `filter-highlight` scene; the library does **not** ship a highlight delegate, leaving selection colors, elision and styling to the caller.
- Computing ranges inside every `paint()` is fine: `setFilterText()` invalidates the filter and the view repaints with fresh values.

## Core Methods

| Method | Description |
|--------|-------------|
| `setFilterText(QString)` / `filterText()` | Filter text (empty = no filtering) |
| `setFilterCaseSensitivity(Qt::CaseSensitivity)` | Case policy |
| `setFilterColumn(int)` | Column to match (`-1` = all columns) |
| `setFilterRole(int)` | Data role to match (default `Qt::DisplayRole`) |
| `setIncludeParentsOfMatches(bool)` | Keep parents of matching children |
| `matchesText(text, filter, cs)` | Static matcher with identical semantics |
| `matchRanges(proxyIndex)` | Match ranges `(start, length)` for one cell, for delegate highlighting |
| `matchRangesIn(text, filter, cs)` | Pure-text ranges; an empty filter returns an empty vector |

## References

- [DACheckListWidget](item-selection.md)
- Example path: `examples/all_widgets_demo` (Filter tab)
