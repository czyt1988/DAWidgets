#pragma once

#include <QString>

/**
 * @brief 一键导出演示窗口与全部控件场景的截图。
 *
 * 输出目录结构：
 * ```
 * <dir>/all-widgets-demo.png      主窗口（第一个标签页）
 * <dir>/tabs/NN-<slug>.png        每个标签页一张
 * <dir>/widgets/<scene-id>.png    每个场景单独一张（文档配图）
 * <dir>/widgets/*-popup.png       弹出菜单/对话框/下拉弹层特写
 * ```
 *
 * @param dir 输出目录（不存在会自动创建）
 * @param errorMessage 失败时写入原因
 * @return 成功返回 true
 */
bool exportDemoGallery(const QString &dir, QString *errorMessage = nullptr);
