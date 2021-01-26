#ifndef TRANSACTIONSTABLEMODEL_H
#define TRANSACTIONSTABLEMODEL_H

#include "billsplit-core_global.h"
#include "datacoreobject.h"
#include "transactionmodel.h"
#include <QAbstractTableModel>

class BILLSPLITCORE_EXPORT TransactionsTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(DataCoreObject* dataCore READ getDataCore WRITE setDataCore NOTIFY dataCoreChanged)

public:
    enum Column
    {
        Cost = 0,
        Payer = 1,
        Covering = 2,
        Description = 3,
        COUNT
    };

    explicit TransactionsTableModel(QObject *parent = nullptr);

    // QAbstractTableModel overrides
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // data and setData overloads for easier direct calling.
    Q_INVOKABLE QVariant data(int row, int column, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data(int row, const QString& roleString, int role = Qt::DisplayRole) const;
    Q_INVOKABLE bool setData(int row, int column, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE bool setData(int row, const QString& roleString, const QVariant& value, int role = Qt::EditRole);

    Q_INVOKABLE void loadToModel(int row, TransactionModel* model) const;
    Q_INVOKABLE bool editFromModel(int row, /*const*/ TransactionModel* model);  // QML doesn't like the const
    Q_INVOKABLE bool addFromModel(/*const*/ TransactionModel* model);            // QML doesn't like the const
    Q_INVOKABLE int columnWidth(int columnIndex, int columnSpacing, int tableWidth) const;

    void setDataCore(DataCoreObject* data);

signals:
    void signalError(QString message) const;
    void dataCoreChanged() const;
    void columnWidthsChanged() const;

private:
    DataCoreObject* getDataCore() const          { return m_data; }

    bool isIndexValid(const QModelIndex& index) const;
    int stringToColumnIndex(const QString& columnRole) const;
    QString columnIndexToString(int columnIndex) const;
    void checkMaxLettersForChange(int specificColumn = -1) const;
    void resetModel();

private:
    DataCoreObject* m_data = nullptr;

    // Cached column width related data
    mutable bool m_cwValid = false;             // If false, col widths need to be recalculated
    mutable QVector<int> m_cwColWidths;         // Stores actual column widths
    mutable QVector<int> m_cwMaxLetterCounts;   // Stores max letter counts for each column
    mutable int m_cwColumnSpacing = 0;
    mutable int m_cwTableWidth = 0;
};

#endif // TRANSACTIONSTABLEMODEL_H
