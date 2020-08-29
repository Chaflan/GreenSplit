#ifndef TRANSACTIONSMODEL_H
#define TRANSACTIONSMODEL_H

#include "BillSplit-Core_global.h"
#include "datacore.h"

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

public:
    TransactionsModel(DataCore& dataCore, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

    bool addTransaction(const QString& payerName,
                        double cost,
                        const QString& description,
                        const QStringList& coveringPids);

    bool addTransaction(Transaction transaction);

    // TODO: These methods are ugly, people should be id'ed by initials
    // Maybe consider making the data core not care about anything but initials and use a wrapper
    QString getInitialFromPid(int pid) const;
    QStringList getAllInitials() const;
    QStringList getInitialsFromPidSet(const std::set<int>& pids) const;
    int getPidFromInitials(const QString& initials) const;
    std::set<int> getPidSetFromInitials(const QStringList& initials) const;
    // Make some of these private?

    void jsonRead(const QJsonObject& json);
    void jsonWrite(QJsonObject& json) const;

private:
    bool isIndexValid(const QModelIndex& i) const;

private:
    DataCore& m_data;
};

#endif // TRANSACTIONSMODEL_H
