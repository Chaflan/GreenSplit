#ifndef RESULTSMODEL_H
#define RESULTSMODEL_H

#include "BillSplit-Core_global.h"
#include "datacore.h"

#include <QAbstractListModel>
#include <vector>


class BILLSPLITCORE_EXPORT ResultsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ResultsModel(DataCore& dataCore, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    //bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    //bool removeRows(int row, int count, const QModelIndex& parent) override;

    void updateCalculations();

private:
    bool isIndexValid(const QModelIndex& i) const;

private:
    DataCore& m_data;
    std::vector<Transaction> m_results;
};

#endif // RESULTSMODEL_H
