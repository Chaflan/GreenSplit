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
// TODO: Consider how to handle errors at this level.  Perhaps throw exceptions?

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
    void DeleteTransactions(int index, int count);
    void EditTransactionPayer(int index, std::string newPayer);
    void EditTransactionCost(int index, double newCost);
    void EditTransactionCovering(int index, std::set<std::string> newCovering);
    const std::string& GetTransactionPayer(int index) const;
    double GetTransactionCost(int index) const;
    const std::set<std::string> GetTransactionCovering(int index) const;
    bool EditPerson(const std::string& oldName, std::string newName);
    bool PersonExists(const std::string& name) const;  // necessary?
    std::vector<Transaction> Calculate();
    void Clear();

private:
    void VerifyTransactionIndex(int index) const;
    void ReviseLedger(int fromIndex);

    std::vector<Transaction> m_transactions;
    std::vector<std::unordered_map<std::string, double> > m_ledger;
};

#endif // DATACORE_H
