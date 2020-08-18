#ifndef PEOPLEMODEL_H
#define PEOPLEMODEL_H

#include "BillSplit-Core_global.h"
#include "person.h"

#include <QAbstractListModel>

#include <memory>
#include <vector>
#include <unordered_map>

class BILLSPLITCORE_EXPORT PeopleModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        InitialsRole,
        NameRole
    };

public:
    PeopleModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

    bool validateData(const QModelIndex& index, const QVariant& value, int role) const;
    bool validateData(const QVariant& value, int role) const;
    bool validateData(const QString& value, int role) const;
    bool addPerson(QString initials, QString name);

    void jsonRead(const QJsonObject& json);
    void jsonWrite(QJsonObject& json) const;

private:
    bool isIndexValid(const QModelIndex& i) const;

private:
    // TODO: Did you need this?  I don't think you use the lookup outside of an initials set
    std::vector<std::shared_ptr<Person> > people;
    std::unordered_map<QString, std::shared_ptr<Person> > initialsLookup;
};

#endif // PEOPLEMODEL_H
