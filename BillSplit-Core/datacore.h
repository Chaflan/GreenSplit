#ifndef DATACORE_H
#define DATACORE_H

#include "algocore.h"
#include <string>
#include <set>
#include <vector>
#include <unordered_map>

// TODO: Analyze use of set vs uset for names
// TODO: Edit transaction covering add, delete

class DataCore
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
    const std::vector<std::tuple<std::string, std::string, double> >& GetResults() const;
    bool EditPerson(const std::string& oldName, const std::string& newName);
    bool PersonExists(const std::string& name) const;
    void Clear();

private:
    void VerifyTransactionIndex(int index) const;
    void ReviseLedger(int fromIndex);
    static bool DebtsCanBeSettled(std::unordered_map<std::string, double> debts);
    void DebugOutputLedgerData() const;
    std::vector<std::tuple<std::string, std::string, double> > Solve() const;

private:

    struct Transaction
    {
        std::string payer;
        double cost;
        std::set<std::string> covering;
    };

    std::vector<Transaction> m_transactions;
    std::vector<std::unordered_map<std::string, double> > m_ledger;
    std::vector<std::tuple<std::string, std::string, double> > m_results;
};

#endif // DATACORE_H
