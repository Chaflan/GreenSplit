#ifndef DATACOREOBJECT_H
#define DATACOREOBJECT_H

#include <QObject>
#include <QMap>
#include "billsplit-core_global.h"
#include "datacore.h"

// TODO: noexcept
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
    virtual ~DataCoreObject();

    int numTransactions() const;
    bool addTransaction(double cost, const QString& payer, const QStringList& covering, QString description, bool silent = false);
    bool deleteTransactions(int index, int count);
    QString getTransactionPayer(int index) const; // Cache this and return const &
    double getTransactionCost(int index) const;
    const QString& getTransactionDescription(int index) const;
    QStringList getTransactionCovering(int index) const; // TODO: Stringlist?  Cache this?
    bool editTransactionPayer(int index, const QString& newPayer);
    bool editTransactionCost(int index, double newCost);
    bool editTransactionDescription(int index, const QString& newDescription);
    bool editTransactionCovering(int index, const QStringList& newCovering);

    Q_INVOKABLE int numPeople() const;
    bool personExists(const QString& identifier) const;
    bool addPerson(QString identifier, QString name);
    bool removePeople(int index, int count);
    const QString& getPersonIdentifier(int index) const;
    const QString& getPersonName(int index) const;
    bool editPersonIdentifier(int index, const QString& newIdentifier);
    bool editPersonName(int index, QString newName);

    int numResults() const;
    QString getResultDebtorId(int index) const;
    QString getResultDebtorName(int index) const;
    QString getResultCreditorId(int index) const;
    QString getResultCreditorName(int index) const;
    double getResultCost(int index) const;

    Q_INVOKABLE void clear();

    // Save and Load methods
    Q_INVOKABLE bool jsonRead(const QString& filePath = "autosave.json");
    Q_INVOKABLE bool jsonWrite(const QString& filePath = "autosave.json") const;
    Q_INVOKABLE bool jsonRead(const QUrl& filePath);
    Q_INVOKABLE bool jsonWrite(const QUrl& filePath) const;
    void jsonRead(const QJsonObject& jsonObj);
    void jsonWrite(QJsonObject& jsonObj) const;
    Q_INVOKABLE QStringList getLocalSaveFiles() const;

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
    //
    // Note that QML and Widgets treat model reloads differently.  EG: changing an identifier from people view will send
    // a transactionsChanged signal.  QML will immediately reload all transactions and call data to get all of them for the
    // table.  When you flip to the transactions tab the new ones are already loaded and no further loads occur.
    // Widgets instead will not reload on signal, but always loads the data with the data call when flipping to the
    // transactions tab.  Therefore the signal is not needed for widgets, but it also does no harm.
    //--------------------------------------------------------------------------------------------------------------------
    void peopleChanged() const;
    void transactionsChanged() const;
    void resultsChanged() const;

private:
    static QString addJsonExtension(QString path);
    bool personInTransactions(const QString& identifier) const;
    static std::set<std::string> stringListToStdSet(const QStringList& stringList);
    bool isNewIdentifierValid(const QString& identifier) const;

private:
    // Data kept in addition to any in the data core to facilitate a clean user experience
    QStringList m_identifierList;
    QStringList m_nameList;
    QStringList m_descriptionsList;

    // Maps identifier to its index in the identifierList, which is also the
    // index of the corresponding name in nameList
    QMap<QString, int> m_identifierIndexLookup;

    DataCore m_data;
};

#endif // DATACOREOBJECT_H
