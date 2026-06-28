#include "DAShapeEditPannelWidget.h"
#include "ui_DAShapeEditPannelWidget.h"

#include "DABrushEditWidget.h"
#include "DAPenEditWidget.h"

namespace DA
{
class DAShapeEditPannelWidget::PrivateData
{
public:
    Ui::DAShapeEditPannelWidget ui;
};

/**
 * @brief 构造形状编辑面板
 * @param parent 父控件
 * @details 组合DABrushEditWidget和DAPenEditWidget，连接各自的变更信号
 */
DAShapeEditPannelWidget::DAShapeEditPannelWidget(QWidget* parent)
    : QWidget(parent), d(new PrivateData)
{
    d->ui.setupUi(this);
    connect(d->ui.widgetPenEdit, &DAPenEditWidget::penChanged, this, &DAShapeEditPannelWidget::borderPenChanged);
    connect(d->ui.widgetBrushEdit, &DABrushEditWidget::brushChanged, this, &DAShapeEditPannelWidget::backgroundBrushChanged);
}

/**
 * @brief 析构函数
 */
DAShapeEditPannelWidget::~DAShapeEditPannelWidget()
{
    delete d;
}

/**
 * @brief 获取当前背景画刷
 * @return 背景画刷
 */
QBrush DAShapeEditPannelWidget::getBackgroundBrush() const
{
    return d->ui.widgetBrushEdit->getCurrentBrush();
}

/**
 * @brief 获取当前边框画笔
 * @return 边框画笔
 */
QPen DAShapeEditPannelWidget::getBorderPen() const
{
    return d->ui.widgetPenEdit->getCurrentPen();
}

/**
 * @brief 设置背景画刷
 * @param b 新画刷
 */
void DAShapeEditPannelWidget::setBackgroundBrush(const QBrush& b)
{
    if (b == getBackgroundBrush()) {
        return;
    }
    d->ui.widgetBrushEdit->setCurrentBrush(b);
}

/**
 * @brief 设置边框画笔
 * @param v 新画笔
 */
void DAShapeEditPannelWidget::setBorderPen(const QPen& v)
{
    d->ui.widgetPenEdit->setCurrentPen(v);
}
}  // namespace DA
