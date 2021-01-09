#ifndef RESULTSMODEL_H
#define RESULTSMODEL_H

#include "billsplit-core_global.h"
#include "datacoreobject.h"

#include <QAbstractListModel>
#include <vector>

//---------------------------------------------------------------------------------
// ResultsModel is a lot simpler than the other models.  It is wholely reliant on its
// reset signal from the data core.  Data is only read from the core and the model
// is mostly independent otherwise.  Because of this and the complicated presentation
// of the results data, we cache it, and invalidate the cache on reset.
//---------------------------------------------------------------------------------
class BILLSPLITCORE_EXPORT ResultsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(DataCoreObject* data READ getDataCore WRITE setDataCore NOTIFY dataCoreChanged)

public:
    ResultsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int getMaxLetterCount() const;
    void resetModel();

    void setDataCore(DataCoreObject* data);

signals:
    void dataCoreChanged() const;
    void maxLetterCountChanged() const;

private:
    DataCoreObject* getDataCore() const          { return m_data; }

    bool isIndexValid(const QModelIndex& i) const;
    void checkCacheValidity() const;
    void buildCache() const;

private:
    DataCoreObject* m_data = nullptr;

    // Cached data
    mutable QStringList m_results;
    mutable bool m_resultsValid;
    mutable int m_maxLetterCount;
};

#endif // RESULTSMODEL_H
