# Collapse Stable Outer Position Design

## Goal

Make `DACollapseItem` expand and collapse without continuous sibling repositioning in the parent layout.

Desired interaction:

- Expand: sibling items move to their final positions once at animation start.
- Collapse: sibling items stay in place during the internal close animation, then move once when the animation ends.

The public API and `DACollapse` container semantics should stay unchanged.

## Current Problem

`DACollapseItem` currently animates `content.maximumHeight`. That changes the widget's layout height on every frame, so the parent `QVBoxLayout` recomputes geometry on every frame as well. The result is not an internal height glitch; it is repeated outer-layout repositioning while the animation runs.

## Design

### 1. Separate outer occupancy from inner reveal

Treat the item as having two independent heights:

- Occupancy height: the height that the parent layout sees.
- Reveal height: the visible content height shown inside the item during animation.

The fix is to stop using per-frame occupancy changes as the animation driver.

### 2. Expand behavior

When expanding:

1. Compute the natural content height.
2. Immediately switch the item occupancy height to `headerHeight + naturalContentHeight`.
3. Keep sibling layout movement to that single change.
4. Animate only the internal reveal from `0 -> naturalContentHeight`.

This keeps the outer layout stable after the initial reposition.

### 3. Collapse behavior

When collapsing:

1. Keep the current occupancy height fixed for the duration of the animation.
2. Animate the internal reveal from `naturalContentHeight -> 0`.
3. When the animation finishes, switch occupancy height back to header-only height.

This delays sibling movement until the end, so the outer layout still changes only once.

### 4. Internal structure

`DACollapseItem` should introduce a dedicated inner viewport/wrapper for the content area.

Recommended structure:

- `header`
- `contentViewport`
  - fixed in layout according to occupancy rules
  - clips child painting
- `content`
  - real content layout lives here
  - visual reveal is driven inside `contentViewport`

The animation should target an internal reveal property rather than `content.maximumHeight`.

Acceptable implementations:

- animate a custom integer reveal height and apply it to `contentViewport`
- animate viewport geometry directly

Not acceptable:

- continuing to drive the main effect by changing the parent-visible height every frame

## Constraints

- Keep existing public API:
  - `setExpanded`
  - `animationEnabled`
  - `setContentWidget`
  - `contentLayout`
- Preserve current `heightForWidth`-based natural height calculation.
- Preserve non-animated mode behavior.
- Preserve accordion and multi-expand behavior in `DACollapse`.
- Keep opt-in debug logging usable during verification.

## Testing

Add regression coverage for outer-position stability:

- Expand test:
  - in a parent `QVBoxLayout`, expanding one item should cause a sibling item's `y` position to change exactly once.
- Collapse test:
  - collapsing one item should cause a sibling item's `y` position to change exactly once, after the close animation.

Keep existing tests passing:

- content height correctness for word wrap
- collapse animation start height correctness
- expanded sibling content does not get squeezed
- debug logging opt-in behavior

## Out of Scope

- changing public API
- redesigning `DACollapse`
- adding scroll behavior
- changing visual style, timing curve, or animation duration unless required by the implementation

## Risks

- if occupancy and reveal become inconsistent, `sizeHint` or layout requests may drift from the real visual state
- content changes while expanded may require recomputing both occupancy height and reveal height
- collapse completion must not emit intermediate geometry that reintroduces repeated sibling moves

## Acceptance Criteria

- Expand causes one outer reposition at the start, then only internal reveal animation.
- Collapse causes one outer reposition at the end, after internal hide animation.
- Existing public behavior remains compatible.
- The focused regression tests and the existing collapse test suite pass.
