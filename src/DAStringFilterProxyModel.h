#ifndef DASTRINGFILTERPROXYMODEL_H
#define DASTRINGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QPair>
#include <QString>
#include <QVector>
#include "DAWidgetsGlobals.h"

namespace DA
{

/**
 * @brief 按展示文本过滤的通用代理模型（列表/树通用）
 *
 * - 过滤目标：行的 Qt::DisplayRole 文本；可限定单列，默认 -1 表示所有列都参与匹配；
 * - 大小写：默认不敏感，可切换；
 * - 树形模型：setIncludeParentsOfMatches(true) 让命中子节点的父节点也放行，
 *   否则父节点会被单独过滤掉、子节点无法显示；
 * - 空过滤文本表示不过滤（全部放行）。
 */
class DAWIDGETS_EXPORT DAStringFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DAStringFilterProxyModel(QObject* parent = nullptr);
    ~DAStringFilterProxyModel();

    // 过滤文本（空串 = 不过滤）
    void setFilterText(const QString& text);
    QString filterText() const;

    // 大小写敏感（默认不敏感）
    void setFilterCaseSensitivity(Qt::CaseSensitivity cs);
    Qt::CaseSensitivity filterCaseSensitivity() const;

    // 过滤列（-1 = 所有列）
    void setFilterColumn(int column);
    int filterColumn() const;

    // 过滤使用的数据角色（默认 Qt::DisplayRole）
    void setFilterRole(int role);
    int filterRole() const;

    // 子节点命中时是否放行父节点（树形模型用，默认 false）
    void setIncludeParentsOfMatches(bool on);
    bool includeParentsOfMatches() const;

    // 单段文本的匹配语义（空过滤恒为 true）；供不能挂代理的视图复用同一实现
    static bool matchesText(const QString& text, const QString& filter, Qt::CaseSensitivity cs);

    // 单个单元格的命中区间：按当前过滤设置计算，元素为 (start, length)，UTF-16 下标。
    // 空过滤 / 该列不参与匹配 / 索引无效 / 索引不属于本代理 → 返回空数组。
    // 区间从左到右、互不重叠；父节点仅因子节点命中被放行时，其自身区间为空。
    QVector< QPair< int, int > > matchRanges(const QModelIndex& proxyIndex) const;

    // 纯文本版本：与 matchesText() 同一套语义。
    // 注意：空过滤返回空数组，而 matchesText() 对空过滤返回 true（放行 ≠ 高亮）。
    static QVector< QPair< int, int > > matchRangesIn(const QString& text,
                                                      const QString& filter,
                                                      Qt::CaseSensitivity cs);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
    // 单行自身是否命中（不含子节点）
    bool rowMatches(int source_row, const QModelIndex& source_parent) const;

    QString mFilterText;
    int mFilterColumn { -1 };
    int mFilterRole { Qt::DisplayRole };
    bool mIncludeParents { false };
};

}  // namespace DA

#endif  // DASTRINGFILTERPROXYMODEL_H
