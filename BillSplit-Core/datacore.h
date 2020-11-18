#ifndef DATACORE_H
#define DATACORE_H

#include "BillSplit-Core_global.h"
#include <string>
#include <set>
#include <vector>
#include <map>
#include <unordered_map>

// TODO: Analyze use of set vs uset for names
// TODO: Entry a private member of datacore?
// TODO: Edit transaction covering add, delete
// TODO: Do you need the exports?  I think you don't
// TODO: Consider how to handle errors at this level.  Perhaps throw exceptions?

struct BILLSPLITCORE_EXPORT Entry
{
    // TODO: I don't think you use this anymore
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
    const std::vector<std::tuple<std::string, std::string, double> >& GetResults() const;
    bool EditPerson(const std::string& oldName, std::string newName);
    bool PersonExists(const std::string& name) const;  // necessary?
    void Clear();

private:
    void VerifyTransactionIndex(int index) const;

    std::vector<Transaction> m_transactions;
    std::vector<std::unordered_map<std::string, double> > m_ledger;
    std::vector<std::tuple<std::string, std::string, double> > m_results;

    // Main Algorithms
    void ReviseLedger(int fromIndex);
    static bool DebtsCanBeSettled(std::unordered_map<std::string, double> debts);
    void DebugOutputLedgerData() const;
    std::vector<std::tuple<std::string, std::string, double> > Calculate() const;

    // SettleMinMax Algorithm
    //
    // Greedy algorithm, fast but non-optimal for some cases.
    // These cases will be ones where a subset of the payments can be solved in isolation.
    // These could be common with larger sets, whole number prices, and/or a large margin
    //      n = debts.size()
    //      Time -> O(n)
    //      Space (solution vector) -> O(n)
    //      Space (other required to solve) -> O(1)
    static std::vector<std::tuple<std::string, std::string, double> >
        SettleMinMax(std::unordered_map<std::string, double> debts);

    // TODO: Namespace?
    // Settle Tree Algorithm and Related Code
    //
    // Brute force recursive tree algorithm, slow but optimal
    // TODO: Note about how solution requires solution to the ____ problem which is open
    std::vector<std::tuple<std::string, std::string, double> >
        SettleTree(std::unordered_map<std::string, double> debts) const;

    struct ITransaction {
        std::size_t from;
        std::size_t to;
        double cost;
    };

    static constexpr double mst_pmargin = 0.01;
    static constexpr double mst_nmargin = mst_pmargin * -1;
    mutable int mst_solnNumT = 0;
    mutable std::vector<ITransaction> mst_finalSoln;
    mutable std::vector<ITransaction> mst_currSoln;

    bool SettleTreeRecurse(
        std::vector<double> pset,
        std::size_t pcount,
        std::vector<double> nset,
        std::size_t ncount,
        int numT) const;
    /////////////
};

#endif // DATACORE_H
