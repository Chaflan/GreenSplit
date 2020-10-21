#include "resultsmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFont>
#include <QFontMetrics>

ResultsModel::ResultsModel(DataCoreOld& dataCore, QObject* parent) :
    QAbstractListModel(parent),
    m_data(dataCore),
    m_columnWidths(1)
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
        const TransactionOld& t = m_results[index.row()];

        return QString("%1 \t %2 -> %3")
                .arg(QString::number(t.cost, 'f', 2))
                .arg(m_data.GetPersonById(t.payerPid).initials)
                .arg(m_data.GetPersonById(*t.coveringPids.begin()).initials);
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

int ResultsModel::columnWidth(int c, const QFont* font)
{
    Q_UNUSED(font);

    if (c != 0) {
        return 0;
    }

    if (!m_columnWidths[c]) {

        // The font is always passed as nullptr, so I am hacking past this
        QFont hackFont;
        hackFont.setPointSize(15);
//        QFontMetrics defaultFontMetrics = QFontMetrics(QGuiApplication::font());
//        QFontMetrics fm = (font ? QFontMetrics(*font) : defaultFontMetrics);

        QFontMetrics fm(hackFont);
        int ret = fm.horizontalAdvance(headerData(c, Qt::Horizontal).toString() + QLatin1String(" ^")) + 8;

        for (int r = 0; r < rowCount(); ++r) {
            ret = qMax(ret, fm.horizontalAdvance(data(index(r,c), Qt::DisplayRole).toString()));
        }

        m_columnWidths[c] = ret;
    }

    return m_columnWidths[c];
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
