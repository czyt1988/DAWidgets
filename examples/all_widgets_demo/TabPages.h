#pragma once

#include "DemoScene.h"

// 每个函数返回一个标签页的场景列表，展示对应分类的控件。
// 新增控件时：写一个 createXxxContent()，再在对应列表里追加一条 DemoScene。
QVector< DemoScene > selectorScenes();       // 选择器 / 标签
QVector< DemoScene > collapseScenes();       // 折叠面板
QVector< DemoScene > colorScenes();          // 颜色相关
QVector< DemoScene > drawingStyleScenes();   // 绘图样式编辑器
QVector< DemoScene > alignmentScenes();      // 对齐编辑器
QVector< DemoScene > utilityScenes();        // 工具组件
QVector< DemoScene > itemSelectionScenes();  // 勾选列表 + 字符串列表
QVector< DemoScene > textScenes();           // 文本编辑器
QVector< DemoScene > filterScenes();         // 树 + 过滤代理
