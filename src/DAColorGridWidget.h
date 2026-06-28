#ifndef DACOLORGRIDWIDGET_H
#define DACOLORGRIDWIDGET_H

#include <QWidget>
#include <QList>
#include <QColor>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QToolButton;
class QGridLayout;
class QButtonGroup;
class QSpacerItem;

namespace DA
{
/**
 * @brief 基础颜色网格控件。
 *
 * 使用 QGridLayout + QButtonGroup(exclusive) 排列一组颜色色块按钮,
 * 每个色块为 QToolButton(IconOnly),颜色通过 QPixmap icon 显示。
 *
 * 是 DAColorPaletteGridWidget 和 DAColorMenu 的基础构件。
 */
class DAWIDGETS_EXPORT DAColorGridWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    DA_DECLARE_PRIVATE(DAColorGridWidget)
public:
    using FunColorBtn = std::function<void(QToolButton*)>;

    explicit DAColorGridWidget(QWidget* par = nullptr);
    ~DAColorGridWidget();

    void setColumnCount(int c);
    int columnCount() const;

    void setColorList(const QList<QColor>& cls);
    QList<QColor> getColorList() const;

    int spacing() const;
    void setSpacing(int v);

    int colorCount() const;

    void setColorIconSize(const QSize& s);
    QSize colorIconSize() const;

    void setColorCheckable(bool on = true);
    bool isColorCheckable() const;

    QColor currentCheckedColor() const;
    QToolButton* colorButton(int index) const;

    void setVerticalSpacing(int v);
    int verticalSpacing() const;
    void setHorizontalSpacing(int v);
    int horizontalSpacing() const;

    void clearCheckedState();

    void iterationColorBtns(FunColorBtn fn);

    void setRowMinimumHeight(int row, int minSize);
    void setHorizontalSpacerToRight(bool on = true);

    QSize sizeHint() const override;

Q_SIGNALS:
    void colorClicked(const QColor& c);
    void colorPressed(const QColor& c);
    void colorReleased(const QColor& c);
    void colorToggled(const QColor& c, bool on);

private:
    void updateGridColor(bool isRemoveSpacer = false);
    void setColorAt(const QColor& clr, int r, int c);
    static QIcon makeColorIcon(const QColor& clr, const QSize& size);
};
}  // namespace DA

#endif  // DACOLORGRIDWIDGET_H
