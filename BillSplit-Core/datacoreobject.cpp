#include "datacoreobject.h"
#include <QDebug>

// TODO: Future remove includes
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QDir>

// TODO: Disallow blank payer name?

DataCoreObject::DataCoreObject(QObject *parent) :
    QObject(parent)
{
    jsonRead();
}

DataCoreObject::~DataCoreObject()
{
    jsonWrite();
}

int DataCoreObject::numTransactions() const
{
    return static_cast<int>(m_data.NumTransactions());
}

// silent flag exists for bulk transaction add
bool DataCoreObject::addTransaction(double cost, const QString& payer, const QStringList& covering, QString description, bool silent)
{
    try {
        m_data.AddTransaction(payer.toStdString(), cost, stringListToStdSet(covering));
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::AddTransaction - " << ex.what();
        return false;
    }
    m_descriptionsList.append(std::move(description));

    if (!silent) {
        emit resultsChanged();
    }
    return true;
}

bool DataCoreObject::deleteTransactions(int index, int count)
{
    bool success = false;
    try {
        success = m_data.DeleteTransactions(index, count);
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::DeleteTransactions - " << ex.what();
        return false;
    }

    if (success) {
        emit resultsChanged();
    }
    return success;
}

bool DataCoreObject::editTransactionPayer(int index, const QString& newPayer)
{
    bool wasChanged = false;
    try {
        wasChanged = m_data.EditTransactionPayer(index, newPayer.toStdString());
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionPayer - " << ex.what();
        return false;
    }

    if (wasChanged) {
        emit resultsChanged();
    }
    return true;
}

// TODO: Make a note somewhere.  DCO returns false if something went wrong, DC returns true if something changed.
bool DataCoreObject::editTransactionCost(int index, double newCost)
{
    bool wasChanged = false;
    try {
        wasChanged = m_data.EditTransactionCost(index, newCost);
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionPayer - " << ex.what();
        return false;
    }

    if (wasChanged) {
        emit resultsChanged();
    }
    return true;
}

bool DataCoreObject::editTransactionDescription(int index, const QString& newDescription)
{
    if (index < 0 || index >= numPeople()) {
        qDebug() << "Error - DataCoreObject::EditTransactionDescription - Invalid index:" << index;
        return false;
    }

    // Editing to what it already is, nothing to do.
    if (m_descriptionsList[index] == newDescription) {
        return true;
    }

    m_descriptionsList[index] = newDescription;
    return true;
}

bool DataCoreObject::editTransactionCovering(int index, const QStringList& newCovering)
{
    if (newCovering.isEmpty()) {
        emit signalError("A transaction must cover one or more person");
        return false;
    }

    bool wasChanged = false;
    try {
        wasChanged = m_data.EditTransactionCovering(index, stringListToStdSet(newCovering));
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionPayer - " << ex.what();
        return false;
    }

    if (wasChanged) {
        emit resultsChanged();
    }
    return true;
}

QString DataCoreObject::getTransactionPayer(int index) const
{
    try {
        return QString::fromStdString(m_data.GetTransactionPayer(index));
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionPayer - " << ex.what();
    }

    return QString();
}

double DataCoreObject::getTransactionCost(int index) const
{
    try {
        return m_data.GetTransactionCost(index);
    } catch (const std::exception& ex) {
        qDebug() << "Error - DataCoreObject::GetTransactionCost - " << ex.what();
    }

    return 0;
}

QStringList DataCoreObject::getTransactionCovering(int index) const
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

void DataCoreObject::clear()
{
    m_data.Clear();
    m_identifierList.clear();
    m_nameList.clear();
    m_descriptionsList.clear();
    emit peopleChanged();
    emit transactionsChanged();
    emit resultsChanged();
}

int DataCoreObject::numPeople() const
{
    return static_cast<int>(m_identifierList.size());
}

// Returns true if the person shows up in the identifier list.
// If this is true, PersonInTransactions will also be true, but not
// vice versa.  You can have a person where PersonExists==true but
// PersonInTransactions==false.
bool DataCoreObject::personExists(const QString& identifier) const
{
    // TODO: This is quite slow, maybe consider the data structure for it
    return m_identifierList.contains(identifier);
}

// Returns true if the datacore has knowledge of the person, meaning
// they are involved in one or more transactions
bool DataCoreObject::personInTransactions(const QString& identifier) const
{
    return m_data.PersonExists(identifier.toStdString());
}

bool DataCoreObject::addPerson(QString identifier, QString name)
{
    if (!isNewIdentifierValid(identifier)) {
        return false;
    }

    m_identifierList.append(std::move(identifier));
    m_nameList.append(std::move(name));
    return true;
}

// Must return true only if people were removed
bool DataCoreObject::removePeople(int index, int count)
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
        if (personInTransactions(currentPerson)) {
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

        return true;
    }

    return false;
}

const QString& DataCoreObject::getPersonIdentifier(int index) const
{
    if (index < 0 || index >= numPeople()) {
        qDebug() << "Error - DataCoreObject::GetPersonIdentifier - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return m_identifierList[index];
}

const QString& DataCoreObject::getPersonName(int index) const
{
    if (index < 0 || index >= numPeople()) {
        qDebug() << "Error - DataCoreObject::GetPersonName - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return m_nameList[index];
}

const QString& DataCoreObject::getTransactionDescription(int index) const
{
    if (index < 0 || index >= numTransactions()) {
        qDebug() << "Error - DataCoreObject::GetTransactionDescription - Invalid index:" << index;
        const static QString errorString = "";
        return errorString;
    }

    return m_descriptionsList[index];
}

QString DataCoreObject::getResultDebtor(int index) const
{
    QString result;
    if (index < 0 || index >= numResults()) {
        qDebug() << "Error - DataCoreObject::GetResultDebtor - Invalid index:" << index;
    } else {
        try {
            result = QString::fromStdString(std::get<0>(m_data.GetResults()[index]));
        } catch (const std::exception& ex) {
            qDebug() << "Error - DataCoreObject::GetResultDebtor - " << ex.what();
            result.clear();
        }
    }
    return result;
}

QString DataCoreObject::getResultCreditor(int index) const
{
    QString result;
    if (index < 0 || index >= numResults()) {
        qDebug() << "Error - DataCoreObject::GetResultCreditor - Invalid index:" << index;
    } else {
        try {
            result = QString::fromStdString(std::get<1>(m_data.GetResults()[index]));
        } catch (const std::exception& ex) {
            qDebug() << "Error - DataCoreObject::GetResultDebtor - " << ex.what();
            result.clear();
        }
    }
    return result;
}

double DataCoreObject::getResultCost(int index) const
{
    double result = 0;
    if (index < 0 || index >= numResults()) {
        qDebug() << "Error - DataCoreObject::GetResultCost - Invalid index:" << index;
    } else {
        try {
            result = std::get<2>(m_data.GetResults()[index]);
        } catch (const std::exception& ex) {
            qDebug() << "Error - DataCoreObject::GetResultDebtor - " << ex.what();
            result = 0;
        }
    }
    return result;
}

int DataCoreObject::numResults() const
{

    return static_cast<int>(m_data.GetResults().size());
}

bool DataCoreObject::editPersonIdentifier(int index, const QString& newIdentifier)
{
    if (index < 0 || index >= numPeople()) {
        qDebug() << "Error - DataCoreObject::EditPersonIdentifier - Invalid index:" << index;
        return false;
    }

    // Editing to what it already is, nothing to do
    if (m_identifierList[index] == newIdentifier) {
        return true;
    }

    if (!isNewIdentifierValid(newIdentifier)) {
        return false;
    }

    bool editedInTransactions = m_data.EditPerson(m_identifierList[index].toStdString(), newIdentifier.toStdString());;
    m_identifierList[index] = newIdentifier;
    if (editedInTransactions) {
        emit transactionsChanged();
        emit resultsChanged();
    }

    return true;
}

bool DataCoreObject::editPersonName(int index, QString newName)
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

bool DataCoreObject::jsonRead(const QString& filePath)
{
    return jsonRead(QUrl::fromLocalFile(addJsonExtension(filePath)));
}

bool DataCoreObject::jsonWrite(const QString& filePath) const
{
    return jsonWrite(QUrl::fromLocalFile(addJsonExtension(filePath)));
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
    clear();
    QJsonArray peopleArray = jsonObj["people"].toArray();
    for (const auto& element : peopleArray)
    {
        QJsonObject personObj = element.toObject();
        addPerson(personObj["identifier"].toString(), personObj["name"].toString());
    }

    emit peopleChanged();

    QJsonArray transactionArray = jsonObj["transaction"].toArray();
    for (const auto& element : transactionArray)
    {
        QJsonObject transactionObj = element.toObject();
        QJsonArray transactionArray = transactionObj["covering"].toArray();

        QStringList coveringStringList;
        for (const auto& name : transactionArray) {
            coveringStringList.append(name.toString());
        }

        // Bulk add silently and then emit the results changed signal
        // to avoid a recalc upon each add.
        addTransaction(
            transactionObj["cost"].toDouble(),
            transactionObj["payer"].toString(),
            std::move(coveringStringList),
            transactionObj["description"].toString(),
            true);
    }

    emit transactionsChanged();
    emit resultsChanged();
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
    for(int i = 0; i < numTransactions(); ++i)
    {
        QJsonObject curr;
        curr["cost"] = getTransactionCost(i);
        curr["payer"] = getTransactionPayer(i);

        QJsonArray coveringArray;
        for (const auto& name : getTransactionCovering(i)) {
            coveringArray.append(name);
        }
        curr["covering"] = coveringArray;
        curr["description"] = getTransactionDescription(i);

        transactionArray.append(curr);
    }
    jsonObj["transaction"] = transactionArray;
}

QStringList DataCoreObject::getLocalSaveFiles() const
{
    QDir directory;
    QStringList entriesWithExt = directory.entryList(QStringList() << "*.json" << "*.JSON", QDir::Files);

    QStringList entriesNameOnly;
    for (const QString& entry : entriesWithExt) {
        entriesNameOnly.append(entry.left(entry.lastIndexOf(".")));
    }

    return entriesNameOnly;
}

QString DataCoreObject::addJsonExtension(QString path)
{
    if (!path.endsWith(".json", Qt::CaseInsensitive)) {
        path += ".json";
    }

    return path;
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

bool DataCoreObject::isNewIdentifierValid(const QString& identifier) const
{
    if (identifier.isEmpty()) {
        emit signalError("Identifiers cannot be empty.");
        return false;
    }

    if (identifier.length() >= 3) {
        emit signalError("Identifiers cannot exceed 3 characters in length");
        return false;
    }

    if (personExists(identifier)) {
        emit signalError("Identifier \"" + identifier + "\" already exists. Identifiers must be unique.");
        return false;
    }

    return true;
}
