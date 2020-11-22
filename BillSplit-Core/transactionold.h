#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "billsplit-core_global.h"
#include <QString>
#include <set>

class BILLSPLITCORE_EXPORT TransactionOld
{
public:
    // Rule of 5
    TransactionOld();
    TransactionOld(int payer, QString descr, double cost, std::set<int> covering);
    TransactionOld(const TransactionOld& other);
    const TransactionOld& operator= (const TransactionOld& other);

    const int id;
    int payerPid;
    QString description;
    double cost;
    std::set<int> coveringPids;

private:
    void Copy(const TransactionOld& other);

    static int nextId;
};

#endif // TRANSACTION_H
