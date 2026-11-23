#!/usr/bin/env bash
#
# 重新生成 DAWidgets 单文件合并版本（src-amalgamate/DAWidgets.h + DAWidgets.cpp）。
#
# 与 tools/Amalgamate.ps1 等价，供 Git Bash / MSYS2 使用：Amalgamate.exe 是
# Windows 可执行文件，因此本脚本只能在 Windows 上的 POSIX shell 里运行。
# Linux/macOS 上的维护者请从 https://github.com/vinniefalco/Amalgamate
# 自行编译 Amalgamate 后，按脚本中的三条命令手动执行。
#
# 用法：
#   ./tools/Amalgamate.sh                       # 从 build*/CMakeCache.txt 自动找 Qt
#   QT_BIN=/d/Qt/6.7.3/msvc2019_64/bin ./tools/Amalgamate.sh
#
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
REPO_ROOT=$(cd "$SCRIPT_DIR/.." && pwd)
SRC_DIR="$REPO_ROOT/src"
TPL_DIR="$SCRIPT_DIR/amalgamate"
GEN_DIR="$TPL_DIR/generated"
OUT_DIR="$REPO_ROOT/src-amalgamate"
AMALGAMATE_EXE="$SCRIPT_DIR/Amalgamate.exe"

if [ ! -f "$AMALGAMATE_EXE" ]; then
    echo "错误：缺少 $AMALGAMATE_EXE" >&2
    exit 1
fi

# ---- 定位 Qt（uic / rcc） ----
if [ -z "${QT_BIN:-}" ]; then
    for cache in "$REPO_ROOT"/build*/CMakeCache.txt; do
        [ -f "$cache" ] || continue
        cmake_dir=$(grep -E '^(Qt6_DIR|Qt5_DIR|QT_DIR):PATH=' "$cache" | head -1 | cut -d= -f2- || true)
        [ -n "$cmake_dir" ] || continue
        prefix=$(dirname "$(dirname "$(dirname "$cmake_dir")")")
        if [ -x "$prefix/bin/uic.exe" ] || [ -x "$prefix/bin/uic" ]; then
            QT_BIN="$prefix/bin"
            echo "使用 $(basename "$(dirname "$cache")") 中的 Qt: $QT_BIN"
            break
        fi
    done
fi
if [ -z "${QT_BIN:-}" ]; then
    echo "错误：找不到 Qt 的 uic/rcc，请设置 QT_BIN，例如：" >&2
    echo "  QT_BIN=/d/Qt/6.7.3/msvc2019_64/bin ./tools/Amalgamate.sh" >&2
    exit 1
fi

find_tool() {
    local base=$1
    for candidate in "$QT_BIN/$base.exe" "$QT_BIN/$base"; do
        [ -x "$candidate" ] && { echo "$candidate"; return 0; }
    done
    return 1
}
UIC=$(find_tool uic) || { echo "错误：$QT_BIN 下缺少 uic" >&2; exit 1; }
RCC=$(find_tool rcc) || { echo "错误：$QT_BIN 下缺少 rcc" >&2; exit 1; }

# ---- 清理生成目录 ----
echo "==> 清理生成目录"
rm -rf "$GEN_DIR"
mkdir -p "$GEN_DIR" "$OUT_DIR"
# Amalgamate 在“输出与已有文件完全相同”时返回 1（不算错误），先删掉旧文件保证重写
rm -f "$OUT_DIR/DAWidgets.h" "$OUT_DIR/DAWidgets.cpp"

# ---- uic：.ui -> ui_*.h ----
echo "==> 生成 .ui 的 ui_*.h（uic）"
for ui in "$SRC_DIR"/*.ui; do
    [ -f "$ui" ] || continue
    base=$(basename "$ui" .ui)
    "$UIC" "$ui" -o "$GEN_DIR/ui_$base.h"
    echo "    ui_$base.h"
done

# ---- rcc：.qrc -> qrc_dawidgetsicons.cpp ----
echo "==> 生成资源代码 qrc_dawidgetsicons.cpp（rcc）"
"$RCC" --name dawidgetsicons "$SRC_DIR/resources/dawidgetsicons.qrc" \
    -o "$GEN_DIR/qrc_dawidgetsicons.cpp"

# ---- Amalgamate（Windows 可执行文件） ----
echo "==> 合并头文件 -> src-amalgamate/DAWidgets.h"
"$AMALGAMATE_EXE" \
    -i "$SRC_DIR" \
    -i "$GEN_DIR" \
    -i "$TPL_DIR" \
    -w '*.cpp;*.h;*.hpp;*.c' \
    "$TPL_DIR/DAWidgetsAmalgamTemplate.h" "$OUT_DIR/DAWidgets.h"

echo "==> 合并实现 -> src-amalgamate/DAWidgets.cpp"
"$AMALGAMATE_EXE" \
    -i "$SRC_DIR" \
    -i "$GEN_DIR" \
    -i "$TPL_DIR" \
    -w '*.cpp;*.h;*.hpp;*.c' \
    "$TPL_DIR/DAWidgetsAmalgamTemplate.cpp" "$OUT_DIR/DAWidgets.cpp"

# 规范化输出：去掉注释里的本机绝对路径，统一为 CRLF + UTF-8 BOM
echo "==> 规范化输出（相对路径、CRLF、UTF-8 BOM）"
REPO_ROOT_WIN=$(cd "$REPO_ROOT" && { pwd -W 2>/dev/null || pwd; })
for file in "$OUT_DIR/DAWidgets.h" "$OUT_DIR/DAWidgets.cpp"; do
    sed -i "s|$REPO_ROOT_WIN/||g" "$file"
    sed -i 's/\r$//; s/$/\r/' "$file"
    printf '\xef\xbb\xbf' > "$file.tmp"
    cat "$file" >> "$file.tmp"
    mv "$file.tmp" "$file"
done

echo "==> 结果"
for file in DAWidgets.h DAWidgets.cpp; do
    path="$OUT_DIR/$file"
    [ -f "$path" ] || { echo "错误：缺少 $path" >&2; exit 1; }
    echo "    src-amalgamate/$file  ($(wc -c < "$path") 字节)"
done

echo ""
echo "完成。这两个文件由工具生成，请勿手工编辑。"
echo "编译验证："
echo "  cmake -S . -B build-amalgamate -DCMAKE_PREFIX_PATH=<Qt> -DDAWIDGETS_BUILD_AMALGAMATE_DEMO=ON -DDAWIDGETS_BUILD_DEMO=OFF"
echo "  cmake --build build-amalgamate --config Release --target dawidgets_amalgamate_demo"
