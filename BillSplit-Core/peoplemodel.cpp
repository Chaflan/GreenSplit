#include "peoplemodel.h"
#include <QJsonArray>
#include <QJsonObject>

PeopleModel::PeopleModel(QObject* parent) :
    QAbstractListModel(parent)
{
}

int PeopleModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return people.size();
}

QVariant PeopleModel::data(const QModelIndex& index, int role) const
{
    if (isIndexValid(index))
    {
        auto personPtr = people[index.row()];

        switch (role)
        {
            case Qt::DisplayRole: return personPtr->initials + " - \t" + personPtr->name;
            case Roles::IdRole: return personPtr->id;
            case Roles::InitialsRole: return personPtr->initials;
            case Roles::NameRole: return personPtr->name;
        }
    }

    return QVariant();
}

bool PeopleModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!validateData(index, value, role))
    {
        return false;
    }

    if (role == Roles::InitialsRole)
    {
        QString newInitials = value.toString();
        const QString& oldInitials = people[index.row()]->initials;
        auto nodeHandle = initialsLookup.extract(oldInitials);
        nodeHandle.key() = newInitials;
        nodeHandle.mapped()->initials = std::move(newInitials);
        initialsLookup.insert(std::move(nodeHandle));
    }
    else if (role == Roles::NameRole)
    {
        QString newName = value.toString();
        people[index.row()]->name = std::move(newName);
    }

    emit dataChanged(index, index);
    return true;
}

bool PeopleModel::removeRows(int row, int count, const QModelIndex& parent)
{
    int last = row + count - 1;
    if (row < 0 || count < 0 || last >= rowCount())
    {
        return false;
    }

    beginRemoveRows(parent, row, last);
    for(int i = row; i <= last; ++i)
    {
        initialsLookup.erase(people[i]->initials);
    }
    people.erase(people.begin() + row, people.begin() + last + 1);
    endRemoveRows();
    return true;
}

bool PeopleModel::validateData(const QModelIndex& index, const QVariant& value, int role) const
{
    return isIndexValid(index) && validateData(value, role);
}

bool PeopleModel::validateData(const QVariant& value, int role) const
{
    return validateData(value.toString(), role);
}

bool PeopleModel::validateData(const QString& value, int role) const
{
    if (role == Roles::InitialsRole)
    {
        if (value.isEmpty())
        {
            // Empty initials
            return false;
        }
        if (initialsLookup.find(value) != initialsLookup.end())
        {
            // Initials already in use
            return false;
        }
    }
    else if (role == Roles::NameRole)
    {
        if (value.isEmpty())
        {
            // Empty name
            return false;
        }
    }
    else
    {
        // Role invalid
        return false;
    }

    return true;
}

bool PeopleModel::addPerson(QString initials, QString name)
{
    if (!validateData(initials, InitialsRole) || !validateData(name, NameRole))
    {
        return false;
    }

    auto NewPerson = std::make_shared<Person>(std::move(initials), std::move(name));
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    people.emplace_back(NewPerson);
    initialsLookup.emplace(NewPerson->initials, NewPerson);
    endInsertRows();

    return true;
}

void PeopleModel::jsonRead(const QJsonObject& json)
{
    // TODO: You could speed this up with bulk begin insert rows, but you
    // also need to consider telling the model you are clearing the data
    // revist this later.  You are probably changing it anyway.
    people.clear();
    initialsLookup.clear();
    QJsonArray peopleArray = json["people"].toArray();
    for (const auto& element : peopleArray)
    {
        QJsonObject personObj = element.toObject();
        addPerson(personObj["initials"].toString(), personObj["name"].toString());
    }
}

void PeopleModel::jsonWrite(QJsonObject& json) const
{
    QJsonArray peopleArray;
    for(auto personPtr : people)
    {
        QJsonObject curr;
        curr["initials"] = personPtr->initials;
        curr["name"] = personPtr->name;
        peopleArray.append(curr);
    }
    json["people"] = peopleArray;
}

bool PeopleModel::isIndexValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() < static_cast<int>(people.size());
}
