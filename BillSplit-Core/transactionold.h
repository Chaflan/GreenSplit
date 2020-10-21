#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "BillSplit-Core_global.h"
#include <QString>
#include <set>

class BILLSPLITCORE_EXPORT Transaction
{
public:
    // Rule of 5
    Transaction();
    Transaction(int payer, QString descr, double cost, std::set<int> covering);
    Transaction(const Transaction& other);
    const Transaction& operator= (const Transaction& other);

    const int id;
    int payerPid;
    QString description;
    double cost;
    std::set<int> coveringPids;

private:
    void Copy(const Transaction& other);

    static int nextId;
};

#endif // TRANSACTION_H
