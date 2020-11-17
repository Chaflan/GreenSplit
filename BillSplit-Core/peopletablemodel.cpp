#include "peopletablemodel.h"
#include "datacoreobject.h"
#include <QDebug>

PeopleTableModel::PeopleTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int PeopleTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_data->numPeople();
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

// TODO: Make this look like transactionstablemodel setdata
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

                // TODO: Signals should be lower probably, do a signal analysis, could just connect these
                QString newIdentifier = value.toString();
                if (newIdentifier.isEmpty()) {
                    emit signalError("Attempting to change identifier to be empty.  Identifiers cannot be empty.");
                    return false;
                }
                if (m_data->PersonExists(newIdentifier)) {
                    emit signalError("Attempting to change identifier to one that already exists.  Identifiers must be unique.");
                    return false;
                }

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

    if (m_data->PersonInTransactions(m_data->GetPersonIdentifier(row))) {
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
        return columnIndexToString(section);
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

QVariant PeopleTableModel::getData(int row, int column, int role) const
{
    return data(index(row, column), role);
}

QVariant PeopleTableModel::getData(int row, const QString& roleString, int role) const
{
    return getData(row, stringToColumnIndex(roleString), role);
}

bool PeopleTableModel::setData(int row, int column, const QVariant& value, int role)
{
    return setData(index(row, column), value, role);
}

bool PeopleTableModel::setData(int row, const QString& roleString, const QVariant& value, int role)
{
    return setData(row, stringToColumnIndex(roleString), value, role);
}

bool PeopleTableModel::addPerson(QString initials, QString name)
{
    if (initials.isEmpty()) {
        emit signalError("Identifier cannot be empty.");
        return false;
    }

    if (m_data->PersonExists(initials)) {
        emit signalError("Identifier must be unique.");
        return false;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    bool result = m_data->AddPerson(std::move(initials), std::move(name));
    endInsertRows();
    return result;
}

int PeopleTableModel::columnWidth(int columnIndex, int columnSpacing, int totalWidth)
{
    switch (columnIndex) {
        case Column::Identifier: return 150;
        case Column::FullName: return totalWidth - columnSpacing * (Column::COUNT - 1) - 150;
    }

    return 0;
}

void PeopleTableModel::setDataCore(DataCoreObject* data)
{
    if (data != m_data) {
        m_data = data;
        assert(m_data);
        emit dataCoreChanged();

        QObject::connect(m_data, &DataCoreObject::modelCleared,
            this, &PeopleTableModel::resetModel);
    }
}

bool PeopleTableModel::isIndexValid(const QModelIndex& index) const
{
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount()) {
        qDebug() << "Error - PeopleTableModel::isIndexValid - index invalid: ";
        qDebug() << "\tindex.isValid() == " << index.isValid();
        if (index.isValid()) {
            qDebug() << "\trow = " << index.row() << " column = " << index.column();
        }
        return false;
    }

    return true;
}

int PeopleTableModel::stringToColumnIndex(const QString& columnRole) const
{
    const static QMap<QString, int> stringToColumn {
        { columnIndexToString(Column::Identifier), Column::Identifier },
        { columnIndexToString(Column::FullName), Column::FullName }
    };

    auto findResult = stringToColumn.find(columnRole);
    if (findResult == stringToColumn.end()) {
        qDebug() << "Error - PeopleTableModel::stringToColumnIndex - columnRole not in map: " << columnRole;
        return 0;
    }

    return findResult == stringToColumn.end() ? 0 : findResult.value();
}

QString PeopleTableModel::columnIndexToString(int columnIndex) const
{
    // TODO: Figure out how to do this instead, it doesn't compile
    //
    //static constexpr std::array<QStringView, 2> columnRoles {
    //    "Identifier",
    //    "Name"
    //};

    const static QString columnToString[Column::COUNT] {
        "Identifier",
        "Name"
    };

    if (columnIndex < 0 || columnIndex >= Column::COUNT) {
        qDebug() << "Error - PeopleTableModel::columnIndexToString - invalid columnIndex: " << columnIndex;
        return QString();
    }

    return columnToString[columnIndex];
}

void PeopleTableModel::resetModel()
{
    beginResetModel();
    endResetModel();
}
