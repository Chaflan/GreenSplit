#include "peoplemodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>      // This requires widgets, see if there is another way to do this

PeopleModel::PeopleModel(DataCore& dataCore, QObject* parent) :
    QAbstractTableModel(parent),
    m_data(dataCore)
{
}

int PeopleModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_data.NumPeople();
}

int PeopleModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : Sections::COUNT;
}

QVariant PeopleModel::data(const QModelIndex& index, int role) const
{
    if (isIndexValid(index))
    {
        auto person = m_data.GetPersonByIndex(index.row());

        switch (role)
        {
            case Qt::DisplayRole:
            {
                switch (index.column())
                {
                    case Sections::Initials: return person.initials;
                    case Sections::Name: return person.name;
                }
                break;
            }
            case Roles::IdRole: return person.id;
            case Roles::InitialsRole: return person.initials;
            case Roles::NameRole: return person.name;
        }
    }

    return QVariant();
}

// TODO: Is this really the best way to do this, recursion?  Maybe we need an overload
// The use of roles like this is really not as the model was intended.  Perhaps we
// Should instead have a different method that can be called directly.  EditRoles?

bool PeopleModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!isIndexValid(index)) {
        return false;
    }

    if (role == Qt::EditRole) {
        switch(index.column()) {
            case Sections::Initials: return setData(index, value, Roles::InitialsRole);
            case Sections::Name: return setData(index, value, Roles::NameRole);
        }

    } else if (role == Roles::InitialsRole || role == Roles::NameRole) {
        Person personCopy(m_data.GetPersonByIndex(index.row()));

        switch (role) {
            case Roles::InitialsRole: personCopy.initials = value.toString(); break;
            case Roles::NameRole: personCopy.name = value.toString(); break;
        }

        if (m_data.EditPerson(index.row(), personCopy)) {
            emit dataChanged(index, index);
            return true;
        } else {
            if (role == Roles::InitialsRole) {
                QMessageBox msg;
                msg.setText("Initials must be unique");
                msg.exec();
            }
        }
    }

    return false;
}

bool PeopleModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    bool result = m_data.DeletePeople(row, count);
    endRemoveRows();
    return result;
}

QVariant PeopleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case Sections::Initials: return "Initials";
            case Sections::Name: return "Name";
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags PeopleModel::flags(const QModelIndex& index) const
{
    // TODO: I don't understand the purpose of this return variable.
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool PeopleModel::addPerson(QString initials, QString name)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    bool result = m_data.AddPerson({std::move(initials), std::move(name)});
    endInsertRows();
    return result;
}

void PeopleModel::jsonRead(const QJsonObject& json)
{
    beginRemoveRows(QModelIndex(), 0, m_data.NumPeople() - 1);
    m_data.ClearPeople();
    endRemoveRows();

    QJsonArray peopleArray = json["people"].toArray();
    beginInsertRows(QModelIndex(), 0, peopleArray.count() - 1);
    for (const auto& element : peopleArray)
    {
        QJsonObject personObj = element.toObject();
        m_data.AddPerson({personObj["initials"].toString(), personObj["name"].toString()});
    }
    endInsertRows();
}

void PeopleModel::jsonWrite(QJsonObject& json) const
{
    QJsonArray peopleArray;
    for(int i = 0; i < m_data.NumPeople(); ++i)
    {
        QJsonObject curr;
        curr["initials"] = m_data.GetPersonByIndex(i).initials;
        curr["name"] = m_data.GetPersonByIndex(i).name;
        peopleArray.append(curr);
    }
    json["people"] = peopleArray;
}

bool PeopleModel::isIndexValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}
