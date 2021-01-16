#include "resultsmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFont>
#include <QFontMetrics>

// TODO: Ensure your brace style is upheld throughout

ResultsModel::ResultsModel(QObject* parent) :
    QAbstractListModel(parent)
{ }

int ResultsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_data->numResults();
}

QVariant ResultsModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole && isIndexValid(index)) {
        checkCacheValidity();
        return m_results[index.row()];
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

int ResultsModel::getMaxLetterCount() const
{
    checkCacheValidity();
    return m_maxLetterCount;
}

void ResultsModel::resetModel()
{
    beginResetModel();
    m_resultsValid = false;
    endResetModel();
}

void ResultsModel::setDataCore(DataCoreObject* data)
{
    if (data != m_data) {
        m_data = data;
        assert(m_data);
        emit dataCoreChanged();

        QObject::connect(m_data, &DataCoreObject::resultsChanged,
            this, &ResultsModel::resetModel);
        resetModel();
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

void ResultsModel::checkCacheValidity() const
{
    if (!m_resultsValid) {
        buildCache();
        m_resultsValid = true;
    }
}

void ResultsModel::buildCache() const
{
    static const QString strTemplate("(%1)%2    owes    (%3)%4    %5");

    m_results.clear();
    m_maxLetterCount = 6; // "Result" length
    for (int i = 0; i < rowCount(); ++i ) {
        m_results.append(strTemplate
                .arg(m_data->getResultDebtorId(i))
                .arg(m_data->getResultDebtorName(i))
                .arg(m_data->getResultCreditorId(i))
                .arg(m_data->getResultCreditorName(i))
                .arg(QString::number(m_data->getResultCost(i), 'f', 2)));
        m_maxLetterCount = std::max(m_maxLetterCount, m_results[i].length());
    }
}
