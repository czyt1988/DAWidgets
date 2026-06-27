#ifndef DACHIP_H
#define DACHIP_H

#include <QFrame>
#include "DAWidgetsGlobals.h"
#include "DAWIDGETS_EXPORT.h"

class QLabel;
class QToolButton;

namespace DA
{
/**
 * @brief 单个标签胶囊控件，显示文本 + 可选关闭按钮。
 *
 * 独立可用，亦作为 DAChipsEdit 的子项。可通过 qss 定制：
 *
 * ```css
 * DA--DAChip { background: #ecf5ff; border:1px solid #d9ecff;
 *          border-radius:4px; }
 * DA--DAChip:hover { background: #d9ecff; }
 * QToolButton#DAChipCloseButton { border:none; color:#909399; }
 * QToolButton#DAChipCloseButton:hover { color:#f56c6c; }
 * ```
 */
class DAWIDGETS_EXPORT DAChip : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool closable READ closable WRITE setClosable NOTIFY closableChanged)
    DA_DECLARE_PRIVATE(DAChip)
public:
    explicit DAChip(const QString &text, QWidget *parent = nullptr);
    explicit DAChip(QWidget *parent = nullptr);
    ~DAChip();

    QString text() const;
    void setText(const QString &text);

    bool closable() const;
    void setClosable(bool on);

Q_SIGNALS:
    void closeRequested();
    void clicked();
    void textChanged(const QString &text);
    void closableChanged(bool closable);

protected:
    void mousePressEvent(QMouseEvent *e) override;

private:
    void init();
};
}  // namespace DA

#endif // DACHIP_H
