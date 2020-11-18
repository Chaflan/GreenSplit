#include "datacore.h"
#include <algorithm>

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

// Assumes debts can be settled
std::vector<std::tuple<std::string, std::string, double> >
    DataCore::SettleMinMax(std::unordered_map<std::string, double> debts) // TODO: currently by value
{
    std::vector<std::tuple<std::string, std::string, double> > res;

    auto DebtsAreSettled = [&debts]()->bool {
        for(const auto& [name, cost] : debts) {
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

    return res;
}

// TODO: Undo this: Assumes debts can be settled
// TODO: Consider long term resizing and switching back and forth between large solutions and small ones
std::vector<std::tuple<std::string, std::string, double> >
    DataCore::SettleTree(std::unordered_map<std::string, double> debts) const
{
    // Start with the minmax solution
    std::vector<std::tuple<std::string, std::string, double> > res = SettleMinMax(debts);

    // Now break it into components to make it faster to solve

    // Sets of positive and negative costs in the debts object, prime the first ones
    if (mst_psets.empty()) {
        mst_psets.emplace_back();
    } else {
        mst_psets[0].clear();
    }

    if (mst_nsets.empty()) {
        mst_nsets.emplace_back();
    } else {
        mst_nsets[0].clear();
    }

    // Indexing system, map index of vector to person's name
    std::vector<std::string> plookup;
    std::vector<std::string> nlookup;

    // Reverse lookup is only needed for converting settle minmax soln to an indexed one.
    std::unordered_map<std::string, std::size_t> prevlookup;
    std::unordered_map<std::string, std::size_t> nrevlookup;

    for (const auto& [name, cost] : debts) {
        if (cost < mst_nmargin) {
            nrevlookup[name] = nlookup.size();
            nlookup.push_back(name);
            mst_nsets[0].push_back(cost);
        } else if (cost > mst_pmargin) {
            prevlookup[name] = plookup.size();
            plookup.push_back(name);
            mst_psets[0].push_back(cost);
        }
    }

    // Prime the final solution using the SettleMinMax solution
    mst_finalSoln.clear();
    for (const auto& fromToCost : res) {
        mst_finalSoln.push_back({
            nrevlookup[std::get<0>(fromToCost)],
            prevlookup[std::get<1>(fromToCost)],
            std::get<2>(fromToCost)
            });
    }
    mst_solnNumT = static_cast<int>(mst_finalSoln.size());
    mst_currSoln.resize(mst_solnNumT - 1);  // TODO: are you sure -1?
    int originalsolutionNumT = mst_solnNumT;

    // Preallocate for the algorithm
    mst_psets.resize(mst_solnNumT);
    mst_nsets.resize(mst_solnNumT);

    SettleTreeRecurse(mst_psets[0].size(), mst_nsets[0].size(), 0);

    // If this didn't change, then the final solution is the result from SettleMinMax and we can just return it.
    if (originalsolutionNumT != mst_solnNumT) {
        res.clear();
        // TODO: Shouldn't need double check
        for (int i = 0; i < mst_solnNumT && i < static_cast<int>(mst_finalSoln.size()); ++i) {
            const auto& iTrans = mst_finalSoln[i];
            res.emplace_back(nlookup[iTrans.from], plookup[iTrans.to], iTrans.cost);
        }
    }

    return res;
}

// return value true means you need to recheck your base case
// comment should explain how numT acts as an index
bool DataCore::SettleTreeRecurse(
    std::size_t pcount,
    std::size_t ncount,
    int numT) const
{
    const auto& currPset = mst_psets[numT];
    const auto& currNset = mst_nsets[numT];

    //////////// Debug output
    std::cout << std::endl;
    for (int i = 0; i < numT; ++i) { std::cout << '\t'; }
    std::cout << "pset{";
    bool needsComma = false;
    for (const auto& x : currPset) {
        if (needsComma) { std::cout << ","; }
        else { needsComma = true; }
        std::cout << x;
    }
    std::cout << "}, nset{";
    needsComma = false;
    for (const auto& x : currNset) {
        if (needsComma) { std::cout << ","; }
        else { needsComma = true; }
        std::cout << x;
    }
    std::cout << "}, numT=" << numT << std::flush;
    ///////////////////////////

    if (pcount == 0 && ncount == 0) {
        // Base case: Solution found
        mst_solnNumT = numT;
        mst_finalSoln = mst_currSoln;

        /////// Debug output
        std::cout << " SOLN=";
        int i = 0;
        for (const auto& x : mst_finalSoln) {
            std::cout << (i >= mst_solnNumT ? "ignore" : "");
            std::cout << '{' << x.from << ',' << x.to << ',' << x.cost << "},";
            i++;
        }
        ////////////////////

        return true;
    }

    if (static_cast<int>(std::max(pcount, ncount)) + numT >= mst_solnNumT) {
        // Base Case: Can't possibly improve upon best solution
        std::cout << " rF(cant improve base)";
        return false;
    }

    for (std::size_t p = 0; p < currPset.size(); ++p) {
        if (currPset[p] <= mst_pmargin) {
            continue;
        }

        for (std::size_t n = 0; n < currNset.size(); ++n) {
            if (currNset[n] >= mst_nmargin) {
                continue;
            }

            const double remainder = currPset[p] + currNset[n];
            const double payment = std::min(currPset[p], -1 * currNset[n]);

            // Add Solution
            mst_currSoln[numT].from = n;
            mst_currSoln[numT].to = p;
            mst_currSoln[numT].cost = payment;

            // Prime next recurse
            const int nextNumT = numT + 1;
            auto& nextNset = mst_nsets[nextNumT];
            auto& nextPset = mst_psets[nextNumT];
            nextNset = currNset;
            nextPset = currPset;
            int nextNcount = ncount;
            int nextPcount = pcount;

            if (remainder < 0) {
                nextNset[n] = remainder;
                nextPset[p] = 0;
                nextPcount--;
                if (remainder >= mst_nmargin) {
                    nextNcount--;
                }
            } else {
                nextNset[n] = 0;
                nextPset[p] = remainder;
                nextNcount--;
                if (remainder <= mst_pmargin) {
                    nextPcount--;
                }
            }

            // Recursive case.  If we find a solution recheck for the base case where
            // we can't improve upon solution because mst_solnNumT will be different.
            if (SettleTreeRecurse(
                    nextPcount,
                    nextNcount,
                    nextNumT)
                && static_cast<int>(std::max(pcount, ncount)) + numT >= mst_solnNumT)
            {
                    std::cout << " rF(recheck base)" << std::flush;
                    return true;
            }
        }
    }

    // No solutions in this branch of the tree
    std::cout << " rF(end loop)";
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

// TODO: Rename this to solve
// from, to, cost
std::vector<std::tuple<std::string, std::string, double> > DataCore::Calculate() const
{
    if (m_ledger.empty()) {
        return {};
    }
    const auto& lastLedgerLine = m_ledger[m_ledger.size() - 1];
    if (!DebtsCanBeSettled(lastLedgerLine)) {
        return {};
    }

    return SettleTree(lastLedgerLine);
    //return SettleMinMax(m_ledger[m_ledger.size() - 1]);
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

