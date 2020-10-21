#ifndef DATACORE_H
#define DATACORE_H

#include "BillSplit-Core_global.h"
#include "person.h"
#include "transactionold.h"

#include <memory>
#include <vector>
#include <unordered_map>

//--------------------------------
// QT Stuff to remove in the future
#include <QString>
#include <QHash>
#include <QObject>
//--------------------------------

class BILLSPLITCORE_EXPORT DataCoreOld : public QObject
{
    Q_OBJECT
public:
    explicit DataCoreOld(QObject *parent = nullptr);
    virtual ~DataCoreOld() {};  // to fix vtable error

    bool AddPerson(Person person);
    bool DeletePeople(int index, int count = 1);
    bool EditPerson(int index, const Person& newPerson);
    const Person& GetPersonByIndex(int index) const;
    const Person& GetPersonByInitials(const QString& initials) const;
    const Person& GetPersonById(int id) const;
    int NumPeople() const;

    bool AddTransaction(TransactionOld transaction);
    bool DeleteTransactions(int index, int count = 1);
    bool EditTransaction(int index, const TransactionOld& newTransaction);
    const TransactionOld& GetTransactionByIndex(int index) const;
    const TransactionOld& GetTransactionById(int id) const;
    int NumTransactions() const;

    void ClearPeople();
    void ClearTransactions();
    void Clear();

    std::vector<TransactionOld> Calculate();

private:
    bool HasValidPids(const TransactionOld& transaction) const;

private:
    std::vector<std::shared_ptr<Person> > peopleByIndex;
    std::unordered_map<QString, std::shared_ptr<Person> > peopleByInitials;
    std::unordered_map<int, std::shared_ptr<Person> > peopleById;

    std::vector<std::shared_ptr<TransactionOld> > transactionsByIndex;
    std::unordered_map<int, std::shared_ptr<TransactionOld> > transactionsById;
};

#endif // DATACORE_H

// Threadsafe with readlock vs writelock

// This works for your model but it is incomplete as a data model.  There is no way to delete
// a person by id/initials without linear searching by index.  However this is what you would do
// unless you added an index lookup with a dirty flag, which is what you should do but its a lot
// of unnecessary complexity.
// id->index lookup.  On delete look at every person after deleted index grab id, update
// id->index lookup to suit.

// Person and Transaction managing index counter

// Exposing Person and Transaction vs passing in arguments.

// Edit methods are wasteful

// Clear people vs clear trnasactions
