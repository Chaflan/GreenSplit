#include "peoplemodel.h"
#include <QJsonArray>
#include <QJsonObject>

PeopleModel::PeopleModel(DataCore& dataCore, QObject* parent) :
    QAbstractListModel(parent),
    m_data(dataCore)
{
}

int PeopleModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_data.NumPeople();
}

QVariant PeopleModel::data(const QModelIndex& index, int role) const
{
    if (isIndexValid(index))
    {
        auto person = m_data.GetPersonByIndex(index.row());

        switch (role)
        {
            case Qt::DisplayRole: return person.initials + " - \t" + person.name;
            case Roles::IdRole: return person.id;
            case Roles::InitialsRole: return person.initials;
            case Roles::NameRole: return person.name;
        }
    }

    return QVariant();
}

bool PeopleModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (isIndexValid(index) && (role == Roles::InitialsRole || role == Roles::NameRole))
    {
        Person personCopy(m_data.GetPersonByIndex(index.row()));

        switch (role)
        {
            case Roles::InitialsRole: personCopy.initials = value.toString(); break;
            case Roles::NameRole: personCopy.name = value.toString(); break;
        }

        if (m_data.EditPerson(index.row(), personCopy))
        {
            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

bool PeopleModel::removeRows(int row, int count, const QModelIndex& parent)
{
    // TODO: is parent used?  How?  Check validity?

    beginRemoveRows(parent, row, row + count - 1);
    bool result = m_data.DeletePeople(row, count);
    endRemoveRows();
    return result;
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
    return index.isValid() && index.row() < rowCount();
}
