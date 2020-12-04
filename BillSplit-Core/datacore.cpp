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
    SetLedgerRevisionIndex(m_transactions.size() - 1);
}

bool DataCore::DeleteTransactions(int index, int count)
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
    SetLedgerRevisionIndex(index);

    return count > 0;
}

bool DataCore::EditTransactionPayer(int index, std::string newPayer)
{
    VerifyTransactionIndex(index);
    if (newPayer != m_transactions[index].payer) {
        m_transactions[index].payer = std::move(newPayer);
        SetLedgerRevisionIndex(index);
        return true;
    }
    return false;
}

bool DataCore::EditTransactionCost(int index, double newCost)
{
    VerifyTransactionIndex(index);
    if (newCost != m_transactions[index].cost) {
        m_transactions[index].cost = newCost;
        SetLedgerRevisionIndex(index);
        return true;
    }
    return false;
}

bool DataCore::EditTransactionCovering(int index, std::set<std::string> newCovering)
{
    VerifyTransactionIndex(index);
    if (newCovering.empty()) {
        throw std::invalid_argument("Transaction covering set must not be empty");
    }
    if (newCovering != m_transactions[index].covering) {
        m_transactions[index].covering = std::move(newCovering);
        SetLedgerRevisionIndex(index);
        return true;
    }
    return false;
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

const std::set<std::string>& DataCore::GetTransactionCovering(int index) const
{
    VerifyTransactionIndex(index);
    return m_transactions[index].covering;
}

const std::vector<std::tuple<std::string, std::string, double> >& DataCore::GetResults() const
{
    UpdateResults();
    return m_results;
}

// Returns true if person was edited
bool DataCore::EditPerson(const std::string& oldName, const std::string& newName)
{
    if (PersonExists(newName)) {
        // Attempting to change name to one that already exists
        return false;
    }

    bool personWasEdited = false;
    for (auto& transaction : m_transactions) {
        if (transaction.payer == oldName) {
            transaction.payer = newName;
            personWasEdited = true;
        }
        auto node = transaction.covering.extract(oldName);
        if (!node.empty()) {
            node.value() = newName;
            transaction.covering.insert(std::move(node));
            personWasEdited = true;
        }
    }

    // No need to mark personWasEdited flag, names in the ledger and results
    // are a subset of names in transactions
    for (auto& ledgerEntry : m_ledger) {
        auto node = ledgerEntry.extract(oldName);
        if (!node.empty()) {
            node.key() = newName;
            ledgerEntry.insert(std::move(node));
        }
    }

    for (auto& result : m_results) {
        if (std::get<0>(result) == oldName) {
            std::get<0>(result) = newName;
        } else if (std::get<1>(result) == oldName) {
            std::get<1>(result) = newName;
        }
    }

    return personWasEdited;
}

bool DataCore::PersonExists(const std::string& name) const
{
    // We can't just check the last ledger line (which would be very fast)
    // A transaction with a below margin cost could exist.  So we must check all transactions

    for (const auto& transaction : m_transactions) {
        if (transaction.payer == name || transaction.covering.find(name) != transaction.covering.end()) {
            return true;
        }
    }

    return false;
}

void DataCore::Clear()
{
    m_transactions.clear();
    m_ledgerRevisionIndex = 0;
    m_resultsDirty = true;
}

// return true if ledger was updated
bool DataCore::UpdateLedger() const
{
    if (m_ledger.size() == m_transactions.size() && m_ledgerRevisionIndex >= m_ledger.size()) {
        return false;
    }

    if (m_ledger.size() > m_transactions.size()) {
        m_ledger.erase(m_ledger.begin() + m_transactions.size(), m_ledger.end());
    }

    for (int i = m_ledgerRevisionIndex; i < static_cast<int>(m_transactions.size()); ++i) {

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
    }

    m_resultsDirty = true;
    m_ledgerRevisionIndex = m_ledger.size();
    return true;
}

// retrun true if results were updated
bool DataCore::UpdateResults() const
{
    UpdateLedger();
    if (m_resultsDirty) {
        m_results = Solve();
        m_resultsDirty = false;
        return true;
    }
    return false;
}

void DataCore::VerifyTransactionIndex(int index) const
{
    if (index < 0 || index >= static_cast<int>(NumTransactions())) {
        throw std::out_of_range("Transaction index " + std::to_string(index) +
            " is out of range: [0," + std::to_string(NumTransactions() + 1) + "]");
    }
}

void DataCore::SetLedgerRevisionIndex(std::size_t newIndex) const
{
    m_ledgerRevisionIndex = std::min(m_ledgerRevisionIndex, newIndex);
}

std::vector<std::tuple<std::string, std::string, double> > DataCore::Solve() const
{
    // Beware: Solve assumes ledger is updated.  The chain of calls currently makes this always true.

    if (m_ledger.empty()) {
        return {};
    }

    return AlgoCore::SolveOptimal(m_ledger[m_ledger.size() - 1]);
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

