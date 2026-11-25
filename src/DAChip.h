#ifndef DACHIP_H
#define DACHIP_H

#include <QFrame>
#include "DAWidgetsGlobals.h"

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
    // 构造带文本的标签胶囊
    explicit DAChip(const QString &text, QWidget *parent = nullptr);
    // 构造空文本的标签胶囊
    explicit DAChip(QWidget *parent = nullptr);
    // 析构，释放内部资源
    ~DAChip();

    // 获取标签显示文本
    QString text() const;
    // 设置标签显示文本
    void setText(const QString &text);

    // 是否显示关闭按钮
    bool closable() const;
    // 设置是否显示关闭按钮
    void setClosable(bool on);

    // 最大宽度（像素）。0 = 不省略（默认）；> 0 时超长文本以…结尾，全文进 tooltip，
    // text() 始终返回全文
    void setElideWidth(int px);
    // 获取最大宽度限制
    int elideWidth() const;

Q_SIGNALS:
    /**
     * @brief 关闭按钮被点击时发射
     */
    void closeRequested();
    /**
     * @brief 标签整体被点击时发射
     */
    void clicked();
    /**
     * @brief 显示文本变化时发射
     * @param text 当前文本
     */
    void textChanged(const QString &text);
    /**
     * @brief closable 属性变化时发射
     * @param closable 是否可关闭
     */
    void closableChanged(bool closable);

protected:
    void mousePressEvent(QMouseEvent *e) override;

private:
    void init();
    // 按 elideWidth 重新计算 label 显示文本与 tooltip
    void applyElide();
};
}  // namespace DA

#endif // DACHIP_H
