#ifndef DACHECKLISTWIDGET_H
#define DACHECKLISTWIDGET_H

#include <QWidget>
#include <QVector>
#include "DAItemListEditWidget.h"
#include "DAWidgetsGlobals.h"

namespace DA
{

/**
 * @brief 通用勾选列表（搜索 + 勾选 + 全选/反选）
 *
 * 面向「从候选里挑选若干条」的平铺列表场景（合并列、分组键、列可见性、
 * 子 Agent 工具白名单等）。控件只认 DAItemEntry（text + data），不绑定业务语义。
 *
 * 勾选语义（调用方必须按此理解）：
 * 1. forced-checked 条目永远出现在 checkedEntries()，用户不能取消，全选/反选都不动它；
 * 2. 全选只作用于「当前可见 + 可勾选 + 非 forced + 可用」的条目，反选同理；
 * 3. 搜索只隐藏行，不改勾选状态；被过滤掉的已勾选条目仍然出现在 checkedEntries() 里；
 * 4. setEntryEnabled(false) 且非 forced 的条目不能被勾选；若已勾选，控件保留状态，
 *    是否清理由调用方用 forced 语义表达，控件不猜；
 * 5. setEntryCheckable(false) 的条目没有复选框、按未勾选处理，checkedEntries() 不含它；
 * 6. setItems/setCheckedEntries 整体替换，不发 checkedEntriesChanged；用户操作才发。
 */
class DAWIDGETS_EXPORT DACheckListWidget : public QWidget
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DACheckListWidget)
public:
    explicit DACheckListWidget(QWidget* parent = nullptr);
    ~DACheckListWidget();

    // 设置候选条目（全部未勾选、可用、可勾选、非 forced；不发信号）
    void setItems(const QVector< DAItemEntry >& items);
    // 获取全部候选条目（含过滤掉的、禁用的）
    QVector< DAItemEntry > items() const;
    // 预勾选若干条目（按 DAItemEntry 相等匹配，forced 条目始终勾选；不发信号）
    void setCheckedEntries(const QVector< DAItemEntry >& entries);
    // 获取勾中的条目（列表顺序；含被过滤掉/禁用的；含 forced）
    QVector< DAItemEntry > checkedEntries() const;

    // 条目可用性（不可用的条目不能勾选）
    void setEntryEnabled(const DAItemEntry& e, bool enabled);
    // 条目是否有复选框（false 时按未勾选处理）
    void setEntryCheckable(const DAItemEntry& e, bool checkable);
    // 强制勾选：永远勾选且用户不能取消，全选/反选不动它
    void setEntryForcedChecked(const DAItemEntry& e, bool on);

    // 搜索框显隐与占位文本
    void setSearchVisible(bool on);
    void setSearchPlaceholderText(const QString& t);
    // 全选/反选按钮显隐
    void setSelectAllVisible(bool on);
    void setInvertVisible(bool on);

    // 条目总数 / 勾选总数
    int itemCount() const;
    int checkedCount() const;
Q_SIGNALS:
    /**
     * @brief 用户操作导致勾选变化（全选/反选合并为一次）
     */
    void checkedEntriesChanged();

private:
    Q_DISABLE_COPY(DACheckListWidget)
};
}  // namespace DA

#endif  // DACHECKLISTWIDGET_H
