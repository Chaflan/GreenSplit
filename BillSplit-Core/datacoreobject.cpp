#include "datacoreobject.h"

// TODO: Future remove includes
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>

DataCoreObject::DataCoreObject(QObject *parent) :
    QObject(parent)
{
    JsonRead();
}

bool DataCoreObject::PersonExists(const QString& person) const
{
    // TODO: Stub
    Q_UNUSED(person);
    return false;
}

void DataCoreObject::Clear()
{
    // TODO: m_data.Clear();
    m_identifierList.clear();
    m_nameList.clear();
}

int DataCoreObject::NumPeople() const
{
    return static_cast<int>(m_identifierList.size());
}

bool DataCoreObject::AddPerson(QString identifier, QString name)
{
    if (PersonExists(identifier)) {
        // Signal error here
        return false;
    }

    m_identifierList.append(std::move(identifier));
    m_nameList.append(std::move(name));
    // Signal people changed
    return true;
}

bool DataCoreObject::RemovePeople(int index, int count)
{
    // TODO: Stub
    Q_UNUSED(index);
    Q_UNUSED(count);
    return false;
}

QString DataCoreObject::GetPersonIdentifier(int index) const
{
    if (index < 0 || index >= NumPeople()) {
        return QString();
    }

    return m_identifierList[index];
}

QString DataCoreObject::GetPersonName(int index) const
{
    if (index < 0 || index >= NumPeople()) {
        return QString();
    }

    return m_nameList[index];
}

bool DataCoreObject::EditPersonIdentifier(int index, const QString& newIdentifier)
{
    if (index < 0 || index >= NumPeople()) {
        return false;
    }

    if (PersonExists(newIdentifier)) {
        // TODO: Signal error here
        return false;
    }

    m_identifierList[index] = std::move(newIdentifier);
    // Signal people changed
    return true;
}

bool DataCoreObject::EditPersonName(int index, QString newName)
{
    if (index < 0 || index >= NumPeople())
        return false;

    m_nameList[index] = std::move(newName);
    // Signal people changed
    return true;
}

void DataCoreObject::JsonRead()
{
    // TODO: Model reset signals
    // TODO: Initials -> identifier

    QFile file("save.json");
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument jsonDoc(QJsonDocument::fromJson(file.readAll()));
        QJsonObject jsonObj = jsonDoc.object();

        Clear();
        QJsonArray peopleArray = jsonObj["people"].toArray();
        for (const auto& element : peopleArray)
        {
            QJsonObject personObj = element.toObject();
            AddPerson(personObj["initials"].toString(), personObj["name"].toString());
        }
    }
}

void DataCoreObject::JsonWrite() const
{
    // TODO: Initials -> identifier

    QFile file("save.json");
    if (file.open(QIODevice::WriteOnly))
    {
        QJsonObject jsonObj;
        QJsonArray peopleArray;
        for(int i = 0; i < NumPeople(); ++i)
        {
            QJsonObject curr;
            curr["initials"] = m_identifierList[i];
            curr["name"] = m_nameList[i];
            peopleArray.append(curr);
        }
        jsonObj["people"] = peopleArray;

        QJsonDocument saveDoc(jsonObj);
        file.write(saveDoc.toJson());
    }
}

DataCoreObject::~DataCoreObject()
{
    JsonWrite();
}
