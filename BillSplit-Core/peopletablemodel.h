#ifndef PEOPLETABLEMODEL_H
#define PEOPLETABLEMODEL_H

#include <QAbstractTableModel>
#include "billsplit-core_global.h"

class DataCoreObject;

class BILLSPLITCORE_EXPORT PeopleTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(DataCoreObject* dataCore READ getDataCore WRITE setDataCore NOTIFY dataCoreChanged)

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

    // data and setData overloads for easier direct calling.
    Q_INVOKABLE QVariant data(int row, int column, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data(int row, const QString& roleString, int role = Qt::DisplayRole) const;
    Q_INVOKABLE bool setData(int row, int column, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE bool setData(int row, const QString& roleString, const QVariant& value, int role = Qt::EditRole);

    Q_INVOKABLE bool addPerson(QString initials, QString name);
    Q_INVOKABLE bool addDefaultPerson();

    void setDataCore(DataCoreObject* data);

signals:
    void signalError(QString message) const;
    void dataCoreChanged() const;

private:
    DataCoreObject* getDataCore() const          { return m_data; }

    bool isIndexValid(const QModelIndex& index) const;
    int stringToColumnIndex(const QString& columnRole) const;
    QString columnIndexToString(int columnIndex) const;
    void resetModel();
    void getNextDefaultPerson();

private:
    int m_defaultNameIndex = 0;
    QString m_currDefaultName = "A";
    DataCoreObject* m_data = nullptr;
};

#endif // PEOPLETABLEMODEL_H
