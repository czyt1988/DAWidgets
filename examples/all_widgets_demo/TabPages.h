#pragma once
#include <QWidget>

// 每个函数创建一个 Tab 页面，展示对应分类的控件
QWidget *createSelectorTab(QWidget *parent);       // 选择器 / 标签
QWidget *createCollapseTab(QWidget *parent);       // 折叠面板
QWidget *createColorTab(QWidget *parent);          // 颜色相关
QWidget *createDrawingStyleTab(QWidget *parent);  // 绘图样式编辑器
QWidget *createAlignmentTab(QWidget *parent);      // 对齐编辑器
QWidget *createUtilityTab(QWidget *parent);        // 工具组件
