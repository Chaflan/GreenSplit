#ifndef DATACOREOBJECT_H
#define DATACOREOBJECT_H

#include <QObject>
#include "datacore.h"

class BILLSPLITCORE_EXPORT DataCoreObject : public QObject
{
    Q_OBJECT
public:
    explicit DataCoreObject(QObject *parent = nullptr);

    // TODO: Upper Lower case method name confusion

    // DataCore wrapper methods
    void AddTransaction(const QString& payer, double cost, const QStringList& covering);
    bool DeleteTransaction(int index);
    bool EditTransactionPayer(int index, const QString& newPayer);
    bool EditTransactionCost(int index, double newCost);
    bool EditTransactionCovering(int index, const QStringList& newCovering);
    bool EditPerson(const QString& oldName, const QString& newName);
    bool PersonExists(const QString& person) const;
    void Clear();

    // DataCore extending methods
    int NumPeople() const;
    bool AddPerson(QString identifier, QString name);
    bool RemovePeople(int index, int count);
    QString GetPersonIdentifier(int index) const;
    QString GetPersonName(int index) const;
    bool EditPersonIdentifier(int index, const QString& newIdentifier);
    bool EditPersonName(int index, QString newName);

    // Save and Load methods.
    // TODO: Move these to their own class, or use a database
    void JsonRead();
    void JsonWrite() const;
    virtual ~DataCoreObject();

signals:
    void identifierListChanged();
    void nameListChanged();

private:
    QStringList m_identifierList;
    QStringList m_nameList;
    DataCore m_data;
};

#endif // DATACOREOBJECT_H
