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
    jsonRead();
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
    emit resultsChanged();
    return true;
}

bool DataCoreObject::DeleteTransactions(int index, int count)
{
    try {
        m_data.DeleteTransactions(index, count);
        emit resultsChanged();
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
        emit resultsChanged();  // TODO: Not always
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
        emit resultsChanged();  // TODO: Not always
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
        emit resultsChanged();  // TODO: Not always
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
    m_data.Clear();
    m_identifierList.clear();
    m_nameList.clear();
    m_descriptionsList.clear();
    emit modelCleared();
}

int DataCoreObject::numPeople() const
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
    if (index < 0 || count < 1 || lastIndex >= numPeople()) {
        qDebug() << "Error - DataCoreObject::RemovePeople - Invalid parameters.  index = "
            << index << " count = " << count << " NumPeople = " << numPeople();
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
    if (index < 0 || index >= numPeople()) {
        qDebug() << "Error - DataCoreObject::GetPersonIdentifier - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return m_identifierList[index];
}

const QString& DataCoreObject::GetPersonName(int index) const
{
    if (index < 0 || index >= numPeople()) {
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

QString DataCoreObject::GetResultDebtor(int index) const
{
    if (index < 0 || index >= NumResults()) {
        qDebug() << "Error - DataCoreObject::GetResultDebtor - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return QString::fromStdString(std::get<0>(m_data.GetResults()[index]));
}

QString DataCoreObject::GetResultCreditor(int index) const
{
    if (index < 0 || index >= NumResults()) {
        qDebug() << "Error - DataCoreObject::GetResultCreditor - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return QString::fromStdString(std::get<1>(m_data.GetResults()[index]));
}

double DataCoreObject::GetResultCost(int index) const
{
    if (index < 0 || index >= NumResults()) {
        qDebug() << "Error - DataCoreObject::GetResultCost - Invalid index:" << index;
        return 0;
    }

    return std::get<2>(m_data.GetResults()[index]);
}

int DataCoreObject::NumResults() const
{
    return static_cast<int>(m_data.GetResults().size());
}

bool DataCoreObject::EditTransactionDescription(int index, QString newDescription)
{
    if (index < 0 || index >= numPeople()) {
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
    if (index < 0 || index >= numPeople()) {
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
    if (index < 0 || index >= numPeople()) {
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

bool DataCoreObject::jsonRead(const QString& filePath)
{
    // TODO: Model reset signals.
        // currently these are not needed as you go through AddPeople and AddTransaction
        // but you really need a clean way to add a bunch without ledger updates
        // and without all these signals going bananas

    return jsonRead(QUrl::fromLocalFile(filePath));
}

bool DataCoreObject::jsonWrite(const QString& filePath) const
{
    return jsonWrite(QUrl::fromLocalFile(filePath));
}

bool DataCoreObject::jsonRead(const QUrl& filePath)
{
    QString fileString(filePath.toLocalFile());
    QFile file(fileString);

    // TODO: Clean up this error code
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file for reading: " + fileString;
        qDebug() << "Code: " << file.error() << "Reason: " << file.errorString();
        emit signalError("Could not open file for reading: " + fileString);
        return false;
    }

    QJsonDocument jsonDoc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject jsonObj = jsonDoc.object();
    jsonRead(jsonObj);
    return true;
}

bool DataCoreObject::jsonWrite(const QUrl& filePath) const
{
    QString fileString(filePath.toLocalFile());
    QFile file(fileString);

    // TODO: Clean up this error code
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not open file for writing: " + fileString;
        qDebug() << "Code: " << file.error() << "Reason: " << file.errorString();
        emit signalError("Could not open file for writing: " + fileString);
        return false;
    }

    QJsonObject jsonObj;
    jsonWrite(jsonObj);
    QJsonDocument saveDoc(jsonObj);
    file.write(saveDoc.toJson());
    return true;
}

void DataCoreObject::jsonRead(const QJsonObject& jsonObj)
{
    Clear();
    QJsonArray peopleArray = jsonObj["people"].toArray();
    for (const auto& element : peopleArray)
    {
        QJsonObject personObj = element.toObject();
        AddPerson(personObj["identifier"].toString(), personObj["name"].toString());
    }

    QJsonArray transactionArray = jsonObj["transaction"].toArray();
    for (const auto& element : transactionArray)
    {
        QJsonObject transactionObj = element.toObject();
        QJsonArray transactionArray = transactionObj["covering"].toArray();

        QStringList coveringStringList;
        for (const auto& name : transactionArray) {
            coveringStringList.append(name.toString());
        }

        AddTransaction(
            transactionObj["cost"].toDouble(),
            transactionObj["payer"].toString(),
            std::move(coveringStringList),
            transactionObj["description"].toString());
    }
}

void DataCoreObject::jsonWrite(QJsonObject& jsonObj) const
{
    QJsonArray peopleArray;
    for(int i = 0; i < numPeople(); ++i)
    {
        QJsonObject curr;
        curr["identifier"] = m_identifierList[i];
        curr["name"] = m_nameList[i];
        peopleArray.append(curr);
    }
    jsonObj["people"] = peopleArray;

    QJsonArray transactionArray;
    for(int i = 0; i < static_cast<int>(m_data.NumTransactions()); ++i)
    {
        QJsonObject curr;
        curr["cost"] = GetTransactionCost(i);
        curr["payer"] = GetTransactionPayer(i);

        QJsonArray coveringArray;
        for (const auto& name : GetTransactionCovering(i)) {
            coveringArray.append(name);
        }
        curr["covering"] = coveringArray;
        curr["description"] = GetTransactionDescription(i);

        transactionArray.append(curr);
    }
    jsonObj["transaction"] = transactionArray;
}

DataCoreObject::~DataCoreObject()
{
    jsonWrite();
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
