#ifndef RESULTSMODEL_H
#define RESULTSMODEL_H

#include "BillSplit-Core_global.h"
#include "datacoreobject.h"

#include <QAbstractListModel>
#include <vector>


class BILLSPLITCORE_EXPORT ResultsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(DataCoreObject* data READ getDataCore WRITE setDataCore NOTIFY dataCoreChanged)

public:
    ResultsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void UpdateResults();

signals:
    void dataCoreChanged() const;

private:
    DataCoreObject* getDataCore() const          { return m_data; }
    void setDataCore(DataCoreObject* data);

    bool isIndexValid(const QModelIndex& i) const;

private:
    DataCoreObject* m_data = nullptr;
};

#endif // RESULTSMODEL_H
