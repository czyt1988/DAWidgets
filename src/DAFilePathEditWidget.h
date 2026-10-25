#ifndef DAFILEPATHEDITWIDGET_H
#define DAFILEPATHEDITWIDGET_H

#include <QWidget>
#include "DAWidgetsGlobals.h"
namespace Ui
{
class DAFilePathEditWidget;
}

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
private Q_SLOTS:
    void onToolButtonOpenClicked();
    void onLineEditEditingFinished();

protected:
    void changeEvent(QEvent* e);

private:
    Ui::DAFilePathEditWidget* ui;
};
}

#endif  // DAFILEPATHEDITWIDGET_H
