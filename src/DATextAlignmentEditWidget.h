#ifndef DATEXTALIGNMENTEDITWIDGET_H
#define DATEXTALIGNMENTEDITWIDGET_H

#include <QWidget>
#include "DAWidgetsGlobals.h"

namespace DA
{
/**
 * @brief 文本对齐编辑控件：水平一组（左/居中/右）+ 垂直一组（顶端/居中/底端）
 *
 * 两行互斥按钮组，各自独立选择；值以 Qt::Alignment 的水平位 + 垂直位组合表示。
 * 通用控件：只认 Qt::Alignment，不含业务语义（表格单元格、图表文本、标签排版均可复用）。
 * 与 DAAligmentEditWidget 的区别：后者是单选语义（水平垂直共用一个按钮组），
 * 本控件把水平与垂直拆成两组，可分别设置（如「右对齐 + 顶端对齐」）。
 */
class DAWIDGETS_EXPORT DATextAlignmentEditWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
public:
    explicit DATextAlignmentEditWidget(QWidget* parent = nullptr);
    ~DATextAlignmentEditWidget() override;

    // 当前对齐（水平位 AlignLeft/AlignHCenter/AlignRight + 垂直位 AlignTop/AlignVCenter/AlignBottom）
    Qt::Alignment alignment() const;
    // 当前水平对齐位
    Qt::Alignment horizontalAlignment() const;
    // 当前垂直对齐位
    Qt::Alignment verticalAlignment() const;

public Q_SLOTS:
    // 设置对齐：缺水平位/垂直位的那一半保持当前值；与当前值相同不发信号
    void setAlignment(Qt::Alignment al);

Q_SIGNALS:
    // 对齐发生变化（用户点击或 setAlignment 改变值时发出）
    void alignmentChanged(Qt::Alignment al);

protected:
    void changeEvent(QEvent* e) override;

private:
    DA_DECLARE_PRIVATE(DATextAlignmentEditWidget)
};
}  // namespace DA
#endif  // DATEXTALIGNMENTEDITWIDGET_H
