#ifndef DATACORE_H
#define DATACORE_H

#include "algocore.h"
#include <string>
#include <set>
#include <vector>
#include <unordered_map>

//---------------------------------------------------------------------------------------
// A pure C++ class for calculating the bill splitting.  DataCore will store the data
// and reduce the problem to a series of debts and credits for each person involved.
// This is then passed to the AlgoCore for processing the final step of solving
// in the fewest steps.  The series of debts and credits is stored in a ledger.
//
// bool return values of true generally mean the edit/action took place.  false means
// no error, but nothing was changed.  Improper use results in exceptions.
//---------------------------------------------------------------------------------------
class DataCore
{
public:
    std::size_t NumTransactions() const noexcept;
    void AddTransaction(std::string payer, double cost, std::set<std::string> covering);
    bool DeleteTransactions(int index, int count);

    bool EditTransactionPayer(int index, std::string newPayer);
    bool EditTransactionCost(int index, double newCost);
    bool EditTransactionCovering(int index, std::set<std::string> newCovering);

    const std::string& GetTransactionPayer(int index) const;
    double GetTransactionCost(int index) const;
    const std::set<std::string>& GetTransactionCovering(int index) const;

    bool PersonExists(const std::string& name) noexcept;
    bool EditPerson(const std::string& oldName, const std::string& newName) noexcept;

    //---------------------------------------------------------------------------------------
    // Output (return value): A sequence of payments/transactions to be made to solve the
    //      problem.  Order is arbitrary. Tuple elements are <from, to, amount>.
    //      e.g. <"Mary", "Tom", 50> is a transaction where Mary pays Tom 50 units of currency.
    //---------------------------------------------------------------------------------------
    const std::vector<std::tuple<std::string, std::string, double> >& GetResults();

    //---------------------------------------------------------------------------------------
    // These update methods allow you to control the time of calculation.
    // A call to GetResults will update ledger, and then update results before returning
    // results.  You could optionally (for example) update ledger each time a transaction
    // is added or altered to partially precalculate for the results.
    // They return true if an update took place.
    //---------------------------------------------------------------------------------------
    bool UpdateLedger() noexcept;
    bool UpdateResults();

    void Clear() noexcept;

private:
    void VerifyTransactionIndex(int index) const;
    void SetLedgerRevisionIndex(std::size_t newIndex) noexcept;
    std::vector<std::tuple<std::string, std::string, double> > Solve();

private:

    struct Transaction
    {
        std::string payer;
        double cost;
        std::set<std::string> covering;
    };

    AlgoCore m_algoCore;
    std::vector<Transaction> m_transactions;

    // Mapping of names to credits/debts that reflects the state of the problem in relation to each transaction.
    std::vector<std::unordered_map<std::string, double> > m_ledger;
    std::size_t m_ledgerRevisionIndex = 0;  // Index at which ledger is invalid due to changes

    std::vector<std::tuple<std::string, std::string, double> > m_results;
    bool m_resultsDirty = true;
};

#endif // DATACORE_H
