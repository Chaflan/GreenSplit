#include "person.h"

int Person::nextId = 0;

Person::Person() :
    id(nextId++)
{
}

Person::Person(QString i, QString n) :
    id(nextId++),
    initials(std::move(i)),
    name(std::move(n))
{
}
