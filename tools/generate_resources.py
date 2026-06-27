#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DAWidgets 资源生成脚本

扫描 tools/ 目录下所有 .png 文件，生成 src/dawidgets_resources.h 与
src/dawidgets_resources.cpp，把图片以 C++ 字节数组形式内嵌进库，
避免依赖 Qt RCC（静态库的资源不会自动传递给链接它的可执行文件）。

更换资源流程：
  1. 用同名 PNG 替换 tools/ 下的文件（例如替换 arrow_down.png）
  2. 运行：python tools/generate_resources.py
  3. 重新构建库即可

新增资源流程：
  1. 把新 PNG 放进 tools/ 目录（例如 add.png）
  2. 运行本脚本
  3. 代码中用 dawidgets::resources::load("add") 获取 QPixmap

用法：
  python tools/generate_resources.py            # 默认相对路径
  python tools/generate_resources.py /path/to   # 指定项目根目录
"""
import os
import sys

# 文件名 -> C++ 标识符：arrow_down.png -> arrowDown
def to_identifier(name):
    base = name[:-4] if name.lower().endswith('.png') else name
    parts = base.split('_')
    return ''.join(p[:1].upper() + p[1:] for p in parts if p)

def to_camel(name):
    ident = to_identifier(name)
    return ident[:1].lower() + ident[1:] if ident else ident

def main():
    project_root = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    tools_dir = os.path.join(project_root, 'tools')
    out_h = os.path.join(project_root, 'src', 'dawidgets_resources.h')
    out_cpp = os.path.join(project_root, 'src', 'dawidgets_resources.cpp')

    if not os.path.isdir(tools_dir):
        print('ERROR: tools dir not found: ' + tools_dir)
        return 1

    pngs = sorted(f for f in os.listdir(tools_dir) if f.lower().endswith('.png'))
    if not pngs:
        print('WARN: no .png files in ' + tools_dir)

    # ---- 生成头文件 ----
    h_lines = []
    h_lines.append('// ============================================================') 
    h_lines.append('// 自动生成，请勿手动修改。')
    h_lines.append('// 由 tools/generate_resources.py 从 tools/*.png 生成。')
    h_lines.append('// 修改资源：替换 tools/ 下同名 PNG 后重新运行该脚本。')
    h_lines.append('// ============================================================')
    h_lines.append('#ifndef DAWIDGETS_RESOURCES_H')
    h_lines.append('#define DAWIDGETS_RESOURCES_H')
    h_lines.append('#include <cstddef>')
    h_lines.append('#include <QPixmap>')
    h_lines.append('#include <QString>')
    h_lines.append('')
    h_lines.append('namespace dawidgets { namespace resources {')
    h_lines.append('')
    h_lines.append('// 每个资源对应一个字节数组与长度。资源名 = 文件名去掉 .png 后缀。')
    for name in pngs:
        data = open(os.path.join(tools_dir, name), 'rb').read()
        var = to_camel(name)
        h_lines.append('// {}: {} bytes'.format(name, len(data)))
        h_lines.append('extern const unsigned char k{}[];'.format(to_identifier(name)))
        h_lines.append('extern const std::size_t k{}Size;'.format(to_identifier(name)))
        h_lines.append('')
    h_lines.append('// 按资源名（如 "arrow_down"）加载 QPixmap。找不到返回空 QPixmap。')
    h_lines.append('QPixmap load(const QString &name);')
    h_lines.append('')
    h_lines.append('}} // namespace dawidgets::resources')
    h_lines.append('#endif // DAWIDGETS_RESOURCES_H')

    with open(out_h, 'w', encoding='utf-8') as f:
        f.write('\n'.join(h_lines) + '\n')
    print('generated ' + out_h)

    # ---- 生成 cpp 文件 ----
    c_lines = []
    c_lines.append('// 自动生成，请勿手动修改。由 tools/generate_resources.py 生成。')
    c_lines.append('#include "dawidgets_resources.h"')
    c_lines.append('#include <QByteArray>')
    c_lines.append('#include <QBuffer>')
    c_lines.append('#include <QHash>')
    c_lines.append('')
    c_lines.append('namespace dawidgets { namespace resources {')
    c_lines.append('')
    for name in pngs:
        data = open(os.path.join(tools_dir, name), 'rb').read()
        ident = to_identifier(name)
        hex_bytes = ', '.join('0x{:02X}'.format(b) for b in data)
        # 每行最多 12 个字节，便于阅读
        lines = []
        for i in range(0, len(hex_bytes.split(', ')), 12):
            chunk = ', '.join(hex_bytes.split(', ')[i:i+12])
            lines.append('    ' + chunk + ',')
        body = '\n'.join(lines).rstrip(',\n')
        c_lines.append('const unsigned char k{}[] = {{'.format(ident))
        c_lines.append(body)
        c_lines.append('};')
        c_lines.append('const std::size_t k{}Size = {};'.format(ident, len(data)))
        c_lines.append('')
    c_lines.append('QPixmap load(const QString &name)')
    c_lines.append('{')
    c_lines.append('    static QHash<QString, QPixmap> cache;')
    c_lines.append('    if (cache.contains(name))')
    c_lines.append('        return cache.value(name);')
    c_lines.append('    QPixmap pm;')
    for name in pngs:
        ident = to_identifier(name)
        base = name[:-4]
        c_lines.append('    if (name == QStringLiteral("{}")) {{'.format(base))
        c_lines.append('        pm.loadFromData(k{}, static_cast<int>(k{}Size));'.format(ident, ident))
        c_lines.append('    }')
    c_lines.append('    cache.insert(name, pm);')
    c_lines.append('    return pm;')
    c_lines.append('}')
    c_lines.append('')
    c_lines.append('}} // namespace dawidgets::resources')

    with open(out_cpp, 'w', encoding='utf-8') as f:
        f.write('\n'.join(c_lines) + '\n')
    print('generated ' + out_cpp)
    print('done: {} resources'.format(len(pngs)))
    return 0

if __name__ == '__main__':
    sys.exit(main())
