#include "transactionold.h"

int TransactionOld::nextId = 0;

TransactionOld::TransactionOld() :
    id(nextId++)
{
}

TransactionOld::TransactionOld(int payer, QString descr, double cost, std::set<int> covering) :
    id(nextId++),
    payerPid(payer),
    description(std::move(descr)),
    cost(cost),
    coveringPids(std::move(covering))
{
}

TransactionOld::TransactionOld(const TransactionOld& other) :
    id(nextId++)
{
    Copy(other);
}

const TransactionOld& TransactionOld::operator=(const TransactionOld& other)
{
    Copy(other);
    return *this;
}

void TransactionOld::Copy(const TransactionOld& other)
{
    payerPid = other.payerPid;
    description = other.description;
    cost = other.cost;
    coveringPids = other.coveringPids;
}
