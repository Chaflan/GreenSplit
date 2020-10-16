#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QObject>
#include "BillSplit-Core_global.h"
#include "datacore.h"

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
    // TODO: Try MEMBER with a NOTIFY only called by non-setters
    Q_PROPERTY(QString payerName READ getPayerName WRITE setPayerName NOTIFY payerNameChanged)
    Q_PROPERTY(int payerIndex READ getPayerIndex WRITE setPayerIndex NOTIFY payerIndexChanged)
    Q_PROPERTY(double cost READ getCost WRITE setCost NOTIFY costChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)

    // TODO: Readonly?
    Q_PROPERTY(QStringList allPeople READ getAllPeople NOTIFY allPeopleChanged)
    Q_PROPERTY(QList<PersonCheck*> coveringList READ getCoveringList NOTIFY coveringListChanged)

public:
    explicit TransactionModel(QObject *parent = nullptr);
    //TransactionModel(DataCore& dataCore, QObject* parent = nullptr);

    Q_INVOKABLE void initialize(DataCore* data);

    QString getPayerName() const                 { return m_payerName; }
    int getPayerIndex() const                    { return m_payerIndex; }
    double getCost() const                       { return m_cost; }
    QString getDescription() const               { return m_description; }
    QStringList getAllPeople() const             { return m_allPeople; }
    QList<PersonCheck*> getCoveringList() const  { return m_coveringList; }

    void setPayerName(QString payerName);
    void setPayerIndex(int payerIndex);
    void setCost(double cost);
    void setDescription(QString description);

    Q_INVOKABLE void load(QString name, double cost, QString description, const QStringList& covering);
    Q_INVOKABLE void clear();
    QStringList getCoveringStringList() const;
    void setCoveringStringList(const QStringList& stringList);

signals:
    void payerNameChanged() const;
    void payerIndexChanged() const;
    void costChanged() const;
    void descriptionChanged() const;
    void allPeopleChanged() const;
    void coveringListChanged() const;

private:
    // TODO: Some of this data is in the people model, do we want to hook them together?
    QString m_payerName;
    int m_payerIndex;
    double m_cost;
    QString m_description;
    QStringList m_allPeople;
    QList<PersonCheck*> m_coveringList;

    // TODO: This information can change from the people model, so we really should
    // store models in here, not the raw data core, that is a fix for the future though
    DataCore* m_data = nullptr;
    //DataCore& m_data;
};

#endif // TRANSACTIONMODEL_H
