#ifndef DAPENEDITWIDGET_H
#define DAPENEDITWIDGET_H
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"
#include <QWidget>
#include <QPen>

namespace DA
{
/**
 * @brief 画笔编辑窗口
 * 颜色按钮+线型选择+线宽
 * ┏┓┍━━━━┓┍━━┓
 * ┗┛┗━━━━┛┗━━┛
 */
class DAWIDGETS_EXPORT DAPenEditWidget : public QWidget
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAPenEditWidget)
public:
	// 构造函数
	explicit DAPenEditWidget(QWidget* parent = nullptr);
	// 构造并设置初始画笔
	explicit DAPenEditWidget(const QPen& p, QWidget* parent = nullptr);
	// 析构函数
	~DAPenEditWidget();
	// 重新翻译UI文本（多语言支持）
	void retranslateUi();
	// 获取当前的画笔
	QPen getCurrentPen() const;
	// 是否在样式上显示文字
	void setLineStyleTextVisible(bool on);
	// 返回是否在样式上显示文字
	bool isLineStyleTextVisible() const;
public slots:
	// 设置画笔,设置画笔会触发penChanged信号
	void setCurrentPen(const QPen& p);
signals:
	/**
	 * @brief 画笔改变信号
	 * @param p
	 */
	void penChanged(const QPen& p);

protected slots:
	void onColorChanged(const QColor& c);
	void onPenWidthValueChanged(double w);
	void onPenStyleChanged(Qt::PenStyle s);

private:
	void initConnect();
};
}  // namespace DA
#endif  // DAPENEDITWIDGET_H
