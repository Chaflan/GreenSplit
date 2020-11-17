#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QObject>
#include "BillSplit-Core_global.h"
#include "datacoreobject.h"

// TODO: Consistent order cost payer covering description
// TODO: const signals in all classes
// TODO: dataSet name?  dataChaned has already been used
// TODO: some of the sets and gets can be private

class BILLSPLITCORE_EXPORT PersonCheck : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool checkStatus READ getCheckStatus WRITE setCheckStatus NOTIFY checkStatusChanged)

public:
    explicit PersonCheck(QObject *parent = nullptr);
    PersonCheck(QString name, bool checkStatus, QObject* parent = nullptr);

    QString getName() const     { return m_name; }
    int getCheckStatus() const  { return m_checkStatus; }

    void setName(QString name);
    void setCheckStatus(bool checkStatus);

signals:
    void nameChanged() const;
    void checkStatusChanged() const;

private:
    QString m_name;
    bool m_checkStatus;
};

class BILLSPLITCORE_EXPORT TransactionModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DataCoreObject* data READ getDataCore WRITE setDataCore NOTIFY dataSet)

    // TODO: Try MEMBER with a NOTIFY only called by non-setters
    Q_PROPERTY(QString payerName READ getPayerName WRITE setPayerName NOTIFY payerNameChanged)
    Q_PROPERTY(int payerIndex READ getPayerIndex WRITE setPayerIndex NOTIFY payerIndexChanged)
    Q_PROPERTY(double cost READ getCost WRITE setCost NOTIFY costChanged)
    Q_PROPERTY(QString costStr READ getCostStr WRITE setCostStr NOTIFY costChanged)  // TODO: Cache this
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged USER true)

    // TODO: Readonly?
    Q_PROPERTY(QStringList allPeople READ getAllPeople NOTIFY allPeopleChanged)
    Q_PROPERTY(QList<PersonCheck*> coveringList READ getCoveringList NOTIFY coveringListChanged)

public:
    explicit TransactionModel(QObject *parent = nullptr);

    DataCoreObject* getDataCore() const          { return m_data; }
    QString getPayerName() const                 { return m_payerName; }
    int getPayerIndex() const                    { return m_payerIndex; }
    double getCost() const                       { return m_cost; }
    QString getCostStr() const;
    QString getDescription() const               { return m_description; }
    QList<PersonCheck*> getCoveringList() const  { return m_coveringList; }
    QStringList getAllPeople() const;

    // TODO: const refs?  Others too?
    void setDataCore(DataCoreObject* data);
    void setPayerName(QString payerName);
    void setPayerIndex(int payerIndex);
    void setCost(double cost);
    void setCostStr(QString cost);
    void setDescription(QString description);

    Q_INVOKABLE void load(double cost, QString payer, const QStringList& covering, QString description);
    Q_INVOKABLE void clear();
    QStringList getCoveringStringList() const;
    void setCoveringStringList(const QStringList& stringList);

signals:
    void dataSet() const;
    void payerNameChanged() const;
    void payerIndexChanged() const;
    void costChanged() const;
    void descriptionChanged() const;
    void allPeopleChanged() const;
    void coveringListChanged() const;

private:
    void identifierListChanged();
    void setDefaultPayer();

private:
    // TODO: Some of this data is in the people model, do we want to hook them together?
    QString m_payerName;
    int m_payerIndex = -1;
    double m_cost;
    QString m_description;
    QList<PersonCheck*> m_coveringList;

    // TODO: This information can change from the people model, so we really should
    // store models in here, not the raw data core, that is a fix for the future though
    DataCoreObject* m_data = nullptr;
};

#endif // TRANSACTIONMODEL_H
