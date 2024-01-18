// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/list.h>

#include "listmonitorlistmodel.h"

using namespace scratchcpprender;

ListMonitorListModel::ListMonitorListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void ListMonitorListModel::setList(libscratchcpp::List *list)
{
    if (!list)
        return;

    // Initial load
    if (m_list != list) {
        beginResetModel();
        m_list = list;
        m_oldRowCount = m_list->size();
        endResetModel();
        return;
    }

    // Notify about changed items
    int count = std::min(m_oldRowCount, static_cast<int>(m_list->size()));

    for (int i = 0; i < count; i++)
        emit dataChanged(index(i), index(i));

    // Notify about new items (at the end of the list)
    if (m_list->size() > m_oldRowCount) {
        beginInsertRows(QModelIndex(), m_oldRowCount, m_list->size() - 1);
        endInsertRows();
    } else if (m_list->size() < m_oldRowCount) {
        // Notify about removed items (at the end of the list)
        beginRemoveRows(QModelIndex(), m_list->size(), m_oldRowCount - 1);
        endRemoveRows();
    }

    m_oldRowCount = m_list->size();
}

int ListMonitorListModel::rowCount(const QModelIndex &parent) const
{
    if (m_list)
        return m_list->size();
    else
        return 0;
}

QVariant ListMonitorListModel::data(const QModelIndex &index, int role) const
{
    if (!m_list || index.row() < 0 || index.row() >= m_list->size())
        return QVariant();

    return QString::fromStdString((*m_list)[index.row()].toString());
}

QHash<int, QByteArray> ListMonitorListModel::roleNames() const
{
    return { { 0, "value" } };
}
