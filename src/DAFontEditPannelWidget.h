#ifndef DAFONTEDITPANNELWIDGET_H
#define DAFONTEDITPANNELWIDGET_H

#include <QWidget>
#include <QColor>
#include <QFont>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

namespace DA
{
/**
 * @brief 字体编辑面板
 *
 * 字体选择 + 字号 + 粗体/斜体/下划线 + 字体颜色。
 * 通过 currentFontChanged / currentFontColorChanged 信号通知变更。
 */
class DAWIDGETS_EXPORT DAFontEditPannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DAFontEditPannelWidget(QWidget* parent = nullptr);
    ~DAFontEditPannelWidget();

    QColor getCurrentFontColor() const;
    void setCurrentFontColor(const QColor& c);

    void setCurrentFont(const QFont& f);
    QFont getCurrentFont();

    void setFontWeight(int w);
    int getFontWeight() const;

private Q_SLOTS:
    void onComboBoxFontSizeTextChanged(const QString& t);
    void onFontSizeAdd();
    void onFontSizeSub();
    void signalEmitFontChanged();

Q_SIGNALS:
    void currentFontChanged(const QFont& f);
    void currentFontColorChanged(const QColor& c);

private:
    class PrivateData;
    PrivateData* d;
};
}  // namespace DA

#endif  // DAFONTEDITPANNELWIDGET_H
