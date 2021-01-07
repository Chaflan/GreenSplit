#ifndef RESULTSMODEL_H
#define RESULTSMODEL_H

#include "billsplit-core_global.h"
#include "datacoreobject.h"

#include <QAbstractListModel>
#include <vector>


class BILLSPLITCORE_EXPORT ResultsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(DataCoreObject* data READ getDataCore WRITE setDataCore NOTIFY dataCoreChanged)
    Q_PROPERTY(int maxLetterCount READ getMaxLetterCount NOTIFY maxLetterCountChanged)

public:
    ResultsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int getMaxLetterCount() const;
    void resetModel();

    void setDataCore(DataCoreObject* data);

signals:
    void dataCoreChanged() const;
    void maxLetterCountChanged() const;

private:
    DataCoreObject* getDataCore() const          { return m_data; }

    bool isIndexValid(const QModelIndex& i) const;

private:
    DataCoreObject* m_data = nullptr;
    mutable int m_maxLetterCount;
};

#endif // RESULTSMODEL_H
