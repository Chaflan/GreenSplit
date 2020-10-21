#ifndef PEOPLETABLEMODEL_H
#define PEOPLETABLEMODEL_H

#include <QAbstractTableModel>
#include "datacoreobject.h"

//class DataCoreObject;

class PeopleTableModel : public QAbstractTableModel
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

    // QAbstractTableModel Overrides
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    Q_INVOKABLE bool addPerson(QString initials, QString name);
    Q_INVOKABLE int columnWidth(int col);

signals:
    void signalError(QString message) const;
    void dataSet() const;

private:
    bool isIndexValid(const QModelIndex& index) const;

private:
    DataCoreObject* m_data = nullptr;
};

#endif // PEOPLETABLEMODEL_H
