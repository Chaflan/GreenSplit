#ifndef DATACOREOBJECT_H
#define DATACOREOBJECT_H

#include <QObject>
#include "billsplit-core_global.h"
#include "datacore.h"

// TODO: Standardize use of name vs identifier
// TODO: QDebug lines
// TODO: Upper Lower case method name confusion (all lower in any qobject for qinvokableness)
// TODO: Return whole list in some cases?
// TODO: Currently there is nothing preventing the front end from adding a transaction with nonexistant people
// TODO: Braced initialization

class BILLSPLITCORE_EXPORT DataCoreObject : public QObject
{
    Q_OBJECT
public:
    explicit DataCoreObject(QObject *parent = nullptr);

    // DataCore wrapper methods
    int numTransactions() const;
    bool addTransaction(double cost, const QString& payer, const QStringList& covering, QString description);
    bool deleteTransactions(int index, int count);
    bool editTransactionPayer(int index, const QString& newPayer);
    bool editTransactionCost(int index, double newCost);
    bool editTransactionCovering(int index, const QStringList& newCovering);
    QString getTransactionPayer(int index) const; // Cache this and return const &
    double getTransactionCost(int index) const;
    QStringList getTransactionCovering(int index) const; // TODO: Stringlist?  Cache this?
    bool editPerson(const QString& oldName, const QString& newName);
    void clear();

    // DataCore extending methods
    Q_INVOKABLE int numPeople() const;
    bool personExists(const QString& identifier) const;
    bool personInTransactions(const QString& identifier) const;
    bool addPerson(QString identifier, QString name);
    bool removePeople(int index, int count);
    const QString& getPersonIdentifier(int index) const;
    const QString& getPersonName(int index) const;
    const QString& getTransactionDescription(int index) const;
    QString getResultDebtor(int index) const;
    QString getResultCreditor(int index) const;
    double getResultCost(int index) const;
    int numResults() const;
    bool editTransactionDescription(int index, QString newDescription);   // It might be that the fail case is more common meaning we want const ref here
    bool editPersonIdentifier(int index, const QString& newIdentifier);
    bool editPersonName(int index, QString newName);
    const QStringList& getIdentifierList() const;

    // Save and Load methods.
    // TODO: Move these to their own class, or use a database
    bool jsonRead(const QString& filePath = "save.json");
    bool jsonWrite(const QString& filePath = "save.json") const;
    Q_INVOKABLE bool jsonRead(const QUrl& filePath);
    Q_INVOKABLE bool jsonWrite(const QUrl& filePath) const;
    void jsonRead(const QJsonObject& jsonObj);
    void jsonWrite(QJsonObject& jsonObj) const;

    virtual ~DataCoreObject();

signals:
    void signalError(QString error) const;

    //--------------------------------------------------------------------------------------------------------------------
    // Signal system
    //
    // This signal system is adapted to this particular design and a little bit stiff.  The burden of deciding how signals
    // are handled by submodels (peopleModel, transactionsModel, etc) is on this dataCoreObject class which is backwards.
    // I did this to keep things simple.  A problem we want to avoid is a submodel that sends a signal it also needs to
    // handle, and another model also sends that signal.  We would have to listen for that signal and also send that signal
    // resulting in a loop.  Instead dataCoreObject only sends a signal when it is an action performed by one submodel,
    // that a different submodel needs to react to.  A more adaptable system would have a signaller enum and just ignore
    // any signal that came from itself.  The burden would then be on the submodels to deal with the signals, but this has
    // a lot of extra code and is clunky.
    //--------------------------------------------------------------------------------------------------------------------
    void peopleChanged() const;
    void transactionsChanged() const;
    void resultsChanged() const;

private:
    std::set<std::string> stringListToStdSet(const QStringList& stringList);

private:
    QStringList m_identifierList;
    QStringList m_nameList;
    QStringList m_descriptionsList;
    DataCore m_data;
};

#endif // DATACOREOBJECT_H
