#ifndef PERSON_H
#define PERSON_H

#include <QString>

class Person
{
public:
    Person();
    Person(QString initials, QString name);

    unsigned int id;
    QString initials;
    QString name;

private:
    static unsigned int nextId;
};

#endif // PERSON_H
