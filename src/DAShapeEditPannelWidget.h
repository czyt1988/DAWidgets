#ifndef DASHAPEEDITPANNELWIDGET_H
#define DASHAPEEDITPANNELWIDGET_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

namespace DA
{
/**
 * @brief 形状编辑面板
 *
 * 组合 DABrushEditWidget(背景填充)和 DAPenEditWidget(边框画笔),
 * 发射 backgroundBrushChanged / borderPenChanged 信号。
 */
class DAWIDGETS_EXPORT DAShapeEditPannelWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造形状编辑面板
    explicit DAShapeEditPannelWidget(QWidget* parent = nullptr);
    // 析构函数
    ~DAShapeEditPannelWidget();

    // 获取当前背景画刷
    QBrush getBackgroundBrush() const;
    // 获取当前边框画笔
    QPen getBorderPen() const;

public Q_SLOTS:
    // 设置背景画刷
    void setBackgroundBrush(const QBrush& b);
    // 设置边框画笔
    void setBorderPen(const QPen& v);

Q_SIGNALS:
    /**
     * @brief 背景画刷改变时发射
     * @param b 新画刷
     */
    void backgroundBrushChanged(const QBrush& b);
    /**
     * @brief 边框画笔改变时发射
     * @param p 新画笔
     */
    void borderPenChanged(const QPen& p);

private:
    class PrivateData;
    PrivateData* d;
};
}  // namespace DA

#endif  // DASHAPEEDITPANNELWIDGET_H
