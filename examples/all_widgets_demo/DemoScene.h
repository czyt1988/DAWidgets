#pragma once

#include <QString>
#include <QVector>
#include <QWidget>

/**
 * @brief 一个可独立演示/截图的控件场景。
 *
 * id 同时用作折叠项 name、截图文件名与文档锚点，因此保持 kebab-case 且全局唯一。
 * preferredHeight 用于含伸缩子控件（列表、树、文本区）的场景：0 表示按 sizeHint 自适应。
 */
struct DemoScene
{
    QString id;
    QString title;
    QWidget *(*create)(QWidget *parent);
    int preferredHeight;
};

/**
 * @brief 一个演示标签页：标题 + 场景列表（顺序即界面顺序）。
 */
struct DemoTab
{
    QString title;
    QString slug;  ///< 截图文件名用的英文短名
    QVector< DemoScene > scenes;
};

// 全部标签页与场景，顺序即演示窗口中的 tab 顺序
QVector< DemoTab > demoTabs();

// 按场景列表构建可滚动标签页；expandAll=true 时展开全部折叠项（截图用）
QWidget *createSceneTab(const QVector< DemoScene > &scenes, QWidget *parent, bool expandAll = false);
