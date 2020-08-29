#ifndef PERSON_H
#define PERSON_H

#include "BillSplit-Core_global.h"
#include <QString>

class BILLSPLITCORE_EXPORT Person
{
public:
    Person();
    Person(QString initials, QString name);

    const int id;
    QString initials;
    QString name;

private:
    static int nextId;
};

#endif // PERSON_H
