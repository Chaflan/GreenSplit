#ifndef DATACOREOBJECT_H
#define DATACOREOBJECT_H

#include <QObject>
#include "datacore.h"

// TODO: rename billsplitcore_global to be lowercaswe
// TODO: Standardize use of name vs identifier
// TODO: QDebug lines
// TODO: Upper Lower case method name confusion (all lower in any qobject for qinvokableness)
// TODO: Return whole list in some cases?
// TODO: Currently there is nothing preventing the front end from adding a transaction with nonexistant people
// TODO: Standardize name vs identifier
// TODO: Braced initialization


// TODO: Remove
//#include <QUrl>

class BILLSPLITCORE_EXPORT DataCoreObject : public QObject
{
    Q_OBJECT
public:
    explicit DataCoreObject(QObject *parent = nullptr);

    // DataCore wrapper methods
    int NumTransactions() const;
    bool AddTransaction(double cost, const QString& payer, const QStringList& covering, QString description);
    bool DeleteTransactions(int index, int count);
    bool EditTransactionPayer(int index, const QString& newPayer);
    bool EditTransactionCost(int index, double newCost);
    bool EditTransactionCovering(int index, const QStringList& newCovering);
    QString GetTransactionPayer(int index) const; // Cache this and return const &
    double GetTransactionCost(int index) const;
    QStringList GetTransactionCovering(int index) const; // TODO: Stringlist?  Cache this?
    bool EditPerson(const QString& oldName, const QString& newName);
    void Clear();

    // DataCore extending methods
    Q_INVOKABLE int numPeople() const;
    bool PersonExists(const QString& identifier) const;
    bool PersonInTransactions(const QString& identifier) const;
    bool AddPerson(QString identifier, QString name);
    bool RemovePeople(int index, int count);
    const QString& GetPersonIdentifier(int index) const;
    const QString& GetPersonName(int index) const;
    const QString& GetTransactionDescription(int index) const;
    QString GetResultDebtor(int index) const;
    QString GetResultCreditor(int index) const;
    double GetResultCost(int index) const;
    int NumResults() const;
    bool EditTransactionDescription(int index, QString newDescription);   // It might be that the fail case is more common meaning we want const ref here
    bool EditPersonIdentifier(int index, const QString& newIdentifier);
    bool EditPersonName(int index, QString newName);
    const QStringList& GetIdentifierList() const;

    // Save and Load methods.
    // TODO: Move these to their own class, or use a database
    // Can probably do some smart overloading here to cut down on num decls
    bool jsonRead(const QString& filePath = "save.json");
    bool jsonWrite(const QString& filePath = "save.json") const;
    Q_INVOKABLE bool jsonRead(const QUrl& filePath);
    Q_INVOKABLE bool jsonWrite(const QUrl& filePath) const;
    void jsonRead(const QJsonObject& jsonObj);
    void jsonWrite(QJsonObject& jsonObj) const;

    virtual ~DataCoreObject();

signals:
    void signalError(QString error) const;
    void identifierListChanged() const;
    void nameListChanged() const;
    void resultsChanged() const;
    void modelCleared() const;  // TODO: rethink this

private:
    std::set<std::string> stringListToStdSet(const QStringList& stringList);

private:
    QStringList m_identifierList;
    QStringList m_nameList;
    QStringList m_descriptionsList;
    DataCore m_data;
};

#endif // DATACOREOBJECT_H
