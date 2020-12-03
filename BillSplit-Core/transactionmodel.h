#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QObject>
#include "billsplit-core_global.h"
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
    bool getCheckStatus() const  { return m_checkStatus; }

    void setName(QString name);
    void setCheckStatus(bool checkStatus);

signals:
    void nameChanged() const;
    void checkStatusChanged() const;

private:
    QString m_name;
    bool m_checkStatus;
};

//------------------------------------------------------------------------------------------
// TransactionModel
//
// This class is used to model a single transaction for the view or add transaction window
// popup.  It's use is very different between the QML and Widget views.  For QML we load or
// loadDefault and cached data is bound to the view using qt properties.  For widgets we simply load
// model data to the view on popup execution, and save view data to the model on popup save.
// The QML solution is unnecessarily complex as the view window is modal, so binding user changes
// that later get changed again is a waste of resources.  It also adds a lot of code.  I did it
// this way partially to learn how this is done, and partially because in the future this
// model may be useful for some changes I have in mind.  This comment is here to explain why
// this model seems to be doing a lot more than it needs to.
//-------------------------------------------------------------------------------------------
class BILLSPLITCORE_EXPORT TransactionModel : public QObject
{
    Q_OBJECT

    // ReadWrite properties
    Q_PROPERTY(DataCoreObject* data READ getDataCore WRITE setDataCore NOTIFY dataSet)
    Q_PROPERTY(int payerIndex READ getPayerIndex WRITE setPayerIndex NOTIFY payerIndexChanged)
    Q_PROPERTY(double costDbl READ getCostDbl WRITE setCostDbl NOTIFY costChanged)
    Q_PROPERTY(QString costStr READ getCostStr WRITE setCostStr NOTIFY costChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged USER true)

    // Readonly properties
    Q_PROPERTY(QStringList allPeople READ getAllPeople NOTIFY allPeopleChanged)
    Q_PROPERTY(QList<PersonCheck*> coveringList READ getCoveringList NOTIFY coveringListChanged)

public:
    explicit TransactionModel(QObject *parent = nullptr);

    DataCoreObject* getDataCore() const          { return m_data; }
    int getPayerIndex() const                    { return m_payerIndex; }
    double getCostDbl() const                    { return m_costDbl; }
    QString getCostStr() const                   { return m_costStr; }
    QString getDescription() const               { return m_description; }
    QList<PersonCheck*> getCoveringList() const  { return m_coveringList; }
    QStringList getAllPeople() const             { return m_allPeople; }

    void setDataCore(DataCoreObject* data);
    void setPayerIndex(int payerIndex);
    void setCostDbl(double cost);
    void setCostStr(const QString& cost);
    void setDescription(const QString& description);

    // Functions interacting with TransactionsModel
    Q_INVOKABLE void load(double cost, const QString& payer, const QStringList& covering, const QString& description);
    Q_INVOKABLE void loadDefault();
    QString getPayerName() const;
    QStringList getCoveringStringList() const;

    // Functions interacting with only WidgetsView
    void setCheck(int index, bool check);

signals:
    void dataSet() const;
    void payerIndexChanged() const;
    void costChanged() const;
    void descriptionChanged() const;
    void allPeopleChanged() const;
    void coveringListChanged() const;

private:
    int m_payerIndex;
    double m_costDbl;
    QString m_costStr;
    QString m_description;
    QStringList m_allPeople;
    QList<PersonCheck*> m_coveringList;

    DataCoreObject* m_data = nullptr;
};

#endif // TRANSACTIONMODEL_H
