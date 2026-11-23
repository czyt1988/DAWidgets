# 控件画廊

这一页集中展示 DAWidgets 全部演示窗口截图。截图不是手工维护的：它们由示例程序 `examples/all_widgets_demo` 的一条命令自动导出，控件或布局改动后重新跑一次即可同步。

```bash
# 在仓库根目录执行（Windows 下确保 Qt 的 bin 目录在 PATH 中）
dawidgets_all_widgets_demo --export-gallery docs/images/gallery
```

命令会输出四类图片：

| 目录 | 内容 |
|------|------|
| `all-widgets-demo.png` | 主窗口首屏 |
| `tabs/NN-<slug>.png` | 每个标签页一张 |
| `widgets/<scene-id>.png` | 每个控件场景一张（文档配图） |
| `widgets/*-popup.png` | 下拉弹层、颜色菜单、条目勾选对话框特写 |

## 主窗口

![DAWidgets 全控件演示主窗口](images/gallery/all-widgets-demo.png)

一个窗口内用 9 个标签页收拢全部控件，每个标签页内部又用 `DACollapseWidget` 按控件分组，既方便逐个体验，也方便截图归档。

## 选择器 / 标签

![选择器 / 标签 tab：DAChip、DAChipsEdit、DAFlowLayout、DASelectComboBox](images/gallery/tabs/01-selector.png)

- [DAChip 标签胶囊](use-guide/chip.md)
- [DAChipsEdit 标签输入框](use-guide/chips-edit.md)
- [DAFlowLayout 流式布局](use-guide/flow-layout.md)
- [DASelectComboBox 下拉多选](use-guide/select-combobox.md)

## 折叠面板

![折叠面板 tab：手风琴/非手风琴容器与折叠条目变体](images/gallery/tabs/02-collapse.png)

- [DACollapseWidget 折叠容器](use-guide/collapse.md)
- [DACollapseItem 折叠条目](use-guide/collapse-item.md)

## 颜色

![颜色 tab：颜色拾取按钮、Office 调色板、颜色网格、颜色菜单](images/gallery/tabs/03-color.png)

- [DAColorPickerButton 颜色拾取按钮](use-guide/color-menu.md#dacolorpickerbutton)
- [DAColorPaletteGridWidget 调色板](use-guide/color-grid.md#dacolorpalettegridwidget)
- [DAColorGridWidget 颜色网格](use-guide/color-grid.md#dacolorgridwidget)
- [DAColorMenu 颜色菜单](use-guide/color-menu.md#dacolormenu)

## 绘图样式

![绘图样式 tab：画刷/画笔样式下拉框、画刷/画笔编辑器、形状与字体面板](images/gallery/tabs/04-drawing-style.png)

- [DABrushStyleComboBox 画刷样式下拉框](use-guide/brush-edit.md#dabrushstylecombobox)
- [DABrushEditWidget 画刷编辑器](use-guide/brush-edit.md#dabrusheditwidget)
- [DAPenStyleComboBox 画笔样式下拉框](use-guide/pen-edit.md#dapenstylecombobox)
- [DAPenEditWidget 画笔编辑器](use-guide/pen-edit.md#dapeneditwidget)
- [DAShapeEditPannelWidget 形状编辑面板](use-guide/shape-edit.md)
- [DAFontEditPannelWidget 字体编辑面板](use-guide/font-edit.md)

## 对齐

![对齐 tab：文本对齐、对齐方式与九宫格位置](images/gallery/tabs/05-alignment.png)

- [DATextAlignmentEditWidget 文本对齐](use-guide/alignment-edit.md#datextalignmenteditwidget)
- [DAAligmentEditWidget 对齐方式](use-guide/alignment-edit.md#daaligmenteditwidget)
- [DAAligmentPositionEditWidget 九宫格位置](use-guide/alignment-edit.md#daaligmentpositioneditwidget)

## 工具组件

![工具组件 tab：文件路径、时间格式、属性项、撤销历史、遮罩层、等待光标](images/gallery/tabs/06-utility.png)

- [DAFilePathEditWidget 文件路径](use-guide/file-path-edit.md)
- [DATimeFormatEditWidget 时间格式](use-guide/time-format-edit.md)
- [DAPropertyItemWidget 属性项](use-guide/property-item.md)
- [DAUndoHistoryWidget 撤销历史](use-guide/undo-overlay.md#daundohistorywidget)
- [DAAbstractWidgetOverlay 遮罩层](use-guide/undo-overlay.md#daabstractwidgetoverlay)
- `DAWaitCursorScoped`：等待光标是交互态，静态截图无法体现，详见[光标守卫类](use-guide/cursor-scoped.md)

## 条目选择

![条目选择 tab：勾选列表、forced 白名单、已选条目、勾选对话框、字符串列表、色阶预览](images/gallery/tabs/07-item-selection.png)

- [DACheckListWidget 勾选列表](use-guide/item-selection.md#dachecklistwidget)
- [DAItemListEditWidget 已选条目列表](use-guide/item-selection.md#daitemlisteditwidget)
- [DAItemListPickDialog 条目勾选对话框](use-guide/item-selection.md#daitemlistpickdialog)
- [DAStringListEditWidget 字符串列表](use-guide/item-selection.md#dastringlisteditwidget)
- [DAColorScalePreviewWidget 色阶预览](use-guide/item-selection.md#dacolorscalepreviewwidget)

## 文本

![文本 tab：DATextEditWidget 的只读、等宽、换行与工具条开关](images/gallery/tabs/08-text.png)

- [DATextEditWidget 文本编辑/查看](use-guide/text-edit.md)

## 过滤

![过滤 tab：树形模型按文本过滤，父节点自动放行](images/gallery/tabs/09-filter.png)

- [DAStringFilterProxyModel 文本过滤代理](use-guide/filter-proxy.md)

## 弹出层特写

有些控件的关键形态在弹出层里，单独导出：

![DASelectComboBox 候选列表弹层](images/gallery/widgets/select-combobox-popup.png)

![DAColorMenu 颜色菜单弹层](images/gallery/widgets/color-menu-popup.png)

![DAItemListPickDialog 条目勾选对话框](images/gallery/widgets/pick-dialog-window.png)

## 重新生成截图

截图由示例程序统一导出，不需要手动 PS：

```bash
# Windows（PowerShell）：把 Qt 的 bin 目录加入 PATH 后运行
$env:Path = "D:/Qt/6.7.3/msvc2019_64/bin;$env:Path"
./build/examples/all_widgets_demo/Release/dawidgets_all_widgets_demo.exe --export-gallery docs/images/gallery
```

`--export-gallery` 会遍历 `demoTabs()` 中的全部场景生成 `widgets/<id>.png`，再导出每个标签页与主窗口。新增控件时只需在对应 `Tab*.cpp` 的 `*Scenes()` 列表中追加一条 `DemoScene`，截图与演示界面会同时出现。
