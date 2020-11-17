#include "datacore.h"

// TODO: add noexcepts
// TODO: Remove this
#include <iostream>
#include <limits>

std::size_t DataCore::NumTransactions() const
{
    return m_transactions.size();
}

void DataCore::AddTransaction(std::string payer, double cost, std::set<std::string> covering)
{
    if (covering.empty()) {
        throw std::invalid_argument("Transaction covering set must not be empty");
    }

    m_transactions.push_back({std::move(payer), cost, std::move(covering)});
    ReviseLedger(m_transactions.size() - 1);
}

void DataCore::DeleteTransactions(int index, int count)
{
    if (index < 0) {
        throw std::out_of_range("DeleteTransaction index is negative: " + std::to_string(index));
    }
    if (count < 0) {
        throw std::invalid_argument("DeleteTransaction count is negative: " + std::to_string(count));
    }
    if (index + count > static_cast<int>(NumTransactions())) {
        throw std::out_of_range("DeleteTransaction index + count = " + std::to_string(index + count) +
            " exceeds NumTransactions = " + std::to_string(NumTransactions()));
    }

    // TODO: ok if count == 0
    m_transactions.erase(m_transactions.begin() + index, m_transactions.begin() + index + count);
    ReviseLedger(index + 1);
}

void DataCore::EditTransactionPayer(int index, std::string newPayer)
{
    VerifyTransactionIndex(index);
    if (newPayer != m_transactions[index].payer) {
        m_transactions[index].payer = std::move(newPayer);
        ReviseLedger(index);
    }
}

void DataCore::EditTransactionCost(int index, double newCost)
{
    VerifyTransactionIndex(index);
    if (newCost != m_transactions[index].cost) {
        m_transactions[index].cost = newCost;
        ReviseLedger(index);
    }
}

void DataCore::EditTransactionCovering(int index, std::set<std::string> newCovering)
{
    VerifyTransactionIndex(index);
    if (newCovering.empty()) {
        throw std::invalid_argument("Transaction covering set must not be empty");
    }
    if (newCovering != m_transactions[index].covering) {
        m_transactions[index].covering = std::move(newCovering);
        ReviseLedger(index);
    }
}

const std::string& DataCore::GetTransactionPayer(int index) const
{
    VerifyTransactionIndex(index);
    return m_transactions[index].payer;
}

double DataCore::GetTransactionCost(int index) const
{
    VerifyTransactionIndex(index);
    return m_transactions[index].cost;
}

const std::set<std::string> DataCore::GetTransactionCovering(int index) const
{
    VerifyTransactionIndex(index);
    return m_transactions[index].covering;
}

const std::vector<std::tuple<std::string, std::string, double> >& DataCore::GetResults() const
{
    return m_results;
}

bool DataCore::PersonExists(const std::string& name) const
{
    if (m_ledger.empty()) {
        return false;
    }

    const auto& lastLedgerEntry = m_ledger[m_ledger.size() - 1];
    return lastLedgerEntry.find(name) != lastLedgerEntry.end();
}

void DataCore::Clear()
{
    m_transactions.clear();
    m_results.clear();
    m_ledger.clear();  // TODO: ledger index?
}

void DataCore::VerifyTransactionIndex(int index) const
{
    if (index < 0 || index >= static_cast<int>(NumTransactions())) {
        throw std::out_of_range("Transaction index " + std::to_string(index) +
            " is out of range: [0," + std::to_string(NumTransactions() + 1) + "]");
    }
}

void DataCore::ReviseLedger(int fromIndex)
{
    if (fromIndex < 0) {
        throw std::invalid_argument("ReviseLedger fromIndex is negative");
    }

    bool ledgerChanged = false;

    // TODO: Consider just using an index to invalidate instead of consciously deleting
    if (m_ledger.size() > m_transactions.size()) {
        m_ledger.erase(m_ledger.begin() + m_transactions.size(), m_ledger.end());
        ledgerChanged = true;
    }

    for (int i = fromIndex; i < static_cast<int>(m_transactions.size()); ++i) {

        // Copy the previous ledger entry, or if there is none, start fresh
        if (i == 0) {
            if (m_ledger.empty()) {
                m_ledger.emplace_back();
            } else {
                m_ledger[0].clear();
            }
        } else {
            if (static_cast<int>(m_ledger.size()) <= i) {
                m_ledger.emplace_back(m_ledger[i - 1]);
            } else {
                m_ledger[i] = m_ledger[i - 1];
            }
        }

        const Transaction& currT = m_transactions[i];
        const double debt = currT.cost / currT.covering.size();

        m_ledger[i][currT.payer] += currT.cost;
        for (const auto& name : currT.covering) {
            m_ledger[i][name] -= debt;
        }

        ledgerChanged = true;
    }

    if (ledgerChanged) {
        m_results = Calculate();
    }
}

std::vector<std::tuple<std::string, std::string, double> >
    DataCore::SettleMinMax(std::unordered_map<std::string, double> debts) // TODO: currently by value
{
    std::vector<std::tuple<std::string, std::string, double> > res;
    if (DebtsCanBeSettled(debts)) {

        auto DebtsAreSettled = [&debts]()->bool {
            for(auto [name, cost] : debts) {
                if (std::abs(cost) >= 0.01) {
                    return false;
                }
            }
            return true;
        };

        // Now we turn this into a series of transactions to pay off debts
        while(!DebtsAreSettled()) {
            // TODO: References here would be faster
            std::string gName;
            std::string lName;
            double gCost = std::numeric_limits<double>::lowest();
            double lCost = std::numeric_limits<double>::max();

            for(auto [name, cost] : debts) {
                if (cost > gCost) {
                    gName = name;
                    gCost = cost;
                }
                if (cost < lCost) {
                    lName = name;
                    lCost = cost;
                }
            }

            double currCost = std::min(gCost, lCost * -1);
            debts[lName] += currCost;
            debts[gName] -= currCost;

            res.emplace_back(std::move(lName), std::move(gName), currCost);
            assert(res.size() < 1000);
        }
    }

    return res;
}

std::vector<std::tuple<std::string, std::string, double> >
    DataCore::SettleTree(std::unordered_map<std::string, double> debts)
{
    std::vector<std::tuple<std::string, std::string, double> > res;
    if (DebtsCanBeSettled(debts)) {

        const double margin = 0.01;

        // Sets of positive and negative costs
        std::vector<double> pset;
        std::vector<double> nset;

        // Indexing system, map index of vector to person's name, this saves size
        std::unordered_map<std::size_t, std::string> plookup;
        std::unordered_map<std::size_t, std::string> nlookup;

        for (const auto& [name, cost] : debts) {
            if (cost < margin * -1) {
                nset.push_back(cost);
                nlookup[nset.size()] = name;
            } else if (cost > margin) {
                pset.push_back(cost);
                plookup[pset.size()] = name;
            }
        }

        pset.shrink_to_fit();
        nset.shrink_to_fit();

        int solutionNumT = std::numeric_limits<int>::max();
        std::vector<ITransaction> tempSolution;
        std::vector<ITransaction> finalSolution;

        SettleTreeRecurse(pset, nset, 0, tempSolution, solutionNumT, finalSolution, 0.01);



    }

    return res;
}

bool DataCore::SettleTreeRecurse(
    std::vector<double> pset,
    std::vector<double> nset,
    int numT,
    std::vector<DataCore::ITransaction>& thisSoln,
    int& solnNumT,
    std::vector<DataCore::ITransaction>& finalSoln,
    double margin)
{
    //////////// Debug output
    for (int i = 0; i < numT; ++i) { std::cout << '\t'; }
    std::cout << "pset{";
    bool needsComma = false;
    for (const auto& x : pset) {
        if (needsComma) { std::cout << ","; }
        else { needsComma = true; }
        std::cout << x;
    }
    std::cout << "}, nset{";
    needsComma = false;
    for (const auto& x : nset) {
        if (needsComma) { std::cout << ","; }
        else { needsComma = true; }
        std::cout << x;
    }
    std::cout << "}, numT=" << numT;
    ///////////////////////////

    const auto AddSolution = [&thisSoln, numT] (std::size_t fr, std::size_t to, double payment) {
        if (static_cast<int>(thisSoln.size()) <= numT) {
            thisSoln.emplace_back(ITransaction{fr, to, payment});
        } else {
            thisSoln[numT].from = fr;
            thisSoln[numT].to = to;
            thisSoln[numT].cost = payment;
        }
    };

    if (numT >= solnNumT) {
        return false;
    }

    bool solutionFound = true;
    for (std::size_t p = 0; p < pset.size(); ++p) {
        if (pset[p] < margin) {
            continue;
        }
        solutionFound = false;

        for (std::size_t n = 0; n < nset.size(); ++n) {
            if (nset[n] < margin * -1) {
                continue;
            }

            // Check below, was drunk when wrote
            // Calcs need margin in them yo
            double remainder = p + n;
            std::vector<double> nset_copy{nset};
            std::vector<double> pset_copy{pset};

            if (remainder > margin) {
                AddSolution(n, p, pset[p] - remainder);
                nset_copy[n] = 0;
                pset_copy[p] = remainder;
            } else if (remainder < margin * -1) {
                AddSolution(n, p, -1 * (nset[n] + remainder));
                nset_copy[n] = remainder;
                pset_copy[p] = 0;
            } else {
                AddSolution(n, p, -1 * n - remainder);
                nset_copy[n] = 0;
                pset_copy[p] = 0;
            }
        }
    }

    if (solutionFound) {
        solnNumT = numT;
        finalSoln = thisSoln;

        /////// Debug output
        std::cout << " SOLN=";
        for (const auto& x : finalSoln) {
            std::cout << '{' << x.from << ',' << x.to << ',' << x.cost << "},";
        }
        ////////////////////

        return true;
    }

    return false;
}

bool DataCore::DebtsCanBeSettled(std::unordered_map<std::string, double> debts)
{
    double sum = 0;
    for (const auto& [ name, debt ]  : debts) {
        sum += debt;
    }
    return sum < 0.01;
}


// from, to, cost
std::vector<std::tuple<std::string, std::string, double> > DataCore::Calculate() const
{
    // TODO: Remove
    DebugOutputLedgerData();

    if (m_ledger.empty()) {
        return {};
    }

    return SettleMinMax(m_ledger[m_ledger.size() - 1]);
}

void DataCore::DebugOutputLedgerData() const
{
    for (const auto& ledger : m_ledger) {
        bool needsComma = false;
        for (const auto& entry : ledger) {
            if (needsComma) {
                std::cout << ", ";
            } else {
                needsComma = true;
            }
            std::cout << entry.first << ":" << entry.second;
        }
        std::cout << std::endl;
    }
}

