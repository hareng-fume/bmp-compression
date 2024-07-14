#include "proxymodel.h"

#include "filelistmodel.h"

//-----------------------------------------------------------------------------
ProxyModel::ProxyModel(QObject *ip_parent /*=nullptr*/)
    : QSortFilterProxyModel(ip_parent)
{}

//-----------------------------------------------------------------------------
QString ProxyModel::filterPattern() const
{
    return m_filterPattern;
}

//-----------------------------------------------------------------------------
void ProxyModel::setFilterPattern(const QString &i_pattern)
{
    if (m_filterPattern != i_pattern) {
        m_filterPattern = i_pattern;
        emit filterPatternChanged();
        invalidateFilter();
    }
}

//-----------------------------------------------------------------------------
void ProxyModel::handleItemClick(int i_index) const
{
    const auto proxyIndex = index(i_index, 0);
    const auto sourceIndex = mapToSource(proxyIndex);

    auto *p_sourceModel = qobject_cast<FileListModel *>(sourceModel());
    p_sourceModel->processItem(sourceIndex);
}

//-----------------------------------------------------------------------------
bool ProxyModel::canHandleItem(int i_index) const
{
    const auto proxyIndex = index(i_index, 0);
    const auto sourceIndex = mapToSource(proxyIndex);

    auto *p_sourceModel = qobject_cast<FileListModel *>(sourceModel());
    return p_sourceModel->canHandleItem(sourceIndex);
}

//-----------------------------------------------------------------------------
bool ProxyModel::filterAcceptsRow(int i_sourceRow, const QModelIndex &i_sourceParent) const
{
    if (m_filterPattern.isEmpty()) // show all items
        return true;

    const auto index = sourceModel()->index(i_sourceRow, 0, i_sourceParent);
    const auto extension = sourceModel()->data(index, FileListModel::ExtensionRole).toString();
    return extension == m_filterPattern;
}
