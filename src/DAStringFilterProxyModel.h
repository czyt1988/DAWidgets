#ifndef DASTRINGFILTERPROXYMODEL_H
#define DASTRINGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>
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

    // 子节点命中时是否放行父节点（树形模型用，默认 false）
    void setIncludeParentsOfMatches(bool on);
    bool includeParentsOfMatches() const;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
    // 单行自身是否命中（不含子节点）
    bool rowMatches(int source_row, const QModelIndex& source_parent) const;

    QString mFilterText;
    int mFilterColumn { -1 };
    bool mIncludeParents { false };
};

}  // namespace DA

#endif  // DASTRINGFILTERPROXYMODEL_H
