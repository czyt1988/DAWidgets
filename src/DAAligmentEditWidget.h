#ifndef DAALIGMENTEDITWIDGET_H
#define DAALIGMENTEDITWIDGET_H

#include <QWidget>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"
class QAbstractButton;
namespace Ui
{
class DAAligmentEditWidget;
}
namespace DA
{
/**
 * @brief 对齐编辑窗口
 */
class DAWIDGETS_EXPORT DAAligmentEditWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造对齐编辑窗口，初始化按钮组映射
	explicit DAAligmentEditWidget(QWidget* parent = nullptr);
	// 析构，释放 ui 资源
	~DAAligmentEditWidget();
	// 设置对齐方式
	Qt::Alignment getCurrentAlignment() const;
public Q_SLOTS:
	// 设置对齐，此函数发射alignmentChanged信号
	void setCurrentAlignment(Qt::Alignment al);
Q_SIGNALS:
	/**
	 * @brief 对齐发生变化
	 * @param al
	 */
	void alignmentChanged(Qt::Alignment al);

protected:
	void changeEvent(QEvent* e);
protected Q_SLOTS:
	void onButtonGroupClicked(int id);

private:
	Ui::DAAligmentEditWidget* ui;
};
}  // end da
#endif  // DAALIGMENTEDITWIDGET_H
