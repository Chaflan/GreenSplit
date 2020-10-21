#include "peopletablemodel.h"
#include "datacoreobject.h"



PeopleTableModel::PeopleTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int PeopleTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_data->NumPeople();
}

int PeopleTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : Column::COUNT;
}

QVariant PeopleTableModel::data(const QModelIndex& index, int role) const
{
    if (isIndexValid(index) && role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case Column::Identifier: return m_data->GetPersonIdentifier(index.row());
            case Column::FullName: return m_data->GetPersonName(index.row());
        }
    }

    return QVariant();
}

bool PeopleTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!isIndexValid(index)) {
        return false;
    }

    if (role != Qt::EditRole) {
        return QAbstractTableModel::setData(index, value, role);
    }

    switch(index.column()) {
        case Column::Identifier: {
            if (!m_data->EditPersonIdentifier(index.row(), value.toString())) {
                // TODO: Signal should come from datacore and propagate up, try that
                emit signalError("Invalid index in people table model");
                return false;
            }
            break;
        }
        case Column::FullName: {
            m_data->EditPersonName(index.row(), value.toString());
            break;
        }
    }

    emit dataChanged(index, index);
    return true;
}

bool PeopleTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid() || row < 0 || count < 1) {
        return false;
    }

    if (m_data->PersonExists(m_data->GetPersonIdentifier(row))) {
        // TODO: Signal should come from datacore and propagate up, try that
        emit signalError("Person is involved in one or more transactions, cannot remove them");
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    bool result = m_data->RemovePeople(row, count);
    endRemoveRows();
    return result;
}

QVariant PeopleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case Column::Identifier: return "Identifier";
            case Column::FullName: return "Name";
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

// Override flags to indicate that table elements are editable
Qt::ItemFlags PeopleTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool PeopleTableModel::addPerson(QString initials, QString name)
{
    if (initials.isEmpty()) {
        emit signalError("Initials cannot be empty.");
        return false;
    }

    if (m_data->PersonExists(initials)) {
        emit signalError("Initials must be unique.");
        return false;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    bool result = m_data->AddPerson(std::move(initials), std::move(name));
    endInsertRows();
    return result;
}

int PeopleTableModel::columnWidth(int col)
{
    // TODO: Implment; this is a stub
    Q_UNUSED(col);
    return 50;
}

bool PeopleTableModel::isIndexValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}
