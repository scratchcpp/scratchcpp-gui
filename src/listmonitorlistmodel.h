// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QAbstractListModel>

namespace libscratchcpp
{

class List;

}

namespace scratchcpprender
{

class ListMonitorListModel : public QAbstractListModel
{
    public:
        explicit ListMonitorListModel(QObject *parent = nullptr);

        void setList(libscratchcpp::List *list, size_t minVisibleIndex, size_t maxVisibleIndex);

        int rowCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;

    private:
        libscratchcpp::List *m_list = nullptr;
        size_t m_oldRowCount = 0;
        size_t m_minIndex = 0;
        size_t m_maxIndex = 0;
};

} // namespace scratchcpprender
