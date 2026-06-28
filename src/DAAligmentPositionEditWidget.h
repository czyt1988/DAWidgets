#ifndef DAALIGMENTPOSITIONEDITWIDGET_H
#define DAALIGMENTPOSITIONEDITWIDGET_H

#include <QWidget>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"
namespace Ui
{
class DAAligmentPositionEditWidget;
}
namespace DA
{
/**
 * @brief The DAAligmentPositionEditWidget class
 */
class DAWIDGETS_EXPORT DAAligmentPositionEditWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造对齐位置编辑窗口，初始化九宫格按钮组映射
	explicit DAAligmentPositionEditWidget(QWidget* parent = nullptr);
	// 析构，释放 ui 资源
	~DAAligmentPositionEditWidget();
	// 设置aligment
	Qt::Alignment getAligmentPosition() const;
    // 把输入的al进行过滤，得到本控件支持的组合
    static Qt::Alignment acceptAligment(Qt::Alignment al);
public Q_SLOTS:
	// 设置对齐位置，此函数会发射 aligmentPositionChanged 信号
	void setAligmentPosition(Qt::Alignment al);
Q_SIGNALS:
	/**
	 * @brief 对齐位置变化时发射
	 * @param al 变化后的对齐位置
	 */
	void aligmentPositionChanged(Qt::Alignment al);

protected:
	void changeEvent(QEvent* e);
private Q_SLOTS:
    void onButtonClicked(int id);

private:
	Ui::DAAligmentPositionEditWidget* ui;
};
}  // end DA
#endif  // DAALIGMENTPOSITIONEDITWIDGET_H
