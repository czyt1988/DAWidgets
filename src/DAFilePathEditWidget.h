#ifndef DAFILEPATHEDITWIDGET_H
#define DAFILEPATHEDITWIDGET_H

#include <QWidget>
#include "DAWidgetsGlobals.h"
namespace Ui
{
class DAFilePathEditWidget;
}

class QDragEnterEvent;
class QDragLeaveEvent;
class QDropEvent;

namespace DA
{

/**
 * @brief 文件/目录路径编辑窗口，可通过此窗口选中一个路径
 *
 * 形如下面的选择按钮
 * --------------  ----
 * |C:/src      | |    |
 * --------------  ----
 *
 * 三种模式共用同一套「文本框 + 浏览按钮」布局：
 * - OpenFile：选择已存在文件（QFileDialog::ExistingFile）
 * - SaveFile：选择保存路径（AcceptSave + 默认后缀，保留 Qt 自带覆盖确认）
 * - ExistingDirectory：选择目录（Directory + ShowDirsOnly）
 */
class DAWIDGETS_EXPORT DAFilePathEditWidget : public QWidget
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DAFilePathEditWidget)
public:
    /// 浏览模式
    enum class Mode
    {
        OpenFile,          ///< 选择已存在文件
        SaveFile,          ///< 选择保存文件路径
        ExistingDirectory  ///< 选择已存在目录
    };

    // 构造文件路径编辑窗口
    explicit DAFilePathEditWidget(QWidget* parent = nullptr);
    // 析构，释放 ui 资源
    ~DAFilePathEditWidget();

    // 设置过滤
    void setNameFilter(const QString& filter);
    void setNameFilters(const QStringList& filters);
    QStringList nameFilters() const;

    // 设置/获取浏览模式（默认 OpenFile）
    void setMode(Mode m);
    Mode mode() const;

    // 设置/获取文件对话框的起始目录（空表示由对话框决定）
    void setStartDirectory(const QString& dir);
    QString startDirectory() const;

    // 设置/获取 SaveFile 模式的默认后缀（不带点，如 "txt"）
    void setDefaultSuffix(const QString& suffix);
    QString defaultSuffix() const;

    // 获取路径
    QString getFilePath() const;
    void setFilePath(const QString& v);
    // 文本框占位提示
    void setPlaceholderText(const QString& t);

    // 拖放：默认关闭。开启后接受文件/目录 URL，落点填入并校验
    void setDropEnabled(bool on = true);
    bool isDropEnabled() const;

    // 路径校验：默认关闭。关闭时状态行隐藏、失焦不校验
    void setValidationEnabled(bool on = true);
    bool isValidationEnabled() const;
    // 立即校验并返回是否合法（空路径返回 false 且不发信号、不显示状态行）
    bool validateNow();

    // 最近路径：入参按“最近 → 最旧”，去重 + 截断到 5；非空时右侧出现“最近”按钮
    void setRecentPaths(const QStringList& paths);
    QStringList recentPaths() const;

Q_SIGNALS:
    /**
     * @brief 选中路径后会发射此信号
     * @param p 路径
     */
    void selectedPath(const QString& p);
    /**
     * @brief 文本框内容每次变化都发射（用于设置页即时标记 dirty）
     * @param p 当前文本
     */
    void pathChanged(const QString& p);
    /**
     * @brief 校验结果变化时发射；同一结果连续校验只发一次，空路径不发射
     */
    void validationChanged(bool valid);
    /**
     * @brief 最近路径被选中时发射（随后仍会发 selectedPath）
     */
    void recentPathActivated(const QString& path);

private Q_SLOTS:
    void onToolButtonOpenClicked();
    void onLineEditEditingFinished();

protected:
    void changeEvent(QEvent* e);
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragLeaveEvent(QDragLeaveEvent* e) override;
    void dropEvent(QDropEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    // 统一收尾：设文本 → （启用时）校验，浏览与拖放共用
    void setPathAndValidate(const QString& path);
    void onRecentPathActivated(const QString& path);
    void refreshValidationVisual(bool valid);
    void rebuildRecentMenu();
    void resetDragActive();

private:
    Ui::DAFilePathEditWidget* ui;
};
}

#endif  // DAFILEPATHEDITWIDGET_H
