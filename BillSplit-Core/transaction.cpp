#include "transaction.h"

int Transaction::nextId = 0;

Transaction::Transaction() :
    id(nextId++)
{
}

Transaction::Transaction(int payer, QString descr, double cost, std::set<int> covering) :
    id(nextId++),
    payerPid(payer),
    description(std::move(descr)),
    cost(cost),
    coveringPids(std::move(covering))
{
}

Transaction::Transaction(const Transaction& other) :
    id(nextId++)
{
    Copy(other);
}

const Transaction& Transaction::operator=(const Transaction& other)
{
    Copy(other);
    return *this;
}

void Transaction::Copy(const Transaction& other)
{
    payerPid = other.payerPid;
    description = other.description;
    cost = other.cost;
    coveringPids = other.coveringPids;
}
