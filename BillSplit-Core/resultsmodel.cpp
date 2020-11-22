#include "resultsmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFont>
#include <QFontMetrics>

ResultsModel::ResultsModel(QObject* parent) :
    QAbstractListModel(parent)
{
}

int ResultsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_data->numResults();
}

QVariant ResultsModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole && isIndexValid(index))
    {
        // TODO: cache these
        return QString("%1 owes %2 $%3")
                .arg(m_data->getResultDebtor(index.row()))
                .arg(m_data->getResultCreditor(index.row()))
                .arg(QString::number(m_data->getResultCost(index.row()), 'f', 2));
    }

    return QVariant();
}

QVariant ResultsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole && section == 0) {
        return "Result";
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

void ResultsModel::UpdateResults()
{
    beginResetModel();
    endResetModel();
}

void ResultsModel::setDataCore(DataCoreObject* data)
{
    if (data != m_data) {
        m_data = data;
        assert(m_data);
        emit dataCoreChanged();

        QObject::connect(m_data, &DataCoreObject::resultsChanged,
            this, &ResultsModel::UpdateResults);
        QObject::connect(m_data, &DataCoreObject::modelCleared,
            this, &ResultsModel::UpdateResults);
        UpdateResults();
    }
}

bool ResultsModel::isIndexValid(const QModelIndex& index) const
{
    if (!index.isValid() || index.row() >= rowCount()) {
        qDebug() << "Error - ResultsModel::isIndexValid - index invalid: ";
        qDebug() << "\tindex.isValid() == " << index.isValid();
        if (index.isValid()) {
            qDebug() << "\trow = " << index.row();
        }
        return false;
    }

    return true;
}
