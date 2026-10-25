---
title: DAWidgets 简介
description: DAWidgets 是一个受 Element UI 启发的 Qt QWidget 补充控件库，为桌面应用带来 Web 风格的组件交互。
keywords: [DAWidgets, Qt 控件库, Element UI, QWidget, 桌面控件]
---

# DAWidgets — Qt 控件补充库

DAWidgets 是一个 Qt **QWidget** UI 补充库，灵感来自 [Element UI](https://element.eleme.io/)。其目标是填补原生 Qt Widgets 控件集的空白，将 Web 风格的组件交互带入桌面应用。所有公共类以 `DA` 为前缀，统一放置在 `DA` 命名空间下。

## 为什么选择 DAWidgets？

Qt 原生 Widgets 提供了基础的控件集，但在现代 UI 交互方面存在不少空白：多标签选择器、流式标签输入、折叠面板、颜色/画笔编辑器等控件在 Qt 中要么缺失，要么需要大量手写代码。DAWidgets 填补这些空白，让 Qt 桌面开发也能拥有 Element UI 级别的组件体验。

| 特性 | 说明 |
|------|------|
| **Element UI 风格** | 组件设计对标 Element UI，交互逻辑一致，Web 开发者可无缝过渡 |
| **Qt5/Qt6 双兼容** | 通过兼容宏实现一套代码同时支持 Qt5 和 Qt6 |
| **PIMPL 模式** | 所有控件类使用 PIMPL 模式，二进制兼容，头文件干净 |
| **独立可用** | 每个组件可独立使用，也可组合使用 |
| **qss 可定制** | 自定义控件支持 qss 样式表，选择器格式 `DA--ClassName` |
| **枚举优先 API** | 固定取值参数提供枚举重载 + 字符串重载，类型安全 |

## 组件家族概览

### Selector 选择器家族

对标 Element UI 的 `el-select`（`multiple + filterable + allow-create`）。

| 类 | 说明 |
|----|------|
| `DASelectComboBox` | 多标签下拉选择框，继承 `QComboBox` |
| `DAChipsEdit` | 流式多标签输入框，可独立使用 |
| `DAChip` | 单个可关闭标签胶囊 |
| `DAFlowLayout` | 流式布局，末尾项可自适应宽度 |

### Collapse 折叠面板家族

对标 Element UI 的 `el-collapse` / `el-collapse-item`。

| 类 | 说明 |
|----|------|
| `DACollapse` | 折叠面板容器，支持手风琴/多展开模式 |
| `DACollapseItem` | 单个折叠条目，标题栏 + 内容区，动画展开/折叠 |

### 光标工具

| 类 | 说明 |
|----|------|
| `DACursorScoped` | 鼠标光标 RAII 守卫，异常安全恢复 |
| `DAWaitCursorScoped` | 等待光标守卫，附带 `DA_WAIT_CURSOR_SCOPED()` 宏 |

### 属性编辑家族

| 类 | 说明 |
|----|------|
| `DAPropertyItemWidget` | 属性项容器，支持 Inline/Below 两种布局 |
| `DATimeFormatEditWidget` | 时间格式设置 |
| `DAAligmentEditWidget` | 对齐方式编辑（左/右/上/下/居中） |
| `DAAligmentPositionEditWidget` | 九宫格对齐位置编辑 |
| `DAFilePathEditWidget` | 文件/目录路径选择（行编辑 + 浏览按钮，打开/保存/目录三种模式） |

### 条目选择与预览家族

| 类 | 说明 |
|----|------|
| `DAItemEntry` | 通用候选/选中条目：展示文本 + 标识数据（无效时按文本判同） |
| `DAItemListEditWidget` | 已选条目列表编辑器：行内删除 + 候选菜单/勾选对话框添加 + 移除所有 |
| `DAItemListPickDialog` | 条目勾选对话框：搜索过滤（过滤不丢勾选状态）+ 多选列表 |
| `DACheckListWidget` | 带搜索的勾选列表（可选全选/反选；forced 条目不可取消，过滤不改勾选） |
| `DAStringListEditWidget` | 纯字符串列表编辑器（Inline/External 两种 Add 模式、删除、上下移动、数量上限） |
| `DAColorScalePreviewWidget` | 2/3 色横向线性渐变预览条 |

### 样式编辑家族

| 类 | 说明 |
|----|------|
| `DABrushStyleComboBox` | Qt::BrushStyle 选择下拉框 |
| `DAPenStyleComboBox` | Qt::PenStyle 选择下拉框 |
| `DAColorGridWidget` | 基础颜色网格 |
| `DAColorPaletteGridWidget` | Office 风格调色板 |
| `DAColorMenu` | 颜色选择菜单 |
| `DAColorPickerButton` | 颜色拾取按钮 |
| `DAPenEditWidget` | 画笔编辑（颜色+线型+线宽） |
| `DABrushEditWidget` | 画刷编辑（纯色/图案/渐变/纹理） |
| `DAFontEditPannelWidget` | 字体编辑面板 |
| `DAShapeEditPannelWidget` | 形状编辑面板（画刷+画笔） |

## 快速集成

=== "CMake（推荐）"

    ```cmake
    # 查找并链接 DAWidgets
    find_package(DAWidgets REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE DA::DAWidgets Qt${QT_VERSION_MAJOR}::Widgets)
    ```

=== "直接引入源码"

    ```cpp
    // 将 src/ 目录下的源文件添加到你的项目中
    #include "DASelectComboBox.h"

    auto *combo = new DA::DASelectComboBox(parent);
    combo->setOptions({ "选项一", "选项二", "选项三" });
    combo->setAllowCreate(true);
    ```

## 技术规格

| 项 | 值 |
|----|----|
| 语言 | C++17 |
| 框架 | Qt Widgets（Qt5 / Qt6 双兼容） |
| 构建系统 | CMake ≥ 3.16 |
| 命名空间 | `DA` |
| 类前缀 | `DA` |
| 许可证 | MIT |
| 验证编译器 | MSVC 19.29 (VS2019) |
| 验证 Qt 版本 | Qt 6.7.3, Qt 5.14.2 |

## 许可证

```
MIT License

Copyright (c) 2026 Chen ZongYan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```
