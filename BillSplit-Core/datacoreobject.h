#ifndef DATACOREOBJECT_H
#define DATACOREOBJECT_H

#include <QObject>
#include "datacore.h"

// TODO: Standardize use of name vs identifier
// TODO: QDebug lines
// TODO: Upper Lower case method name confusion

class BILLSPLITCORE_EXPORT DataCoreObject : public QObject
{
    Q_OBJECT
public:
    explicit DataCoreObject(QObject *parent = nullptr);

    // DataCore wrapper methods
    void AddTransaction(const QString& payer, double cost, const QStringList& covering);
    bool DeleteTransaction(int index);
    bool EditTransactionPayer(int index, const QString& newPayer);
    bool EditTransactionCost(int index, double newCost);
    bool EditTransactionCovering(int index, const QStringList& newCovering);
    bool EditPerson(const QString& oldName, const QString& newName);
    void Clear();

    // DataCore extending methods
    int NumPeople() const;
    bool PersonExists(const QString& identifier) const;
    bool PersonInTransactions(const QString& identifier) const;
    bool AddPerson(QString identifier, QString name);
    bool RemovePeople(int index, int count);
    const QString& GetPersonIdentifier(int index) const; // TODO: Const ref with error
    const QString& GetPersonName(int index) const; // TODO: Const ref with error
    bool EditPersonIdentifier(int index, const QString& newIdentifier);
    bool EditPersonName(int index, QString newName);

    // Save and Load methods.
    // TODO: Move these to their own class, or use a database
    void JsonRead();
    void JsonWrite() const;
    virtual ~DataCoreObject();

signals:
    void signalError(QString error) const;
    void identifierListChanged() const;
    void nameListChanged() const;

private:
    QStringList m_identifierList;
    QStringList m_nameList;
    DataCore m_data;
};

#endif // DATACOREOBJECT_H
