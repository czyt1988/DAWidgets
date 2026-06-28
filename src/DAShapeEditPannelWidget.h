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
    explicit DAShapeEditPannelWidget(QWidget* parent = nullptr);
    ~DAShapeEditPannelWidget();

    QBrush getBackgroundBrush() const;
    QPen getBorderPen() const;

public Q_SLOTS:
    void setBackgroundBrush(const QBrush& b);
    void setBorderPen(const QPen& v);

Q_SIGNALS:
    void backgroundBrushChanged(const QBrush& b);
    void borderPenChanged(const QPen& p);

private:
    class PrivateData;
    PrivateData* d;
};
}  // namespace DA

#endif  // DASHAPEEDITPANNELWIDGET_H
