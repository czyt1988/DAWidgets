#include "DAAligmentPositionEditWidget.h"
#include "ui_DAAligmentPositionEditWidget.h"
namespace DA
{
/**
 * @brief 构造对齐位置编辑窗口
 * @param parent 父控件
 * @details 加载 ui 并将九宫格按钮的 buttonGroup id 绑定到对应的方向组合对齐值，
 *          同时按 Qt 版本连接 buttonClicked/idClicked 信号
 */
DAAligmentPositionEditWidget::DAAligmentPositionEditWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::DAAligmentPositionEditWidget)
{
	ui->setupUi(this);
	ui->buttonGroup->setId(ui->toolButtonTopLeft, static_cast< int >(Qt::AlignLeft | Qt::AlignTop));
	ui->buttonGroup->setId(ui->toolButtonTop, static_cast< int >(Qt::AlignTop | Qt::AlignHCenter));
	ui->buttonGroup->setId(ui->toolButtonTopRight, static_cast< int >(Qt::AlignTop | Qt::AlignRight));
	ui->buttonGroup->setId(ui->toolButtonLeft, static_cast< int >(Qt::AlignLeft | Qt::AlignVCenter));
	ui->buttonGroup->setId(ui->toolButtonCenter, static_cast< int >(Qt::AlignCenter));
	ui->buttonGroup->setId(ui->toolButtonRight, static_cast< int >(Qt::AlignRight | Qt::AlignVCenter));
	ui->buttonGroup->setId(ui->toolButtonBottomLeft, static_cast< int >(Qt::AlignLeft | Qt::AlignBottom));
	ui->buttonGroup->setId(ui->toolButtonBottom, static_cast< int >(Qt::AlignBottom | Qt::AlignHCenter));
	ui->buttonGroup->setId(ui->toolButtonBottomRight, static_cast< int >(Qt::AlignBottom | Qt::AlignRight));
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
	connect(ui->buttonGroup,
			QOverload< int >::of(&QButtonGroup::buttonClicked),
			this,
			&DAAligmentPositionEditWidget::onButtonClicked);
#else
	connect(ui->buttonGroup, &QButtonGroup::idClicked, this, &DAAligmentPositionEditWidget::onButtonClicked);
#endif
}

/**
 * @brief 析构函数
 * @details 释放 ui 指针
 */
DAAligmentPositionEditWidget::~DAAligmentPositionEditWidget()
{
	delete ui;
}

/**
 * @brief 设置对齐位置
 * @param al 要设置的对齐方式
 * @details 先通过 acceptAligment 过滤为本控件支持的方向组合，再选中对应按钮并发射信号
 */
void DAAligmentPositionEditWidget::setAligmentPosition(Qt::Alignment al)
{
	Qt::Alignment realal = acceptAligment(al);
	if (realal == getAligmentPosition()) {
		return;
	}
	auto button = ui->buttonGroup->button(static_cast< int >(realal));
	if (button) {
		if (!button->isChecked()) {
			button->setChecked(true);
			emit aligmentPositionChanged(realal);
		}
	}
}

/**
 * @brief 获取当前对齐位置
 * @return 当前选中的对齐方式
 * @details 直接返回按钮组当前选中项的 id 转换而来的对齐值
 */
Qt::Alignment DAAligmentPositionEditWidget::getAligmentPosition() const
{
	return static_cast< Qt::Alignment >(ui->buttonGroup->checkedId());
}

/**
 * @brief 过滤对齐方式为本控件支持的方向组合
 * @param al 输入的任意对齐方式
 * @return 映射到九宫格按钮支持的组合对齐值，无匹配时返回默认构造的空对齐
 * @details 九宫格仅支持 9 种固定方向组合，此函数把任意对齐值归一化到这些组合之一
 */
Qt::Alignment DAAligmentPositionEditWidget::acceptAligment(Qt::Alignment al)
{
	Qt::Alignment realal;
	if (al.testFlag(Qt::AlignLeft) && al.testFlag(Qt::AlignTop)) {
		realal = Qt::AlignLeft | Qt::AlignTop;
	} else if (al.testFlag(Qt::AlignTop) && al.testFlag(Qt::AlignHCenter)) {
		realal = Qt::AlignTop | Qt::AlignHCenter;
	} else if (al.testFlag(Qt::AlignTop) && al.testFlag(Qt::AlignRight)) {
		realal = Qt::AlignTop | Qt::AlignRight;
	} else if (al.testFlag(Qt::AlignLeft) && al.testFlag(Qt::AlignVCenter)) {
		realal = Qt::AlignLeft | Qt::AlignVCenter;
	} else if (al.testFlag(Qt::AlignCenter)) {
		realal = Qt::AlignCenter;
	} else if (al.testFlag(Qt::AlignRight) && al.testFlag(Qt::AlignVCenter)) {
		realal = Qt::AlignRight | Qt::AlignVCenter;
	} else if (al.testFlag(Qt::AlignLeft) && al.testFlag(Qt::AlignBottom)) {
		realal = Qt::AlignLeft | Qt::AlignBottom;
	} else if (al.testFlag(Qt::AlignBottom) && al.testFlag(Qt::AlignHCenter)) {
		realal = Qt::AlignBottom | Qt::AlignHCenter;
	} else if (al.testFlag(Qt::AlignRight) && al.testFlag(Qt::AlignBottom)) {
		realal = Qt::AlignRight | Qt::AlignBottom;
	}
	return realal;
}

void DAAligmentPositionEditWidget::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DAAligmentPositionEditWidget::onButtonClicked(int id)
{
	emit aligmentPositionChanged(static_cast< Qt::Alignment >(id));
}
}
