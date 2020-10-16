#ifndef TRANSACTIONSMODEL_H
#define TRANSACTIONSMODEL_H

#include "BillSplit-Core_global.h"
#include "datacore.h"
#include "transactionmodel.h"

#include <QAbstractListModel>

#include <memory>
#include <vector>
#include <unordered_map>

class BILLSPLITCORE_EXPORT TransactionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        PayerRole,
        PayerPidRole,
        DescriptionRole,
        CostRole,
        CoveringInitialsRole,
        //CoveringPidsRole
    };

    // TODO: Standardize these names
    enum Column
    {
        Cost = 0,
        Payer = 1,
        Covering = 2,
        Description = 3,
        COUNT
    };

public:
    TransactionsModel(DataCore& dataCore, QObject* parent = nullptr);

    // TODO: Isn't this numRows in the other one?
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    Q_INVOKABLE QVariant data(int row, int column, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data(int row, const QString& roleString, int role =  Qt::DisplayRole) const;
    Q_INVOKABLE bool setData(int row, int column, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE bool setCostFromString(int row, const QString& cost);
    Q_INVOKABLE bool setDataString(int row, const QString& roleString, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE int getColumnFromRole(const QString& role) const;
    Q_INVOKABLE bool addTransaction(const QString& payerName,
                        double cost,
                        const QString& description,
                        const QStringList& coveringPids);

    bool addTransaction(Transaction transaction);

    // TODO: These methods are ugly, people should be id'ed by initials
    // Maybe consider making the data core not care about anything but initials and use a wrapper
    Q_INVOKABLE QString getInitialFromPid(int pid) const;
    QStringList getAllInitials() const;
    QStringList getInitialsFromPidSet(const std::set<int>& pids) const;
    Q_INVOKABLE int getPidFromInitials(const QString& initials) const;
    std::set<int> getPidSetFromInitials(const QStringList& initials) const;
    Q_INVOKABLE QStringList getCoveringStringList(int row) const;
    // Make some of these private?

    Q_INVOKABLE int columnWidth(int c, const QFont* font = nullptr);

    Q_INVOKABLE void loadToModel(int row, TransactionModel* model) const;
    Q_INVOKABLE bool editFromModel(int row, /*const*/ TransactionModel* model);
    Q_INVOKABLE bool addFromModel(/*const*/ TransactionModel* model);

    void jsonRead(const QJsonObject& json);
    void jsonWrite(QJsonObject& json) const;


private:
    bool isIndexValid(const QModelIndex& i) const;

private:
    DataCore& m_data;
    QVector<int> m_columnWidths;  // <- mutable!
};

#endif // TRANSACTIONSMODEL_H
