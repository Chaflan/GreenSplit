#ifndef PEOPLEMODEL_H
#define PEOPLEMODEL_H

#include "BillSplit-Core_global.h"
#include "datacore.h"

#include <QAbstractListModel>

#include <memory>
#include <vector>
#include <unordered_map>

class BILLSPLITCORE_EXPORT PeopleModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        InitialsRole,
        NameRole
    };

    enum Sections
    {
        Initials = 0,
        Name = 1,
        COUNT = 2
    };

public:
    PeopleModel(DataCore& dataCore, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool addPerson(QString initials, QString name);

    void jsonRead(const QJsonObject& json);
    void jsonWrite(QJsonObject& json) const;

private:
    bool isIndexValid(const QModelIndex& i) const;

private:
    DataCore& m_data;
};

#endif // PEOPLEMODEL_H
