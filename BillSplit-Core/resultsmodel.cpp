#include "resultsmodel.h"
#include <QJsonArray>
#include <QJsonObject>

ResultsModel::ResultsModel(DataCore& dataCore, QObject* parent) :
    QAbstractListModel(parent),
    m_data(dataCore)
{
}

int ResultsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(m_results.size());
}

QVariant ResultsModel::data(const QModelIndex& index, int role) const
{
    // TODO: Show all accounts paied on zero entries

    if (role == Qt::DisplayRole && isIndexValid(index))
    {
        const Transaction& t = m_results[index.row()];

        return QString("%1 \t %2 -> %3")
                .arg(QString::number(t.cost, 'f', 2))
                .arg(m_data.GetPersonById(t.payerPid).initials)
                .arg(m_data.GetPersonById(*t.coveringPids.begin()).initials);
    }

    return QVariant();
}

//bool ResultsModel::setData(const QModelIndex& index, const QVariant& value, int role)
//{
//    if (isIndexValid(index) && (role == Roles::InitialsRole || role == Roles::NameRole))
//    {
//        Person personCopy(m_data.GetPersonByIndex(index.row()));

//        switch (role)
//        {
//            case Roles::InitialsRole: personCopy.initials = value.toString(); break;
//            case Roles::NameRole: personCopy.name = value.toString(); break;
//        }

//        if (m_data.EditPerson(index.row(), personCopy))
//        {
//            emit dataChanged(index, index);
//            return true;
//        }
//    }

//    return false;
//}

//bool ResultsModel::removeRows(int row, int count, const QModelIndex& parent)
//{
//    // TODO: is parent used?  How?  Check validity?
//    beginRemoveRows(parent, row, row + count - 1);
//    bool result = m_data.DeletePeople(row, count);
//    endRemoveRows();
//    return result;
//}

void ResultsModel::updateCalculations()
{
    beginResetModel();
    m_results = m_data.Calculate();
    endResetModel();
}

bool ResultsModel::isIndexValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() < rowCount();
}
