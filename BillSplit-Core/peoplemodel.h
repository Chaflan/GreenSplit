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

    // TODO: Consider a notify method for completeness
    // Property is marked constant as it is meant to be used in a modal window where the people model
    // cannot be changed.  If used in another way we need a notify signal
    Q_PROPERTY(QStringList allPeople READ getAllPeople CONSTANT)

public:
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        InitialsRole,
        NameRole
    };

    enum Column
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
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int getColumnFromRole(const QString& role) const;
    Q_INVOKABLE QVariant data(int row, int column, int role = Qt::DisplayRole) const;
    Q_INVOKABLE bool setData(int row, int column, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE bool addPerson(QString initials, QString name);
    Q_INVOKABLE int columnWidth(int c, const QFont* font = nullptr);
    Q_INVOKABLE QStringList getSelectedPeople(QList<bool> selection) const;

    QStringList getAllPeople() const;
    void jsonRead(const QJsonObject& json);
    void jsonWrite(QJsonObject& json) const;

signals:
    void signalError(QString pErrorMessage) const;

private:
    bool isIndexValid(const QModelIndex& i) const;

private:
    DataCore& m_data;
    QVector<int> m_columnWidths;
};

#endif // PEOPLEMODEL_H
