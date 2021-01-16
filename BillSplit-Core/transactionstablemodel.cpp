#include "transactionstablemodel.h"
#include <QDebug>

// TODO: Consistent braces
// TODO: Compare to peopletablemodel and make sure they align nicely

TransactionsTableModel::TransactionsTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_cwColWidths(Column::COUNT, 0),
    m_cwMaxLetterCounts(Column::COUNT, 0)
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
        checkMaxLettersForChange(index.column());
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

    if (result) {
        checkMaxLettersForChange();
    }

    return result;
}

QVariant TransactionsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        return columnIndexToString(section);
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant TransactionsTableModel::data(int row, int column, int role) const
{
    return data(index(row, column), role);
}

QVariant TransactionsTableModel::data(int row, const QString& roleString, int role) const
{
    return data(row, stringToColumnIndex(roleString), role);
}

bool TransactionsTableModel::setData(int row, int column, const QVariant& value, int role)
{
    return setData(index(row, column), value, role);
}

bool TransactionsTableModel::setData(int row, const QString& roleString, const QVariant& value, int role)
{
    return setData(row, stringToColumnIndex(roleString), value, role);
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

    QString modelCostStr = model->getCostStr();
    bool ok;
    modelCostStr.toDouble(&ok);
    if (!ok) {
        // Don't pester with this warning for an empty value
        if (!modelCostStr.isEmpty()) {
            emit signalError("Cost must be numeric.\nSetting it to zero.");
        }
        ret &= setData(row, Column::Cost, 0);
    } else {
        ret &= setData(row, Column::Cost, model->getCostDbl());
    }

    ret &= setData(row, Column::Payer, model->getPayerName());
    ret &= setData(row, Column::Covering, model->getCoveringStringList());
    ret &= setData(row, Column::Description, model->getDescription());
    return ret;
}

bool TransactionsTableModel::addFromModel(TransactionModel* model)
{
    assert(model);

    bool ok;
    double cost = 0;
    model->getCostStr().toDouble(&ok);
    if (!ok) {
        emit signalError("Cost must be numeric.\nSetting it to zero.");
    } else {
        cost = model->getCostDbl();
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    bool result = m_data->addTransaction(
        cost,
        model->getPayerName(),
        model->getCoveringStringList(),
        model->getDescription());
    endInsertRows();

    if (result) {
        checkMaxLettersForChange();
    }

    return result;
}

int TransactionsTableModel::columnWidth(int columnIndex, int columnSpacing, int tableWidth) const
{
    // During startup tableWidth can cycle through some nonsense values causing unnecessary calculation
    if (tableWidth <= 0) {
        return 0;
    }

    if (m_cwColumnSpacing != columnSpacing) {
        m_cwColumnSpacing = columnSpacing;
        m_cwValid = false;
    }

    if (m_cwTableWidth != tableWidth) {
        m_cwTableWidth = tableWidth;
        m_cwValid = false;
    }

    if (!m_cwValid) {
        // TODO: Consider using actual font size and style in the future, or just wait for QT
        // to write some proper table code.
        const int letterFactor = 12;
        QVector<int> requiredColWidths(Column::COUNT);
        for (int c = 0; c < Column::COUNT; ++c) {
            requiredColWidths[c] = m_cwMaxLetterCounts[c] * letterFactor;
        }

        const QVector<int> defaultColWidths{ 80, 60, 95, tableWidth - columnSpacing * 3 - 235 };

        int excess = 0;
        for (int c = 0; c < Column::COUNT; ++c) {
            excess += requiredColWidths[c] > defaultColWidths[c] ? requiredColWidths[c] - defaultColWidths[c] : 0;
        }

        bool widthsChanged = false;
        for (int c = Column::COUNT - 1; c >= 0; --c) {
            int newColumnWidth = 0;
            int currExcess = defaultColWidths[c] - requiredColWidths[c];
            if (currExcess >= excess) {
                newColumnWidth = defaultColWidths[c] - excess;
                excess = 0;
            } else {
                newColumnWidth = requiredColWidths[c];
                if (currExcess > 0) {
                    excess -= currExcess;
                }
            }
            if (m_cwColWidths[c] != newColumnWidth) {
                m_cwColWidths[c] = newColumnWidth;
                widthsChanged = true;
            }
        }

        if (widthsChanged) {
            // This signal is used only to tell the column header to resize, keeping it
            // in sync with the rest of the table.
            emit columnWidthsChanged();
        }

        m_cwValid = true;
    }

    return m_cwColWidths[columnIndex];
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
    // TODO: Figure out how to do this Qt style instead, it doesn't compile
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

// Check the max letters vector for changes and emit a signal if any are seen.
// specific column is used when only one specific column needs to be checked.
void TransactionsTableModel::checkMaxLettersForChange(int specificColumn) const
{
    bool changeOccurred = false;
    auto checkColumn = [&changeOccurred, this](int colIndex){
        int currMaxLetterCount = columnIndexToString(colIndex).length();
        for (int r = 0; r < rowCount(); ++r) {
            currMaxLetterCount = std::max(
                currMaxLetterCount,
                data(r , colIndex).toString().length());
        }
        if (m_cwMaxLetterCounts[colIndex] != currMaxLetterCount) {
            m_cwMaxLetterCounts[colIndex] = currMaxLetterCount;
            changeOccurred = true;
        }
    };

    if (0 <= specificColumn && specificColumn < Column::COUNT) {
        checkColumn(specificColumn);
    } else {
        // Check all columns
        for (int c = 0; c < Column::COUNT; ++c) {
            checkColumn(c);
        }
    }

    if (changeOccurred) {
        m_cwValid = false;
    }
}

void TransactionsTableModel::resetModel()
{
    beginResetModel();
    endResetModel();
    checkMaxLettersForChange();
}
