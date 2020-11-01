#include "datacoreobject.h"
#include <QDebug>

// TODO: Future remove includes
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>

// TODO: Disallow blank payer name?

DataCoreObject::DataCoreObject(QObject *parent) :
    QObject(parent)
{
    JsonRead();
}

int DataCoreObject::NumTransactions() const
{
    return static_cast<int>(m_data.NumTransactions());
}

bool DataCoreObject::AddTransaction(double cost, const QString& payer, const QStringList& covering, QString description)
{
    try {
        m_data.AddTransaction(payer.toStdString(), cost, stringListToStdSet(covering));
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::AddTransaction - " << ex.what();
        return false;
    }

    m_descriptionsList.append(std::move(description));
    return true;
}

bool DataCoreObject::DeleteTransactions(int index, int count)
{
    try {
        m_data.DeleteTransactions(index, count);
        return true;
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::DeleteTransactions - " << ex.what();
    }

    return false;
}

bool DataCoreObject::EditTransactionPayer(int index, const QString& newPayer)
{
    try {
        m_data.EditTransactionPayer(index, newPayer.toStdString());
        return true;
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionPayer - " << ex.what();
    }

    return false;
}

bool DataCoreObject::EditTransactionCost(int index, double newCost)
{
    try {
        m_data.EditTransactionCost(index, newCost);
        return true;
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionPayer - " << ex.what();
    }

    return false;
}

bool DataCoreObject::EditTransactionCovering(int index, const QStringList& newCovering)
{
    if (newCovering.isEmpty()) {
        emit signalError("A transaction must cover one or more person");
        return false;
    }

    try {
        m_data.EditTransactionCovering(index, stringListToStdSet(newCovering));
        return true;
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionPayer - " << ex.what();
    }

    return false;
}

QString DataCoreObject::GetTransactionPayer(int index) const
{
    try {
        return QString::fromStdString(m_data.GetTransactionPayer(index));
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionPayer - " << ex.what();
    }

    return QString();
}

double DataCoreObject::GetTransactionCost(int index) const
{
    try {
        return m_data.GetTransactionCost(index);
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionCost - " << ex.what();
    }

    return 0;
}

QStringList DataCoreObject::GetTransactionCovering(int index) const
{
    QStringList result;
    try {
        std::set<std::string> coveringSet = m_data.GetTransactionCovering(index);
        for (const std::string& str : coveringSet) {
            result.append(QString::fromStdString(str));
        }
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionCovering - " << ex.what();
        result.clear();
    }

    return result;
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

// Returns true if the person shows up in the identifier list.
// If this is true, PersonInTransactions will also be true, but not
// vice versa.  You can have a person where PersonExists==true but
// PersonInTransactions==false.
bool DataCoreObject::PersonExists(const QString& identifier) const
{
    return m_identifierList.contains(identifier);
}

// Returns true if the datacore has knowledge of the person, meaning
// they are involved in one or more transactions
bool DataCoreObject::PersonInTransactions(const QString& identifier) const
{
    return m_data.PersonExists(identifier.toStdString());
}

bool DataCoreObject::AddPerson(QString identifier, QString name)
{
    if (PersonExists(identifier)) {
        emit signalError("Identifier \"" + identifier + "\" already exists. Identifiers must be unique.");
        return false;
    }

    m_identifierList.append(std::move(identifier));
    emit identifierListChanged();
    m_nameList.append(std::move(name));
    return true;
}

bool DataCoreObject::RemovePeople(int index, int count)
{
    const int lastIndex = index + count - 1;
    if (index < 0 || count < 1 || lastIndex >= NumPeople()) {
        qDebug() << "Error - DataCoreObject::RemovePeople - Invalid parameters.  index = "
            << index << " count = " << count << " NumPeople = " << NumPeople();
        return false;
    }

    QVector<int> indicesToDelete;
    for (int i = lastIndex; i >= index; --i) {
        const QString& currentPerson = m_identifierList[i];
        if (PersonInTransactions(currentPerson)) {
            emit signalError("Identifier \"" + currentPerson + "\" is involved in one or more transactions. They cannot be deleted.");
        } else {
            indicesToDelete.append(i);
        }
    }

    if (indicesToDelete.length() > 0) {
        if (indicesToDelete.length() == count) {
            // Bulk delete if none are being left behind (faster)
            m_identifierList.erase(m_identifierList.begin() + index, m_identifierList.begin() + lastIndex + 1);
            m_nameList.erase(m_nameList.begin() + index, m_nameList.begin() + lastIndex + 1);
        } else {
            // Individual delete otherwise (slower)
            for (int i : indicesToDelete) {
                m_identifierList.removeAt(i);
                m_nameList.removeAt(i);
            }
        }

        emit identifierListChanged();
    }

    return true;
}

const QString& DataCoreObject::GetPersonIdentifier(int index) const
{
    if (index < 0 || index >= NumPeople()) {
        qDebug() << "Error - DataCoreObject::GetPersonIdentifier - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return m_identifierList[index];
}

const QString& DataCoreObject::GetPersonName(int index) const
{
    if (index < 0 || index >= NumPeople()) {
        qDebug() << "Error - DataCoreObject::GetPersonName - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return m_nameList[index];
}

const QString& DataCoreObject::GetTransactionDescription(int index) const
{
    if (index < 0 || index >= NumTransactions()) {
        qDebug() << "Error - DataCoreObject::GetTransactionDescription - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return m_descriptionsList[index];
}

bool DataCoreObject::EditTransactionDescription(int index, QString newDescription)
{
    if (index < 0 || index >= NumPeople()) {
        qDebug() << "Error - DataCoreObject::EditTransactionDescription - Invalid index:" << index;
        return false;
    }

    // Editing to what it already is, nothing to do.
    if (m_descriptionsList[index] == newDescription) {
        return true;
    }

    m_descriptionsList[index] = std::move(newDescription);
    // Signal people changed
    return true;
}

bool DataCoreObject::EditPersonIdentifier(int index, const QString& newIdentifier)
{
    if (index < 0 || index >= NumPeople()) {
        qDebug() << "Error - DataCoreObject::EditPersonIdentifier - Invalid index:" << index;
        return false;
    }

    // Editing to what it already is, nothing to do
    if (m_identifierList[index] == newIdentifier) {
        return true;
    }

    if (newIdentifier.isEmpty()) {
        emit signalError("Attempting to change identifier to be empty.  Identifiers cannot be empty.");
        return false;
    }

    // TODO: Remove this after you implement the m_data.EditPerson method.  It should throw this error.
    if (PersonExists(newIdentifier)) {
        emit signalError("Attempting to change identifier to one that already exists.  Identifiers must be unique.");
        return false;
    }

    // TODO: m_data.EditPerson(m_identifierList[index].toStdString(), newIdentifier.toStdString());
    m_identifierList[index] = std::move(newIdentifier);
    emit identifierListChanged();
    return true;
}

bool DataCoreObject::EditPersonName(int index, QString newName)
{
    if (index < 0 || index >= NumPeople()) {
        qDebug() << "Error - DataCoreObject::EditPersonName - Invalid index:" << index;
        return false;
    }

    // Editing to what it already is, nothing to do.
    if (m_nameList[index] == newName) {
        return true;
    }

    m_nameList[index] = std::move(newName);
    return true;
}

const QStringList& DataCoreObject::GetIdentifierList() const
{
    return m_identifierList;
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

std::set<std::string> DataCoreObject::stringListToStdSet(const QStringList& stringList)
{
    std::set<std::string> ret;
    for (const QString& str : stringList) {
        if (ret.insert(str.toStdString()).second == false) {
            qDebug() << "Error - DataCoreObject::stringListToStdSet - Inserting duplicate covering name:" << str;
        }
    }
    return ret;
}
