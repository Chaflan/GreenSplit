#ifndef DATACORE_H
#define DATACORE_H

#include "BillSplit-Core_global.h"
#include <string>
#include <set>
#include <vector>
#include <unordered_map>

// TODO: Analyze use of set vs uset for names
// TODO: Entry a private member of datacore?
// TODO: Edit transaction covering add, delete
// TODO: Do you need the exports?  I think you don't

struct BILLSPLITCORE_EXPORT Entry
{
    double debt;
    double credit;
};

struct BILLSPLITCORE_EXPORT Transaction
{
    std::string payer;
    double cost;
    std::set<std::string> covering;
};

class BILLSPLITCORE_EXPORT DataCore
{
public:
    std::size_t NumTransactions() const;
    void AddTransaction(std::string payer, double cost, std::set<std::string> covering);
    bool DeleteTransaction(int index);
    bool EditTransactionPayer(int index, std::string newPayer);
    bool EditTransactionCost(int index, double newCost);
    bool EditTransactionCovering(int index, std::set<std::string> newCovering);
    bool EditPerson(const std::string& oldName, std::string newName);
    std::vector<Transaction> Calculate();
    void Clear();

private:
    void reviseLedger(int fromIndex);

    std::vector<Transaction> m_transactions;
    std::vector<std::unordered_map<std::string, Entry> > m_ledger;
};

#endif // DATACORE_H
