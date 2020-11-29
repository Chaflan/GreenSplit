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
        m_results = Solve();
    }
}

bool DataCore::DebtsCanBeSettled(std::unordered_map<std::string, double> debts)
{
    double sum = 0;
    for (const auto& [ name, debt ]  : debts) {
        sum += debt;
    }
    return sum < 0.01;
}

std::vector<std::tuple<std::string, std::string, double> > DataCore::Solve() const
{
    if (m_ledger.empty()) {
        return {};
    }
    const auto& lastLedgerLine = m_ledger[m_ledger.size() - 1];
    if (lastLedgerLine.empty()) {
        return {};
    }

    // TODO: Put this in the algo core
    if (!DebtsCanBeSettled(lastLedgerLine)) {
        // TODO: Throw?
        return {};
    }

    return AlgoCore::SolveOptimal(lastLedgerLine);
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

