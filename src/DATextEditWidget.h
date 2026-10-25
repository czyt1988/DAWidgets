#ifndef DATEXTEDITWIDGET_H
#define DATEXTEDITWIDGET_H

#include <QString>
#include <QWidget>
#include "DAWidgetsGlobals.h"

class QAction;
class QPlainTextEdit;
class QToolBar;

namespace DA
{

/**
 * @brief 通用文本编辑/查看控件（只读、等宽、换行、工具动作）
 *
 * 内容默认做成 QWidget：不拥有 OK/Cancel/Close，需要模态时由调用方套 10 行
 * QDialog 壳（标题 + 本控件 + QDialogButtonBox）。
 *
 * 工具动作由调用方注入（如“另存为”），控件只负责展示与转发触发；
 * 不内置任何业务动作。
 */
class DAWIDGETS_EXPORT DATextEditWidget : public QWidget
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DATextEditWidget)
public:
    explicit DATextEditWidget(QWidget* parent = nullptr);
    ~DATextEditWidget();

    // 只读
    void setReadOnly(bool on);
    bool isReadOnly() const;

    // 等宽字体
    void setMonospace(bool on);
    bool isMonospace() const;

    // 自动换行
    void setLineWrapEnabled(bool on);
    bool isLineWrapEnabled() const;

    // 文本（setText 不发 textChanged）
    void setText(const QString& text);
    QString text() const;

    // 占位提示
    void setPlaceholderText(const QString& t);

    // 工具条显隐
    void setToolbarVisible(bool on);
    bool isToolbarVisible() const;
    // 注入工具动作（加入工具条后由调用方负责生命周期）
    void addToolAction(QAction* action);

Q_SIGNALS:
    /**
     * @brief 用户编辑导致文本变化（setText 不发射）
     */
    void textChanged();

private:
    Q_DISABLE_COPY(DATextEditWidget)
};
}  // namespace DA

#endif  // DATEXTEDITWIDGET_H
