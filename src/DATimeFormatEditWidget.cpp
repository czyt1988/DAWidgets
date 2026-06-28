#include "DATimeFormatEditWidget.h"
#include "ui_DATimeFormatEditWidget.h"
#include <QComboBox>
namespace DA
{

/**
 * @brief 构造时间格式编辑窗口
 * @param parent 父控件
 * @details 加载 ui，填充常用时间格式选项到下拉框并连接文本变化信号，
 *          默认选中最后一个（最完整的日期时间）格式
 */
DATimeFormatEditWidget::DATimeFormatEditWidget(QWidget* parent)
	: QWidget(parent), ui(new Ui::DATimeFormatEditWidget), m_currentDateTime(QDateTime::currentDateTime())
{
	ui->setupUi(this);
	setComboxTimeFormat(ui->comboBox);
	connect(ui->comboBox, &QComboBox::currentTextChanged, this, &DATimeFormatEditWidget::onCurrentTextChanged);
	ui->comboBox->setCurrentIndex(ui->comboBox->count() - 1);
}

/**
 * @brief 析构函数
 * @details 释放 ui 指针
 */
DATimeFormatEditWidget::~DATimeFormatEditWidget()
{
	delete ui;
}

/**
 * @brief 获取当前时间格式
 * @return 下拉框当前选中的时间格式字符串
 */
QString DATimeFormatEditWidget::getTimeFormat() const
{
	return ui->comboBox->currentText();
}

/**
 * @brief 设置时间格式文本
 * @param str 时间格式字符串
 * @details 设置下拉框的当前文本，若匹配已有项则选中对应项
 */
void DATimeFormatEditWidget::setTimeFormatText(const QString& str)
{
	ui->comboBox->setCurrentText(str);
}

void DATimeFormatEditWidget::onCurrentTextChanged(const QString& text)
{
	ui->labelExample->setText(m_currentDateTime.toString(text));
}

void DATimeFormatEditWidget::setComboxTimeFormat(QComboBox* comb)
{
	comb->clear();
	comb->addItem(QString("h:m"), QVariant(QString("h:m")));
	comb->setItemData(comb->count() - 1, tr("example: 8:5"), Qt::ToolTipRole);

	comb->addItem(QString("hh:mm"), QVariant(QString("hh:mm")));
	comb->setItemData(comb->count() - 1, tr("example: 08:05"), Qt::ToolTipRole);

	comb->addItem(QString("h:m:s"), QVariant(QString("h:m:s")));
	comb->setItemData(comb->count() - 1, tr("example: 8:5:12"), Qt::ToolTipRole);

	comb->addItem(QString("hh:mm:ss"), QVariant(QString("hh:mm:ss")));
	comb->setItemData(comb->count() - 1, tr("example: 08:05:12"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-M-d"), QVariant(QString("yyyy-M-d")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-8-1"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-M-d h:m"), QVariant(QString("yyyy-M-d h:m")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-8-1 8:5"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-M-d h:m:s"), QVariant(QString("yyyy-M-d h:m:s")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-8-1 8:5:12"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-MM-dd"), QVariant(QString("yyyy-MM-dd")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-08-01"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-MM-dd hh:mm"), QVariant(QString("yyyy-MM-dd hh:mm")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-08-01 08:05"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-MM-dd hh:mm:ss"), QVariant(QString("yyyy-MM-dd hh:mm:ss")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-08-01 08:05:12"), Qt::ToolTipRole);
}

}  // end DA
