#ifndef PEOPLETABLEMODEL_H
#define PEOPLETABLEMODEL_H

#include <QAbstractTableModel>
#include "BillSplit-Core_global.h"
#include "datacoreobject.h"

#include <QLatin1String>

//class DataCoreObject;

class BILLSPLITCORE_EXPORT PeopleTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(DataCoreObject* data MEMBER m_data NOTIFY dataSet)

public:
    enum Column
    {
        Identifier = 0,
        FullName = 1,
        COUNT = 2
    };

    explicit PeopleTableModel(QObject *parent = nullptr);

    // QAbstractTableModel overrides
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // data and setData overloads for easier direct calling.  data name must be changed to avoid property conflict
    Q_INVOKABLE QVariant getData(int row, int column, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant getData(int row, const QString& roleString, int role = Qt::DisplayRole) const;
    Q_INVOKABLE bool setData(int row, int column, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE bool setData(int row, const QString& roleString, const QVariant& value, int role = Qt::EditRole);

    Q_INVOKABLE bool addPerson(QString initials, QString name);
    Q_INVOKABLE int columnWidth(int columnIndex, int windowWidth);

signals:
    void signalError(QString message) const;
    void dataSet() const;

private:
    bool isIndexValid(const QModelIndex& index) const;
    int stringToColumnIndex(const QString& columnRole) const;
    QString columnIndexToString(int columnIndex) const;

private:
    QVector<int> m_columnWidths;
    QVector<int> m_longestIndices;  // index and length
    int m_lastColumnWidth = 0;
    int m_windowWidth = 0;


    DataCoreObject* m_data = nullptr;
};

#endif // PEOPLETABLEMODEL_H
