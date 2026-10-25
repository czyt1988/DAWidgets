#ifndef DAITEMLISTEDITWIDGET_H
#define DAITEMLISTEDITWIDGET_H

#include <QWidget>
#include <QString>
#include <QVariant>
#include <QVector>
#include "DAWidgetsGlobals.h"

namespace DA
{

/**
 * @brief 通用候选/选中条目（text 展示、data 标识）
 *
 * data 无效时按 text 判同；调用方一般用 data 携带位置索引等真实标识，
 * 避免同名条目（如重名列）混淆。
 */
struct DAWIDGETS_EXPORT DAItemEntry
{
    QString text;    ///< 展示文本
    QVariant data;   ///< 标识数据（无效则退化为按 text 判同）
    bool operator==(const DAItemEntry& o) const;
    bool operator!=(const DAItemEntry& o) const;
};

/**
 * @brief 通用条目选择编辑控件（选中之列表 + 添加 + 移除所有）
 *
 * 面向「从一组候选中挑选若干条」的场景（典型如表格列选择）：
 * - 列表展示已选条目，每行右端一个删除按钮，单击即移除该条目；
 * - 添加按钮为带菜单的 QToolButton（MenuButtonPopup）：点箭头下拉候选菜单
 *   （候选 = availableItems 中尚未选中者，可由调用方设定哪些条目出现），
 *   单击按钮主体弹出 DAItemListPickDialog（可搜索、可多选一次性添加）；
 * - 「移除所有」按钮清空列表，带二次确认。
 *
 * 控件只认 DAItemEntry（text + data），不绑定任何业务语义，列选择/字段选择等
 * 场景均可复用；条目变化统一发 selectedItemsChanged()。
 */
class DAWIDGETS_EXPORT DAItemListEditWidget : public QWidget
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DAItemListEditWidget)
public:
    explicit DAItemListEditWidget(QWidget* parent = nullptr);
    ~DAItemListEditWidget();

    // 设置候选条目（添加菜单与添加对话框的数据源；通常一次性给全量）
    void setAvailableItems(const QVector< DAItemEntry >& items);
    // 获取候选条目
    QVector< DAItemEntry > availableItems() const;

    // 设置已选条目（整体替换，不发 selectedItemsChanged）
    void setSelectedItems(const QVector< DAItemEntry >& items);
    // 获取已选条目（列表顺序 = 添加顺序）
    QVector< DAItemEntry > selectedItems() const;

    // 显示 Up/Down 按钮（按钮移动当前行，不使用拖拽；默认关闭）
    void setReorderEnabled(bool on);
    bool isReorderEnabled() const;

    // 覆盖默认文案（集成侧 i18n 用；默认为英文 tr 文本）
    void setAddButtonText(const QString& t);
    void setRemoveAllButtonText(const QString& t);
    // 覆盖「移除所有」二次确认的标题与正文
    void setRemoveAllConfirmText(const QString& title, const QString& text);

Q_SIGNALS:
    /**
     * @brief 已选条目变化（增/删/清空均发射；setSelectedItems 不发射）
     */
    void selectedItemsChanged();

private:
    Q_DISABLE_COPY(DAItemListEditWidget)
};
}  // namespace DA

#endif  // DAITEMLISTEDITWIDGET_H
