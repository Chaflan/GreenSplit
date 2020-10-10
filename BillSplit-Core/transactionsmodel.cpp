#include "transactionsmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFont>
#include <QFontMetrics>

TransactionsModel::TransactionsModel(DataCore& dataCore, QObject* parent) :
    QAbstractListModel(parent),
    m_data(dataCore),
    m_columnWidths(Column::COUNT)
{
}

int TransactionsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_data.NumTransactions();
}

int TransactionsModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : Column::COUNT;
}

QVariant TransactionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case Column::Cost: return "Cost";
            case Column::Payer: return "Payer";
            case Column::Covering: return "Covering";
            case Column::Description: return "Description";
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant TransactionsModel::data(const QModelIndex& index, int role) const
{
    if (isIndexValid(index)) {
        auto transaction = m_data.GetTransactionByIndex(index.row());

        switch (role)
        {
            case Qt::DisplayRole:
            {
                switch(index.column()) {
                    case Column::Cost: return QString::number(transaction.cost, 'f', 2);
                    case Column::Payer: return getInitialFromPid(transaction.payerPid);
                    case Column::Covering: {
                        QString groupString;
                        for (int pid : transaction.coveringPids) {
                            groupString += (groupString.isEmpty() ? "" : ",") + getInitialFromPid(pid);
                        }
                        return groupString;
                    }
                    case Column::Description: return transaction.description;
                }
                break;
            }

            case Roles::IdRole: return transaction.id;
            case Roles::PayerRole: return getInitialFromPid(transaction.payerPid);
            case Roles::PayerPidRole: return transaction.payerPid;
            case Roles::DescriptionRole: return transaction.description;
            case Roles::CostRole: return transaction.cost;
            case Roles::CoveringInitialsRole: return getInitialsFromPidSet(transaction.coveringPids);
        }
    }

    return QVariant();
}

// TODO: Not convinced we need this method, probably want to set transaction
bool TransactionsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!isIndexValid(index)) {
        return false;
    }

    if (role == Qt::EditRole) {
        switch (index.column()) {
            case Column::Cost: return setData(index, value, Roles::CostRole);
            case Column::Payer: return setData(index, value, Roles::PayerRole);
            case Column::Covering: return setData(index, value, Roles::CoveringInitialsRole);
            case Column::Description: return setData(index, value, Roles::DescriptionRole);
        }
        return false;
    }

    Transaction transactionCopy(m_data.GetTransactionByIndex(index.row()));

    switch (role)
    {
        case Roles::PayerRole: transactionCopy.payerPid = getPidFromInitials(value.toString()); break;
        case Roles::PayerPidRole: transactionCopy.payerPid = value.toInt(); break;
        case Roles::DescriptionRole: transactionCopy.description = value.toString(); break;
        case Roles::CostRole: transactionCopy.cost = value.toDouble(); break;

        case Roles::CoveringInitialsRole:
            transactionCopy.coveringPids = getPidSetFromInitials(value.toStringList());
            break;
    }

    if (m_data.EditTransaction(index.row(), transactionCopy))
    {
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

bool TransactionsModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    bool result = m_data.DeleteTransactions(row, count);
    endRemoveRows();
    return result;
}

Qt::ItemFlags TransactionsModel::flags(const QModelIndex& index) const
{
    // TODO: I don't understand the purpose of this return variable.
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant TransactionsModel::data(int row, int column, int role) const
{
    return data(index(row, column), role);
}

QVariant TransactionsModel::data(int row, const QString& roleString, int role) const
{
    return data(row, getColumnFromRole(roleString), role);
}

bool TransactionsModel::setData(int row, int column, const QVariant& value, int role)
{
    return setData(index(row, column), value, role);
}

// Hack.  I fiddled with the double spinbox for a very long time in QML and could not get it to
// work with floats.  So this function uses proper a proper programming language to do it.
bool TransactionsModel::setCostFromString(int row, const QString& cost)
{
    return setData(row, Column::Cost, cost.toDouble(), Qt::EditRole);
}

// TODO: Undo this, it works fine
// Cannt have an overload because QML is not strongly typed enough to recognize int vs QString (using variants for all)
bool TransactionsModel::setDataString(int row, const QString& roleString, const QVariant& value, int role)
{
    int colfromrole = getColumnFromRole(roleString);
    return setData(row, colfromrole, value, role);
    //return setData(row, getColumnFromRole(roleString), value, role);
}

int TransactionsModel::getColumnFromRole(const QString& role) const
{
    if (role == "Cost")
        return Column::Cost;
    else if (role == "Payer")
        return Column::Payer;
    else if (role == "Covering")
        return Column::Covering;
    else if (role == "Description")
        return Column::Description;

    return Column::COUNT;
}

bool TransactionsModel::addTransaction(const QString& payerName,
                                       double cost,
                                       const QString& description,
                                       const QStringList& coveringPids)
{
    Transaction newTransaction;
    newTransaction.payerPid = getPidFromInitials(payerName);
    newTransaction.cost = cost;
    newTransaction. description = description;
    newTransaction.coveringPids = getPidSetFromInitials(coveringPids);
    return addTransaction(std::move(newTransaction));
}

bool TransactionsModel::addTransaction(Transaction transaction)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    bool result = m_data.AddTransaction(std::move(transaction));
    endInsertRows();
    return result;
}

QString TransactionsModel::getInitialFromPid(int pid) const
{
    // TODO: These try catches are gross, think about what to do with them
    try {
        return m_data.GetPersonById(pid).initials;
    }
    catch(...) {}

    return QString();
}

QStringList TransactionsModel::getAllInitials() const
{
    QStringList result;
    for (int i = 0; i < m_data.NumPeople(); ++i)
    {
        result.append(m_data.GetPersonByIndex(i).initials);
    }
    return result;
}

QStringList TransactionsModel::getInitialsFromPidSet(const std::set<int>& pids) const
{
    QStringList result;
    for (int pid : pids)
    {
        result.append(getInitialFromPid(pid));
    }
    return result;
}

int TransactionsModel::getPidFromInitials(const QString& initials) const
{
    try {
        return m_data.GetPersonByInitials(initials).id;
    }
    catch (...) {}

    return -1;
}

std::set<int> TransactionsModel::getPidSetFromInitials(const QStringList& initials) const
{
    std::set<int> result;
    for (const QString& i : initials)
    {
        result.insert(getPidFromInitials(i));
    }
    return result;
}

QStringList TransactionsModel::getCoveringStringList(int row) const
{
    return getInitialsFromPidSet(m_data.GetTransactionByIndex(row).coveringPids);
}

int TransactionsModel::columnWidth(int c, const QFont* font)
{
        //Q_UNUSED(font);

    if (c < 0 || c >= Column::COUNT) {
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

void TransactionsModel::jsonRead(const QJsonObject& json)
{
    beginRemoveRows(QModelIndex(), 0, m_data.NumTransactions() - 1);
    m_data.ClearTransactions();
    endRemoveRows();

    QJsonArray transactionArray = json["transaction"].toArray();
    beginInsertRows(QModelIndex(), 0, transactionArray.count() - 1);
    for (const auto& element : transactionArray)
    {
        QJsonObject transactionObj = element.toObject();
        Transaction t;
        t.payerPid = transactionObj["payerpid"].toInt();
        t.description = transactionObj["description"].toString();
        t.cost = transactionObj["cost"].toDouble();

        QJsonArray transactionArray = transactionObj["coveringpids"].toArray();
        for (const auto& pidElement : transactionArray)
        {
            t.coveringPids.insert(pidElement.toInt());
        }

        m_data.AddTransaction(std::move(t));
    }
    endInsertRows();
}

void TransactionsModel::jsonWrite(QJsonObject& json) const
{
    QJsonArray transactionArray;
    for(int i = 0; i < m_data.NumTransactions(); ++i)
    {
        const auto& currTransaction = m_data.GetTransactionByIndex(i);

        QJsonObject curr;
        curr["payerpid"] = currTransaction.payerPid;
        curr["description"] = currTransaction.description;
        curr["cost"] = currTransaction.cost;

        QJsonArray coveringPidArray;
        for (auto pid : currTransaction.coveringPids)
        {
            coveringPidArray.append(pid);
        }
        curr["coveringpids"] = coveringPidArray;

        transactionArray.append(curr);
    }
    json["transaction"] = transactionArray;
}

bool TransactionsModel::isIndexValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() < rowCount();
}
