#include "datacoreold.h"

DataCoreOld::DataCoreOld(QObject *parent) : QObject(parent)
{
}

bool DataCoreOld::AddPerson(Person person)
{
    if (person.initials.isEmpty() || person.name.isEmpty() ||
            peopleByInitials.find(person.initials) != peopleByInitials.end())
    {
        return false;
    }

    auto newPerson = std::make_shared<Person>(std::move(person));
    peopleByIndex.emplace_back(newPerson);
    peopleById[newPerson->id] = newPerson;
    peopleByInitials[newPerson->initials] = newPerson;
    return true;
}

bool DataCoreOld::DeletePeople(int index, int count)
{
    const int firstIndex = index;
    const int lastIndex = firstIndex + count - 1;
    if (firstIndex < 0 || count < 1 || lastIndex >= NumPeople())
    {
        return false;
    }

    for (; index <= lastIndex; ++index)
    {
        auto personToDelete = peopleByIndex[index];
        peopleById.erase(personToDelete->id);
        peopleByInitials.erase(personToDelete->initials);
    }

    peopleByIndex.erase(peopleByIndex.begin() + firstIndex, peopleByIndex.begin() + lastIndex + 1);
    return true;
}

bool DataCoreOld::EditPerson(int index, const Person& newPerson)
{
    if (index < 0 || index >= NumPeople() ||
            newPerson.initials.isEmpty() || newPerson.name.isEmpty())
    {
        return false;
    }

    auto personToEdit = peopleByIndex[index];
    if (personToEdit->initials != newPerson.initials)
    {
        if (peopleByInitials.find(newPerson.initials) != peopleByInitials.end())
        {
            return false;
        }

        auto nodeHandle = peopleByInitials.extract(personToEdit->initials);
        nodeHandle.key() = newPerson.initials;
        nodeHandle.mapped()->initials = newPerson.initials;
        peopleByInitials.insert(std::move(nodeHandle));
    }

    personToEdit->name = newPerson.name;
    return true;
}

const Person& DataCoreOld::GetPersonByIndex(int index) const
{
    return *peopleByIndex[index];
}

const Person& DataCoreOld::GetPersonByInitials(const QString& initials) const
{
    return *peopleByInitials.find(initials)->second;
}

const Person& DataCoreOld::GetPersonById(int id) const
{
    return *peopleById.find(id)->second;
}

int DataCoreOld::NumPeople() const
{
    return static_cast<int>(peopleByIndex.size());
}

bool DataCoreOld::AddTransaction(Transaction transaction)
{
    if (!HasValidPids(transaction))
    {
        return false;
    }

    auto newTransaction = std::make_shared<Transaction>(std::move(transaction));
    transactionsByIndex.push_back(newTransaction);
    transactionsById[newTransaction->id] = newTransaction;
    return true;
}

bool DataCoreOld::DeleteTransactions(int index, int count)
{
    const int firstIndex = index;
    const int lastIndex = firstIndex + count - 1;
    if (firstIndex < 0 || count < 1 || lastIndex >= NumTransactions())
    {
        return false;
    }

    for (; index <= lastIndex; ++index)
    {
        auto transactionToDelete = transactionsByIndex[index];
        transactionsById.erase(transactionToDelete->id);
    }

    transactionsByIndex.erase(transactionsByIndex.begin() + firstIndex, transactionsByIndex.begin() + lastIndex + 1);
    return true;
}

bool DataCoreOld::EditTransaction(int index, const Transaction& newTransaction)
{
    if (index < 0 || index >= NumTransactions() ||
            !HasValidPids(newTransaction) || newTransaction.description.isEmpty())
    {
        return false;
    }

    *transactionsByIndex[index] = newTransaction;
    return true;
}

const Transaction& DataCoreOld::GetTransactionByIndex(int index) const
{
    return *transactionsByIndex[index];
}

const Transaction& DataCoreOld::GetTransactionById(int id) const
{
    return *transactionsById.find(id)->second;
}

int DataCoreOld::NumTransactions() const
{
    return static_cast<int>(transactionsByIndex.size());
}

void DataCoreOld::ClearPeople()
{
    peopleByIndex.clear();
    peopleById.clear();
    peopleByInitials.clear();

    for (auto& transactionPtr : transactionsByIndex)
    {
        transactionPtr->payerPid = -1;
        transactionPtr->coveringPids.clear();
    }
}

void DataCoreOld::ClearTransactions()
{
    transactionsByIndex.clear();
    transactionsById.clear();
}

void DataCoreOld::Clear()
{
    peopleByIndex.clear();
    peopleById.clear();
    peopleByInitials.clear();
    transactionsByIndex.clear();
    transactionsById.clear();
}

std::vector<Transaction> DataCoreOld::Calculate()
{
    std::vector<Transaction> result;

    // First build a running cost map.  This is a mapping
    // from personid -> debt owed.  Positive means you are owed
    // that value, negative means you owe it.
    std::unordered_map<int, double> runningCostMap;
    for (const auto& person: peopleByIndex)
    {
        runningCostMap[person->id] = 0;
    }

    for (const auto& transaction : transactionsByIndex)
    {
        if (transaction->coveringPids.empty())
        {
            continue;
        }

        runningCostMap[transaction->payerPid] += transaction->cost;

        double sharedCost = transaction->cost / transaction->coveringPids.size();
        for (int pid : transaction->coveringPids)
        {
            runningCostMap[pid] -= sharedCost;
        }
    }

    auto DebtsAreSettled = [&runningCostMap]()->bool
    {
        for(auto [pid, cost] : runningCostMap)
        {
            if (std::abs(cost) >= 0.01)
            {
                return false;
            }
        }

        return true;
    };

    // Now we turn this into a series of transactions to pay off
    // debts
    while(!DebtsAreSettled())
    {
        int gPid = -1;
        int lPid = -1;
        double gCost = std::numeric_limits<double>::lowest();
        double lCost = std::numeric_limits<double>::max();

        for(auto [pid, cost] : runningCostMap)
        {
            if (cost > gCost)
            {
                gPid = pid;
                gCost = cost;
            }
            if (cost < lCost)
            {
                lPid = pid;
                lCost = cost;
            }
        }

        if (!(gPid != lPid && gCost > 0 && lCost < 0))
        {
            assert(0);
        }
        assert(gPid != lPid && gCost > 0 && lCost < 0);

        Transaction debtPayment;
        debtPayment.cost = std::min(gCost, lCost * -1);
        debtPayment.payerPid = lPid;
        debtPayment.coveringPids.insert(gPid);

        runningCostMap[lPid] += debtPayment.cost;
        runningCostMap[gPid] -= debtPayment.cost;

        result.emplace_back(std::move(debtPayment));

        assert(result.size() < 1000);
    }

    return result;
}

bool DataCoreOld::HasValidPids(const Transaction& transaction) const
{
    if (peopleById.find(transaction.payerPid) == peopleById.end())
    {
        return false;
    }

    for (auto pid : transaction.coveringPids)
    {
        if (peopleById.find(pid) == peopleById.end())
        {
            return false;
        }
    }

    return true;
}















