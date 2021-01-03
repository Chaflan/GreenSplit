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
            case Column::Identifier: return m_data->getPersonIdentifier(index.row());
            case Column::Name: return m_data->getPersonName(index.row());
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

    bool successfulEdit = false;
    switch(index.column()) {
        case Column::Identifier: successfulEdit = m_data->editPersonIdentifier(index.row(), value.toString()); break;
        case Column::Name: successfulEdit = m_data->editPersonName(index.row(), value.toString()); break;
    }

    if (successfulEdit) {
        emit dataChanged(index, index);
    }

    return successfulEdit;
}

bool PeopleTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid()) {
        qDebug() << "Error - PeopleTableModel::removeRows - Valid parent in non-tree";
        return false;
    }

    // We can't use begin/endRemoveRows here becuase Qt still removes them from the view
    // even if removePeople fails.
    beginResetModel();
    bool result = m_data->removePeople(row, count);
    endResetModel();

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
    // Can't use begin/endInsertRows here. Qt still adds to view
    // even if addPerson fails.
    beginResetModel();
    bool result = m_data->addPerson(std::move(initials), std::move(name));
    endResetModel();
    return result;
}

bool PeopleTableModel::addDefaultPerson()
{
    // Constexpr this
    const QVector<std::tuple<QString, QString> > defaults{
        { "C", "Charlie" },
        { "L", "Lucas" },
        { "M", "Megan"},
        { "J", "Javier"},
        { "X", "Xerxes"}
    };

    QString nextId;
    QString nextName;

    do {
        nextId.clear();
        nextName.clear();

        if (m_defaultNameIndex < defaults.size()) {
            nextId = std::get<0>(defaults[m_defaultNameIndex]);
            nextName = std::get<1>(defaults[m_defaultNameIndex]);
        } else {
            int dividend = m_defaultNameIndex - defaults.size() + 1;
            do {
                dividend -= 1;
                int remainder = dividend % 26;
                nextId.push_front('A' + remainder);
                dividend /= 26;
            } while (dividend > 0);
        }

        m_defaultNameIndex++;
    } while (m_data->personExists(nextId));

    return addPerson(std::move(nextId), std::move(nextName));
}

// Move this to QML or QtWidgets
int PeopleTableModel::columnWidth(int columnIndex, int columnSpacing, int totalWidth)
{
    switch (columnIndex) {
        case Column::Identifier: return 150;
        case Column::Name: return totalWidth - columnSpacing * (Column::COUNT - 1) - 150;
    }

    return 0;
}

void PeopleTableModel::setDataCore(DataCoreObject* data)
{
    if (data != m_data) {
        m_data = data;
        assert(m_data);
        emit dataCoreChanged();

        // Default name reset to "A" needs to happen when we clear the model
        // entirely (new file).  People changed signal only gets sent from
        // that event for now.  In the future we may need a separate signal.
        QObject::connect(m_data, &DataCoreObject::peopleChanged,
            [this](){
                resetModel();
                m_currDefaultName = "A";
            });

        resetModel();
        m_currDefaultName = "A";
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
        { columnIndexToString(Column::Name), Column::Name }
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

void PeopleTableModel::getNextDefaultPerson()
{
    char lastChar = m_currDefaultName.back().toLatin1();
    if (lastChar == 'Z') {
        m_currDefaultName.replace(m_currDefaultName.length() - 1, 1, "AA");
    } else {
        m_currDefaultName.replace(m_currDefaultName.length() - 1, 1, lastChar + 1);
    }
}
