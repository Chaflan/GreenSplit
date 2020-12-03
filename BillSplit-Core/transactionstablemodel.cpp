#include "transactionstablemodel.h"
#include <QDebug>

// TODO: Consistent braces
// TODO: Compare to peopletablemodel and make sure they align nicely

TransactionsTableModel::TransactionsTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int TransactionsTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_data->numTransactions();
}

int TransactionsTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : Column::COUNT;
}

QVariant TransactionsTableModel::data(const QModelIndex& index, int role) const
{
    if (isIndexValid(index) && role == Qt::DisplayRole) {
        switch (index.column()) {
            case Column::Cost: return m_data->getTransactionCost(index.row());
            case Column::Payer: return m_data->getTransactionPayer(index.row());
            case Column::Covering: {
                QString coveringListStr;
                bool needsComma = false;
                for (const auto& str : m_data->getTransactionCovering(index.row())) {
                    if (needsComma) {
                        coveringListStr += ',';
                    } else {
                        needsComma = true;
                    }
                    coveringListStr += str;
                }
                return coveringListStr;
            }
            case Column::Description: return m_data->getTransactionDescription(index.row());
        }
    }

    // TODO: Try calling base data instead here, same with peopletablemodel
    return QVariant();
}

bool TransactionsTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!isIndexValid(index)) {
        return false;
    }

    if (role != Qt::EditRole) {
        return QAbstractTableModel::setData(index, value, role);
    }

    bool successfulEdit = false;
    switch(index.column()) {
        case Column::Cost: successfulEdit = m_data->editTransactionCost(index.row(), value.toDouble()); break;
        case Column::Payer: successfulEdit = m_data->editTransactionPayer(index.row(), value.toString()); break;
        case Column::Covering: successfulEdit = m_data->editTransactionCovering(index.row(), value.toStringList()); break;
        case Column::Description: successfulEdit = m_data->editTransactionDescription(index.row(), value.toString()); break;
    }

    if (successfulEdit) {
        emit dataChanged(index, index);
    }

    return successfulEdit;
}

bool TransactionsTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid()) {
        qDebug() << "Error - TransactionsTableModel::removeRows - Valid parent in non-tree";
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    bool result = m_data->deleteTransactions(row, count);
    endRemoveRows();
    return result;
}

QVariant TransactionsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        return columnIndexToString(section);
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

// TODO: Are these used anymore?
QVariant TransactionsTableModel::getData(int row, int column, int role) const
{
    return data(index(row, column), role);
}

QVariant TransactionsTableModel::getData(int row, const QString& roleString, int role) const
{
    return getData(row, stringToColumnIndex(roleString), role);
}

bool TransactionsTableModel::setData(int row, int column, const QVariant& value, int role)
{
    return setData(index(row, column), value, role);
}

bool TransactionsTableModel::setData(int row, const QString& roleString, const QVariant& value, int role)
{
    return setData(row, stringToColumnIndex(roleString), value, role);
}

int TransactionsTableModel::columnWidth(int columnIndex, int columnSpacing, int totalWidth)
{
    // TODO: Static constexpr sum formula here
    switch (columnIndex) {
        case Column::Cost: return 100;
        case Column::Payer: return 100;
        case Column::Covering: return 100;
        case Column::Description: return totalWidth - columnSpacing * (Column::COUNT - 1) - 300;
    }

    return 0;
}

void TransactionsTableModel::loadToModel(int row, TransactionModel* model) const
{
    assert(model);
    model->load(
        m_data->getTransactionCost(row),
        m_data->getTransactionPayer(row),
        m_data->getTransactionCovering(row),
        m_data->getTransactionDescription(row));
}

bool TransactionsTableModel::editFromModel(int row, TransactionModel* model)
{
    assert(model);
    bool ret = true;
    ret &= setData(row, Column::Cost, model->getCostDbl());
    ret &= setData(row, Column::Payer, model->getPayerName());
    ret &= setData(row, Column::Covering, model->getCoveringStringList());
    ret &= setData(row, Column::Description, model->getDescription());
    return ret;
}

bool TransactionsTableModel::addFromModel(TransactionModel* model)
{
    assert(model);
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    bool result = m_data->addTransaction(
        model->getCostDbl(),
        model->getPayerName(),
        model->getCoveringStringList(),
        model->getDescription());

    endInsertRows();
    return result;
}

void TransactionsTableModel::setDataCore(DataCoreObject* data)
{
    if (data != m_data) {
        m_data = data;
        assert(m_data);
        emit dataCoreChanged();

        QObject::connect(m_data, &DataCoreObject::transactionsChanged,
            this, &TransactionsTableModel::resetModel);
        resetModel();
    }
}

bool TransactionsTableModel::isIndexValid(const QModelIndex& index) const
{
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount()) {
        qDebug() << "Error - TransactionsTableModel::isIndexValid - index invalid: ";
        qDebug() << "\tindex.isValid() == " << index.isValid();
        if (index.isValid()) {
            qDebug() << "\trow = " << index.row() << " column = " << index.column();
        }
        return false;
    }

    return true;
}

int TransactionsTableModel::stringToColumnIndex(const QString& columnRole) const
{
    // TODO: Iterator initialization
    const static QMap<QString, int> stringToColumn {
        { columnIndexToString(Column::Cost), Column::Cost },
        { columnIndexToString(Column::Payer), Column::Payer },
        { columnIndexToString(Column::Covering), Column::Covering },
        { columnIndexToString(Column::Description), Column::Description }
    };

    auto findResult = stringToColumn.find(columnRole);
    if (findResult == stringToColumn.end()) {
        qDebug() << "Error - TransactionsTableModel::stringToColumnIndex - columnRole not in map: " << columnRole;
        return 0;
    }

    return findResult == stringToColumn.end() ? 0 : findResult.value();
}

QString TransactionsTableModel::columnIndexToString(int columnIndex) const
{
    // TODO: Figure out how to do this instead, it doesn't compile
    //
    //static constexpr std::array<QStringView, 2> columnRoles {
    //    "Identifier",
    //    "Name"
    //};

    const static QString columnToString[Column::COUNT] {
        "Cost",
        "Payer",
        "Covering",
        "Description"
    };

    if (columnIndex < 0 || columnIndex >= Column::COUNT) {
        qDebug() << "Error - TransactionsTableModel::columnIndexToString - invalid columnIndex: " << columnIndex;
        return QString();
    }

    return columnToString[columnIndex];
}

void TransactionsTableModel::resetModel()
{
    beginResetModel();
    endResetModel();
}
