#ifndef DAITEMLISTPICKDIALOG_H
#define DAITEMLISTPICKDIALOG_H

#include <QDialog>
#include <QVector>
#include "DAItemListEditWidget.h"

namespace DA
{

/**
 * @brief 通用条目勾选对话框（搜索 + 多选一次性添加）
 *
 * 配合 DAItemListEditWidget 的添加按钮使用：列出候选条目（复选框列表），
 * 顶部搜索框按展示文本子串过滤（大小写不敏感，过滤不丢失勾选状态），
 * 确定后由 checkedEntries() 取回勾中的条目。
 */
class DAWIDGETS_EXPORT DAItemListPickDialog : public QDialog
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DAItemListPickDialog)
public:
    explicit DAItemListPickDialog(QWidget* parent = nullptr);
    ~DAItemListPickDialog();

    // 设置候选条目（全部未勾选）
    void setItems(const QVector< DAItemEntry >& items);
    // 预勾选若干条目（按 DAItemEntry 相等匹配）
    void setCheckedEntries(const QVector< DAItemEntry >& entries);
    // 取回勾中的条目（列表顺序）
    QVector< DAItemEntry > checkedEntries() const;

    // 覆盖默认文案（集成侧 i18n 用）
    void setSearchPlaceholderText(const QString& t);

private:
    Q_DISABLE_COPY(DAItemListPickDialog)
};
}  // namespace DA

#endif  // DAITEMLISTPICKDIALOG_H
