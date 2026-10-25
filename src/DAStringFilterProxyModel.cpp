#include "DAStringFilterProxyModel.h"

namespace DA
{

/**
 * @brief 构造过滤代理
 * @param parent 父对象
 */
DAStringFilterProxyModel::DAStringFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

/**
 * @brief 析构
 */
DAStringFilterProxyModel::~DAStringFilterProxyModel()
{
}

/**
 * @brief 设置过滤文本
 * @param text 文本（空串表示不过滤）
 */
void DAStringFilterProxyModel::setFilterText(const QString& text)
{
    if (mFilterText == text) {
        return;
    }
    mFilterText = text;
    invalidateFilter();
}

/**
 * @brief 获取过滤文本
 * @return 文本
 */
QString DAStringFilterProxyModel::filterText() const
{
    return mFilterText;
}

/**
 * @brief 设置大小写敏感
 * @param cs 大小写策略
 */
void DAStringFilterProxyModel::setFilterCaseSensitivity(Qt::CaseSensitivity cs)
{
    QSortFilterProxyModel::setFilterCaseSensitivity(cs);
    invalidateFilter();
}

/**
 * @brief 获取大小写敏感
 * @return 大小写策略
 */
Qt::CaseSensitivity DAStringFilterProxyModel::filterCaseSensitivity() const
{
    return QSortFilterProxyModel::filterCaseSensitivity();
}

/**
 * @brief 设置过滤列
 * @param column 列号，-1 表示所有列
 */
void DAStringFilterProxyModel::setFilterColumn(int column)
{
    if (mFilterColumn == column) {
        return;
    }
    mFilterColumn = column;
    invalidateFilter();
}

/**
 * @brief 获取过滤列
 * @return 列号（-1 = 所有列）
 */
int DAStringFilterProxyModel::filterColumn() const
{
    return mFilterColumn;
}

/**
 * @brief 设置过滤使用的数据角色
 * @param role 数据角色（默认 Qt::DisplayRole）
 */
void DAStringFilterProxyModel::setFilterRole(int role)
{
    if (mFilterRole == role) {
        return;
    }
    mFilterRole = role;
    invalidateFilter();
}

/**
 * @brief 获取过滤数据角色
 * @return 数据角色
 */
int DAStringFilterProxyModel::filterRole() const
{
    return mFilterRole;
}

/**
 * @brief 设置子节点命中时是否放行父节点
 * @param on 是否放行
 */
void DAStringFilterProxyModel::setIncludeParentsOfMatches(bool on)
{
    if (mIncludeParents == on) {
        return;
    }
    mIncludeParents = on;
    invalidateFilter();
}

/**
 * @brief 是否放行命中子节点的父节点
 * @return true = 放行
 */
bool DAStringFilterProxyModel::includeParentsOfMatches() const
{
    return mIncludeParents;
}

/**
 * @brief 过滤判定
 * @param source_row 源行号
 * @param source_parent 源父索引
 * @return 是否保留该行
 */
bool DAStringFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (mFilterText.isEmpty()) {
        return true;
    }
    if (rowMatches(source_row, source_parent)) {
        return true;
    }
    if (!mIncludeParents) {
        return false;
    }
    // 子节点命中则放行父节点（递归，避免只检查一层）
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if (!index.isValid()) {
        return false;
    }
    const int childCount = sourceModel()->rowCount(index);
    for (int i = 0; i < childCount; ++i) {
        if (filterAcceptsRow(i, index)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief 单行自身是否命中（不含子节点）
 * @param source_row 源行号
 * @param source_parent 源父索引
 * @return 是否命中
 */
bool DAStringFilterProxyModel::rowMatches(int source_row, const QModelIndex& source_parent) const
{
    if (mFilterColumn >= 0) {
        const QModelIndex index = sourceModel()->index(source_row, mFilterColumn, source_parent);
        const QString text      = index.isValid() ? index.data(mFilterRole).toString() : QString();
        return text.contains(mFilterText, filterCaseSensitivity());
    }
    const int columns = sourceModel()->columnCount(source_parent);
    for (int c = 0; c < columns; ++c) {
        const QModelIndex index = sourceModel()->index(source_row, c, source_parent);
        if (index.isValid() && index.data(mFilterRole).toString().contains(mFilterText, filterCaseSensitivity())) {
            return true;
        }
    }
    return false;
}

}  // namespace DA
