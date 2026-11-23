#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate the DAWidgets README hero banners.

The banners are plain Pillow drawings (no external assets, no fonts beyond the
system ones), rendered at 2x and downsampled with LANCZOS for clean edges.

Usage:
    python docs/assets/brand/generate-hero.py

Outputs (deterministic; same input -> byte-identical PNGs):
    docs/assets/brand/hero-en.png   2024 x 576, English taglines
    docs/assets/brand/hero-cn.png   2024 x 576, Chinese taglines

The script is committed next to the PNGs so the banners can be regenerated
after a branding / wording change:

    # Windows: Segoe UI / Microsoft YaHei are used when available.
    python docs/assets/brand/generate-hero.py
"""

from __future__ import annotations

import colorsys
import os
import sys

from PIL import Image, ImageDraw, ImageFilter, ImageFont

# --------------------------------------------------------------------------- #
# Canvas / style constants
# --------------------------------------------------------------------------- #

CANVAS_W, CANVAS_H = 2024, 576
SS = 2  # supersampling factor: draw at 2x, downscale to 1x at the end
PRODUCT_VERSION = "0.0.1"

OUTPUT_DIR = os.path.dirname(os.path.abspath(__file__))

# Palette (kept in one place so both banners stay in sync)
BG_TOP = (10, 19, 36)
BG_BOTTOM = (4, 6, 13)
INK = (248, 250, 252)
TEXT_MUTED = (203, 213, 225)
TEXT_DIM = (139, 157, 181)
ACCENT = (34, 211, 238)
ACCENT_BLUE = (59, 130, 246)
CARD_BG_TOP = (17, 27, 46)
CARD_BG_BOTTOM = (11, 19, 33)
CARD_BORDER = (37, 50, 76)
FIELD_BG = (9, 17, 30)
FIELD_BORDER = (39, 54, 82)
LABEL = (104, 120, 145)


def s(value: float) -> int:
    """Scale a 1x design value to the supersampled canvas."""
    return int(round(value * SS))


# --------------------------------------------------------------------------- #
# Fonts
# --------------------------------------------------------------------------- #

FONT_DIRS = [
    os.path.join(os.environ.get("WINDIR", r"C:\Windows"), "Fonts"),
    "/usr/share/fonts",
    "/usr/local/share/fonts",
    "/Library/Fonts",
    "/System/Library/Fonts",
]

FONT_CANDIDATES = {
    # Windows first (the project is developed/verified on Windows), then a
    # best-effort fallback so the script still runs on other platforms.
    "bold": [
        "segoeuib.ttf",
        "SegoeUIBold.ttf",
        "bahnschrift.ttf",
        "arialbd.ttf",
        "truetype/dejavu/DejaVuSans-Bold.ttf",
    ],
    "semibold": [
        "seguisb.ttf",
        "SegoeUISemibold.ttf",
        "segoeuib.ttf",
        "arialbd.ttf",
        "truetype/dejavu/DejaVuSans-Bold.ttf",
    ],
    "regular": [
        "segoeui.ttf",
        "SegoeUI.ttf",
        "arial.ttf",
        "truetype/dejavu/DejaVuSans.ttf",
    ],
    "light": [
        "segoeuisl.ttf",
        "segoeuil.ttf",
        "segoeui.ttf",
        "truetype/dejavu/DejaVuSans.ttf",
    ],
    "cjk_regular": [
        "msyh.ttc",
        "msyhl.ttc",
        "simhei.ttf",
        "truetype/noto/NotoSansCJK-Regular.ttc",
        "truetype/arphic/uming.ttc",
    ],
    "cjk_bold": [
        "msyhbd.ttc",
        "msyh.ttc",
        "simhei.ttf",
        "truetype/noto/NotoSansCJK-Bold.ttc",
    ],
}

_FONT_FILES: dict[str, str] = {}
_FONT_CACHE: dict[tuple[str, int], ImageFont.FreeTypeFont] = {}


def _resolve_fonts() -> None:
    for kind, names in FONT_CANDIDATES.items():
        for name in names:
            for folder in FONT_DIRS:
                path = os.path.join(folder, name)
                if os.path.isfile(path):
                    _FONT_FILES[kind] = path
                    break
            if kind in _FONT_FILES:
                break
        if kind not in _FONT_FILES:
            sys.exit(
                "generate-hero.py: no usable font found for '%s'.\n"
                "Windows: install the Segoe UI / Microsoft YaHei fonts.\n"
                "Linux: install fonts-dejavu and a CJK font (e.g. Noto Sans CJK)." % kind
            )


def ui_font(kind: str, size_px: float) -> ImageFont.FreeTypeFont:
    """Return a cached font of ``kind`` at ``size_px`` design pixels."""
    key = (kind, s(size_px))
    if key not in _FONT_CACHE:
        _FONT_CACHE[key] = ImageFont.truetype(_FONT_FILES[kind], key[1])
    return _FONT_CACHE[key]


# --------------------------------------------------------------------------- #
# Low level drawing helpers
# --------------------------------------------------------------------------- #


def lerp_color(c0, c1, t: float):
    t = max(0.0, min(1.0, t))
    return tuple(int(round(a + (b - a) * t)) for a, b in zip(c0, c1))


def _gradient_image(size: tuple[int, int], c0, c1, diagonal: bool) -> Image.Image:
    """Build a smooth gradient from a tiny source image (fast + nicely banded-free)."""
    gw = gh = 64
    small = Image.new("RGB", (gw, gh))
    px = small.load()
    for j in range(gh):
        for i in range(gw):
            t = (i / (gw - 1) * 0.6 + j / (gh - 1) * 0.4) if diagonal else (j / (gh - 1))
            px[i, j] = lerp_color(c0, c1, t)
    return small.resize(size, Image.BICUBIC)


def rounded_gradient_panel(base: Image.Image, box, radius: float, c0, c1, diagonal=False) -> None:
    x, y, w, h = (int(round(v)) for v in box)
    w, h = max(w, 2), max(h, 2)
    grad = _gradient_image((w, h), c0, c1, diagonal)
    mask = Image.new("L", (w, h), 0)
    ImageDraw.Draw(mask).rounded_rectangle([0, 0, w - 1, h - 1], radius=radius, fill=255)
    base.paste(grad, (x, y), mask)


def panel(base: Image.Image, box, radius: float, fill=None, border=None, width: float = 1.0) -> None:
    x, y, w, h = (int(round(v)) for v in box)
    ImageDraw.Draw(base).rounded_rectangle(
        [x, y, x + w, y + h],
        radius=radius,
        fill=fill,
        outline=border,
        width=max(1, int(round(width))),
    )


def glow(base: Image.Image, box, radius: float, blur: float, color, alpha: int) -> None:
    """Soft colored halo behind an element."""
    x, y, w, h = (int(round(v)) for v in box)
    layer = Image.new("RGBA", base.size, (0, 0, 0, 0))
    ImageDraw.Draw(layer).rounded_rectangle(
        [x, y, x + w, y + h], radius=radius, fill=tuple(color) + (alpha,)
    )
    layer = layer.filter(ImageFilter.GaussianBlur(blur))
    base.alpha_composite(layer)


def shadow(base: Image.Image, box, radius: float, blur: float, alpha: int, dy: float = 0.0) -> None:
    x, y, w, h = (int(round(v)) for v in box)
    layer = Image.new("RGBA", base.size, (0, 0, 0, 0))
    ImageDraw.Draw(layer).rounded_rectangle(
        [x, y + dy, x + w, y + h + dy], radius=radius, fill=(0, 0, 0, alpha)
    )
    layer = layer.filter(ImageFilter.GaussianBlur(blur))
    base.alpha_composite(layer)


def text(base: Image.Image, xy, value: str, font: ImageFont.FreeTypeFont, fill, anchor: str = "la") -> None:
    ImageDraw.Draw(base).text(xy, value, font=font, fill=tuple(fill), anchor=anchor)


def chevron_down(base: Image.Image, cx: float, cy: float, size: float, color, width: float) -> None:
    d = ImageDraw.Draw(base)
    d.line(
        [(cx - size / 2, cy - size / 4), (cx, cy + size / 4), (cx + size / 2, cy - size / 4)],
        fill=tuple(color),
        width=max(1, int(round(width))),
        joint="curve",
    )


# --------------------------------------------------------------------------- #
# Background
# --------------------------------------------------------------------------- #


def build_background() -> Image.Image:
    """Dark navy banner background: diagonal gradient + dot grid + radial glows."""
    img = Image.new("RGB", (CANVAS_W, CANVAS_H), BG_BOTTOM)
    small = Image.new("RGB", (64, 64))
    px = small.load()
    for j in range(64):
        for i in range(64):
            # Diagonal ramp, slightly brighter towards the top-left.
            t = i / 63 * 0.42 + j / 63 * 0.38
            px[i, j] = lerp_color(BG_TOP, BG_BOTTOM, t)
    img = small.resize((CANVAS_W, CANVAS_H), Image.BICUBIC)

    def radial(center, radius, color, intensity):
        mask = Image.new("L", (CANVAS_W, CANVAS_H), 0)
        cx, cy = center
        ImageDraw.Draw(mask).ellipse(
            [cx - radius, cy - radius, cx + radius, cy + radius], fill=255
        )
        mask = mask.filter(ImageFilter.GaussianBlur(radius * 0.55))
        mask = mask.point(lambda v: int(v * intensity))
        return Image.composite(Image.new("RGB", (CANVAS_W, CANVAS_H), color), img, mask)

    img = radial((430, 300), 540, (37, 99, 235), 0.30)  # behind the wordmark
    img = radial((1490, 250), 540, (8, 145, 178), 0.30)  # behind the mock card
    img = radial((140, 40), 340, (29, 78, 216), 0.14)  # top-left lift

    # Soft vignette: keeps the corners dark so the wordmark and card pop.
    vig = Image.new("L", (CANVAS_W, CANVAS_H), 0)
    ImageDraw.Draw(vig).ellipse(
        [-CANVAS_W * 0.10, -CANVAS_H * 0.20, CANVAS_W * 1.10, CANVAS_H * 1.20], fill=255
    )
    vig = vig.filter(ImageFilter.GaussianBlur(CANVAS_W * 0.16))
    vig = vig.point(lambda v: int((255 - v) * 0.55))
    img = Image.composite(Image.new("RGB", (CANVAS_W, CANVAS_H), (1, 2, 6)), img, vig)

    # The background is smooth, so scaling it up from the 1x design resolution
    # is visually lossless and much faster than blurring at 2x.
    img = img.resize((s(CANVAS_W), s(CANVAS_H)), Image.BICUBIC)
    rgba = img.convert("RGBA")

    # Dot grid is drawn at the supersampled resolution so it stays crisp.
    dots = Image.new("RGBA", rgba.size, (0, 0, 0, 0))
    dd = ImageDraw.Draw(dots)
    r = max(1.0, SS * 1.35)
    step_x, step_y = s(30), s(30)
    for y in range(s(16), rgba.height, step_y):
        for x in range(s(16), rgba.width, step_x):
            dd.ellipse([x - r, y - r, x + r, y + r], fill=(148, 176, 220, 18))
    return Image.alpha_composite(rgba, dots)


# --------------------------------------------------------------------------- #
# Left block: logo tile, wordmark, taglines, capability pills
# --------------------------------------------------------------------------- #

TILE_X, TILE_Y, TILE_SIZE = 88, 138, 86


def draw_logo_tile(img: Image.Image) -> None:
    x, y, size = s(TILE_X), s(TILE_Y), s(TILE_SIZE)
    radius = s(21)
    glow(
        img,
        (s(TILE_X - 6), s(TILE_Y - 4), s(TILE_SIZE + 12), s(TILE_SIZE + 14)),
        radius,
        s(30),
        ACCENT,
        90,
    )
    rounded_gradient_panel(
        img,
        (x, y, size, size),
        radius,
        (37, 99, 235),
        (34, 211, 238),
        diagonal=True,
    )
    # Subtle inner top highlight for a glassy tile.
    panel(img, (x, y, size, size), radius, border=(255, 255, 255, 46), width=max(1, s(1)))
    # Abstract "widgets" glyph: stacked rounded bars + a pill.
    d = ImageDraw.Draw(img)
    bar1 = [x + s(18), y + s(23), x + s(18) + s(50), y + s(23) + s(9)]
    bar2 = [x + s(18), y + s(39), x + s(18) + s(38), y + s(39) + s(9)]
    pill = [x + s(18), y + s(55), x + s(18) + s(30), y + s(55) + s(11)]
    d.rounded_rectangle(bar1, radius=s(4.5), fill=(255, 255, 255, 245))
    d.rounded_rectangle(bar2, radius=s(4.5), fill=(214, 244, 255, 205))
    d.rounded_rectangle(pill, radius=s(5.5), fill=(255, 255, 255, 245))


def draw_pills(img: Image.Image, y: float, items: list[str]) -> None:
    font = ui_font("semibold", 20)
    pad_x = s(24)
    gap = s(14)
    h = s(48)
    x = s(TILE_X)
    for item in items:
        w = int(round(font.getlength(item))) + pad_x * 2
        panel(
            img,
            (x, s(y), w, h),
            h / 2,
            fill=(13, 25, 45),
            border=(52, 71, 102),
            width=max(1, s(1.5)),
        )
        text(img, (x + w / 2, s(y) + h / 2 - s(1)), item, font, TEXT_MUTED, anchor="mm")
        x += w + gap


def draw_left_block(img: Image.Image, tagline: str, sub_tagline: str, tagline_kind: str) -> None:
    draw_logo_tile(img)

    wordmark = ui_font("bold", 92)
    text(
        img,
        (s(TILE_X + TILE_SIZE + 26), s(TILE_Y + TILE_SIZE / 2 + 4)),
        "DAWidgets",
        wordmark,
        INK,
        anchor="lm",
    )

    tag_font = ui_font(tagline_kind, 34)
    text(img, (s(TILE_X), s(264)), tagline, tag_font, TEXT_MUTED, anchor="la")

    sub_font = ui_font("cjk_regular" if tagline_kind.startswith("cjk") else "regular", 21)
    text(img, (s(TILE_X), s(320)), sub_tagline, sub_font, TEXT_DIM, anchor="la")

    draw_pills(img, 390, ["Qt 5 / Qt 6", "C++17", "CMake", "MIT"])


# --------------------------------------------------------------------------- #
# Right block: polished mock application window
# --------------------------------------------------------------------------- #

CARD = (1056, 68, 880, 440)  # x, y, w, h in 1x design pixels
CARD_PAD = 24
CONTENT_X = CARD[0] + CARD_PAD
CONTENT_W = CARD[2] - CARD_PAD * 2


def draw_section_label(img: Image.Image, y: float, value: str) -> None:
    x = s(CONTENT_X)
    d = ImageDraw.Draw(img)
    d.rounded_rectangle(
        [x, s(y + 3), x + s(3.5), s(y + 13)], radius=s(2), fill=ACCENT
    )
    text(img, (x + s(13), s(y)), value, ui_font("semibold", 11.5), LABEL, anchor="la")


def draw_mock_window(img: Image.Image) -> None:
    x, y, w, h = CARD
    radius = s(18)
    # Halo + drop shadow, then the card body.
    glow(img, (s(x - 10), s(y - 6), s(w + 20), s(h + 24)), radius, s(26), (56, 189, 248), 26)
    shadow(img, (s(x), s(y), s(w), s(h)), radius, s(38), 155, dy=s(14))
    rounded_gradient_panel(img, (s(x), s(y), s(w), s(h)), radius, CARD_BG_TOP, CARD_BG_BOTTOM)
    panel(img, (s(x), s(y), s(w), s(h)), radius, border=CARD_BORDER, width=max(1, s(1.5)))

    # ---- title bar ----
    d = ImageDraw.Draw(img)
    dot_y = s(y + 21)
    for i, color in enumerate([(255, 95, 87), (254, 188, 46), (40, 200, 64)]):
        cx = s(x + 24 + i * 20)
        r = s(5.5)
        d.ellipse([cx - r, dot_y - r, cx + r, dot_y + r], fill=color)
    text(
        img,
        (s(x + 94), dot_y),
        "DAWidgets \u2014 component preview",
        ui_font("regular", 13.5),
        (125, 144, 170),
        anchor="lm",
    )
    text(
        img,
        (s(x + w - CARD_PAD), dot_y),
        "v" + PRODUCT_VERSION,
        ui_font("regular", 12),
        (76, 92, 118),
        anchor="rm",
    )
    d.line(
        [(s(x + CARD_PAD), s(y + 42)), (s(x + w - CARD_PAD), s(y + 42))],
        fill=(27, 39, 64),
        width=max(1, s(1)),
    )

    draw_mock_tags(img)
    draw_mock_property_row(img)
    draw_mock_palette(img)


def draw_mock_tags(img: Image.Image) -> None:
    draw_section_label(img, 130, "Tags")
    fx, fy, fw, fh = CONTENT_X, 152, CONTENT_W, 54
    panel(img, (s(fx), s(fy), s(fw), s(fh)), s(10), fill=FIELD_BG, border=FIELD_BORDER, width=max(1, s(1.5)))

    chip_font = ui_font("semibold", 14.5)
    close_w = s(9)
    chip_h = s(30)
    chip_cy = s(fy) + s(fh) // 2
    cx = s(fx + 12)
    for label in ["Qt", "C++", "Python"]:
        tw = int(round(chip_font.getlength(label)))
        chip_w = s(11) + tw + s(9) + close_w + s(11)
        panel(
            img,
            (cx, chip_cy - chip_h // 2, chip_w, chip_h),
            s(8),
            fill=(21, 41, 72),
            border=(42, 74, 116),
            width=max(1, s(1)),
        )
        text(img, (cx + s(11), chip_cy - s(1)), label, chip_font, (201, 226, 255), anchor="lm")
        # Close glyph.
        ix = cx + chip_w - s(11) - close_w // 2
        d = ImageDraw.Draw(img)
        stroke = max(1, int(round(s(1.6))))
        d.line([(ix - s(3.5), chip_cy - s(3.5)), (ix + s(3.5), chip_cy + s(3.5))], fill=(127, 184, 239), width=stroke)
        d.line([(ix - s(3.5), chip_cy + s(3.5)), (ix + s(3.5), chip_cy - s(3.5))], fill=(127, 184, 239), width=stroke)
        cx += chip_w + s(8)

    # Input caret + ghost placeholder.
    d = ImageDraw.Draw(img)
    d.line([(cx + s(2), chip_cy - s(9)), (cx + s(2), chip_cy + s(9))], fill=(96, 165, 250), width=max(1, s(1.5)))
    text(
        img,
        (cx + s(12), chip_cy - s(1)),
        "Add a tag\u2026",
        ui_font("regular", 14),
        (71, 88, 112),
        anchor="lm",
    )
    chevron_down(img, s(fx + fw - 22), chip_cy, s(11), (122, 143, 170), s(2))


def draw_mock_property_row(img: Image.Image) -> None:
    draw_section_label(img, 226, "Appearance")
    row_y = 248
    row_h = 38
    cy = row_y + row_h / 2

    # 1) color swatch
    sw = 38
    glow(
        img,
        (s(CONTENT_X - 3), s(row_y - 3), s(sw + 6), s(row_h + 6)),
        s(11),
        s(10),
        ACCENT_BLUE,
        60,
    )
    rounded_gradient_panel(
        img,
        (s(CONTENT_X), s(row_y), s(sw), s(row_h)),
        s(10),
        (56, 189, 248),
        (59, 130, 246),
        diagonal=True,
    )
    panel(
        img,
        (s(CONTENT_X), s(row_y), s(sw), s(row_h)),
        s(10),
        border=(255, 255, 255, 60),
        width=max(1, s(1)),
    )

    # 2) combo box
    combo_x, combo_w = CONTENT_X + 50, 250
    panel(
        img,
        (s(combo_x), s(row_y), s(combo_w), s(row_h)),
        s(9),
        fill=FIELD_BG,
        border=FIELD_BORDER,
        width=max(1, s(1.5)),
    )
    text(
        img,
        (s(combo_x + 14), s(cy - 1)),
        "Solid fill",
        ui_font("regular", 14.5),
        (195, 209, 227),
        anchor="lm",
    )
    chevron_down(img, s(combo_x + combo_w - 20), s(cy), s(10), (128, 148, 176), s(2))

    # 3) spin box
    spin_x, spin_w = combo_x + combo_w + 12, 120
    panel(
        img,
        (s(spin_x), s(row_y), s(spin_w), s(row_h)),
        s(9),
        fill=FIELD_BG,
        border=FIELD_BORDER,
        width=max(1, s(1.5)),
    )
    text(
        img,
        (s(spin_x + 14), s(cy - 1)),
        "12 px",
        ui_font("regular", 14.5),
        (195, 209, 227),
        anchor="lm",
    )
    d = ImageDraw.Draw(img)
    sep_x = s(spin_x + spin_w - 28)
    d.line([(sep_x, s(row_y + 8)), (sep_x, s(row_y + row_h - 8))], fill=(30, 44, 68), width=max(1, s(1)))
    up_x, dn_x = s(spin_x + spin_w - 14), s(spin_x + spin_w - 14)
    stroke = max(1, s(1.6))
    d.line([(up_x - s(4), s(cy - 5)), (up_x, s(cy - 10)), (up_x + s(4), s(cy - 5))], fill=(128, 148, 176), width=stroke, joint="curve")
    d.line([(dn_x - s(4), s(cy + 5)), (dn_x, s(cy + 10)), (dn_x + s(4), s(cy + 5))], fill=(128, 148, 176), width=stroke, joint="curve")

    # 4) gradient scale preview (DAColorScalePreviewWidget-style), framed like
    #    the other fields so the whole row reads as one property editor.
    frame_x, frame_w = 1532, 1912 - 1532
    panel(
        img,
        (s(frame_x), s(row_y), s(frame_w), s(row_h)),
        s(9),
        fill=FIELD_BG,
        border=FIELD_BORDER,
        width=max(1, s(1.5)),
    )
    bar_x = frame_x + 10
    bar_w = frame_w - 20
    bar_h = 14
    bar_y = cy - bar_h / 2
    stops = [(56, 189, 248), (139, 92, 246), (236, 72, 153)]
    gmask = Image.new("L", (s(bar_w), s(bar_h)), 0)
    ImageDraw.Draw(gmask).rounded_rectangle(
        [0, 0, s(bar_w) - 1, s(bar_h) - 1], radius=s(7), fill=255
    )
    grad = Image.new("RGB", (s(bar_w), 2))
    gd = ImageDraw.Draw(grad)
    for i in range(s(bar_w)):
        t = i / max(1, s(bar_w) - 1)
        c = lerp_color(stops[0], stops[1], t * 2) if t <= 0.5 else lerp_color(stops[1], stops[2], (t - 0.5) * 2)
        gd.line([(i, 0), (i, 1)], fill=c)
    grad = grad.resize((s(bar_w), s(bar_h)), Image.BICUBIC)
    img.paste(grad, (s(bar_x), s(bar_y)), gmask)
    panel(
        img,
        (s(bar_x), s(bar_y), s(bar_w), s(bar_h)),
        s(7),
        border=(255, 255, 255, 34),
        width=max(1, s(1)),
    )


ELEMENT_HUES = [220, 200, 182, 152, 120, 48, 28, 357, 335, 268]
ELEMENT_LIGHTNESS = [0.90, 0.78, 0.62, 0.46, 0.33]


def _hsl_to_rgb(h: float, s_: float, l_: float):
    r, g, b = colorsys.hls_to_rgb(h / 360.0, l_, s_)
    return (int(round(r * 255)), int(round(g * 255)), int(round(b * 255)))


def draw_mock_palette(img: Image.Image) -> None:
    draw_section_label(img, 306, "Palette")
    cols, rows = 10, 5
    gap = 7
    cell_h = 25
    cell_w = (CONTENT_W - gap * (cols - 1)) / cols
    y0 = 328
    d = ImageDraw.Draw(img)
    for r in range(rows):
        for c in range(cols):
            color = _hsl_to_rgb(ELEMENT_HUES[c], 0.74, ELEMENT_LIGHTNESS[r])
            x = CONTENT_X + c * (cell_w + gap)
            y = y0 + r * (cell_h + gap)
            d.rounded_rectangle(
                [s(x), s(y), s(x + cell_w), s(y + cell_h)],
                radius=s(4),
                fill=color,
            )


# --------------------------------------------------------------------------- #
# Banner assembly
# --------------------------------------------------------------------------- #


def render_banner(tagline: str, sub_tagline: str, tagline_kind: str) -> Image.Image:
    img = build_background()
    draw_left_block(img, tagline, sub_tagline, tagline_kind)
    draw_mock_window(img)
    return img.convert("RGB").resize((CANVAS_W, CANVAS_H), Image.LANCZOS)


BANNERS = {
    "hero-en.png": {
        "tagline": "Element UI patterns for Qt Widgets",
        "sub_tagline": "Multi-select, collapse, property editors and more \u2014 drop-in for desktop apps",
        "tagline_kind": "light",
    },
    "hero-cn.png": {
        "tagline": "\u4e3a Qt Widgets \u6ce8\u5165 Element UI \u7684\u7ec4\u4ef6\u4f53\u9a8c",
        "sub_tagline": "\u591a\u6807\u7b7e\u9009\u62e9\u3001\u6298\u53e0\u9762\u677f\u3001\u5c5e\u6027\u7f16\u8f91\u5668\u2026\u2026\u5f00\u7bb1\u5373\u7528\u7684\u684c\u9762\u7aef\u63a7\u4ef6",
        "tagline_kind": "cjk_regular",
    },
}


def main() -> int:
    _resolve_fonts()
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    for filename, spec in BANNERS.items():
        banner = render_banner(spec["tagline"], spec["sub_tagline"], spec["tagline_kind"])
        path = os.path.join(OUTPUT_DIR, filename)
        banner.save(path, "PNG", optimize=True)
        size = os.path.getsize(path)
        print("wrote %s (%dx%d, %.1f KiB)" % (path, banner.width, banner.height, size / 1024.0))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
