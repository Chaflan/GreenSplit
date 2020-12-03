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
    void AddTransaction(std::string payer, double cost, std::set<std::string> covering); //
    bool DeleteTransactions(int index, int count); //
    bool EditTransactionPayer(int index, std::string newPayer); //
    bool EditTransactionCost(int index, double newCost); //
    bool EditTransactionCovering(int index, std::set<std::string> newCovering);
    const std::string& GetTransactionPayer(int index) const;
    double GetTransactionCost(int index) const;
    const std::set<std::string>& GetTransactionCovering(int index) const;
    const std::vector<std::tuple<std::string, std::string, double> >& GetResults() const; // start here
    bool EditPerson(const std::string& oldName, const std::string& newName); //
    bool PersonExists(const std::string& name) const;
    void Clear();

    bool UpdateLedger() const;
    bool UpdateResults() const;

private:
    void VerifyTransactionIndex(int index) const;
    void SetLedgerRevisionIndex(std::size_t newIndex) const;
    static bool DebtsCanBeSettled(std::unordered_map<std::string, double> debts);  // TODO: move to algo core
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

    // Cached data to speed calculations
    mutable std::vector<std::unordered_map<std::string, double> > m_ledger;
    mutable std::size_t m_ledgerRevisionIndex = 0;
    mutable std::vector<std::tuple<std::string, std::string, double> > m_results;
    mutable bool m_resultsDirty = true;
};

#endif // DATACORE_H
