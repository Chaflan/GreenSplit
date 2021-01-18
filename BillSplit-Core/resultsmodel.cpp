#include "resultsmodel.h"
#include "datacoreobject.h"
#include <QDebug>

ResultsModel::ResultsModel(QObject* parent) :
    QAbstractListModel(parent)
{ }

int ResultsModel::rowCount(const QModelIndex& parent) const
{
    checkCacheValidity();
    return parent.isValid() ? 0 : m_numResults;
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
    // getMaxLetterCount can get called before data is set in QML due
    // to it being involved in graphical layout.
    if (!m_data) {
        return 0;
    }

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
    m_numResults = m_data->numResults();
    m_maxLetterCount = 6; // "Result" length
    for (int i = 0; i < m_numResults; ++i ) {
         m_results.append(strTemplate.arg(
                m_data->getResultDebtorId(i),
                m_data->getResultDebtorName(i),
                m_data->getResultCreditorId(i),
                m_data->getResultCreditorName(i),
                QString::number(m_data->getResultCost(i), 'f', 2)));
        m_maxLetterCount = std::max(m_maxLetterCount, m_results[i].length());
    }

    if (m_numResults == 0) {
        m_numResults = 1;
        m_results.append("All debts have been settled.");
    } else if (m_numResults < 0) {
        m_numResults = 1;
        m_results.append("Error");
    }
}
