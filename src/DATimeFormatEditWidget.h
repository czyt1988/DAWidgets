#ifndef DATIMEFORMATEDITWIDGET_H
#define DATIMEFORMATEDITWIDGET_H
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"
#include <QWidget>
#include <QDateTime>
class QComboBox;
namespace Ui
{
class DATimeFormatEditWidget;
}

namespace DA
{

///
/// \brief 时间设置窗口 用于设置时间的格式
///
class DAWIDGETS_EXPORT DATimeFormatEditWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造时间格式编辑窗口，预置常用格式选项
	explicit DATimeFormatEditWidget(QWidget* parent = 0);
	// 析构，释放 ui 资源
	~DATimeFormatEditWidget();
	// 获取当前选中的时间格式字符串
	QString getTimeFormat() const;
	// 设置时间格式文本，匹配下拉框中已有的选项
	void setTimeFormatText(const QString& str);
private slots:
	void onCurrentTextChanged(const QString& text);

private:
	void setComboxTimeFormat(QComboBox* comb);

private:
	Ui::DATimeFormatEditWidget* ui;
	QDateTime m_currentDateTime;
};
}  // end DA
#endif  // DATIMEFORMATEDITWIDGET_H
