#ifndef PEOPLETABLEMODEL_H
#define PEOPLETABLEMODEL_H

#include <QAbstractTableModel>
#include "billsplit-core_global.h"

class DataCoreObject;

class BILLSPLITCORE_EXPORT PeopleTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(DataCoreObject* data READ getDataCore WRITE setDataCore NOTIFY dataCoreChanged)

public:
    enum Column
    {
        Identifier = 0,
        Name = 1,
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
    Q_INVOKABLE int columnWidth(int columnIndex, int columnSpacing, int totalWidth);

    void setDataCore(DataCoreObject* data);

signals:
    void signalError(QString message) const;
    void dataCoreChanged() const;
    void modelCleared() const;  // TODO: rethink this

private:
    DataCoreObject* getDataCore() const          { return m_data; }

    bool isIndexValid(const QModelIndex& index) const;
    int stringToColumnIndex(const QString& columnRole) const;
    QString columnIndexToString(int columnIndex) const;
    void resetModel();

private:
    DataCoreObject* m_data = nullptr;
};

#endif // PEOPLETABLEMODEL_H
