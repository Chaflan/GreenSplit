#include "transactionmodel.h"
#include <QDebug>

// TODO: data assert consideration

PersonCheck::PersonCheck(QObject* parent) :
    QObject(parent)
{
}

PersonCheck::PersonCheck(QString name, bool checkStatus, QObject* parent) :
    QObject(parent),
    m_name(std::move(name)),
    m_checkStatus(checkStatus)
{
}

void PersonCheck::setName(QString name)
{
    if (name != m_name) {
        m_name = std::move(name);
        emit nameChanged();
    }
}

void PersonCheck::setCheckStatus(bool checkStatus)
{
    if (checkStatus != m_checkStatus) {
        m_checkStatus = checkStatus;
        emit checkStatusChanged();
    }
}

//--------------------------------------------------------

TransactionModel::TransactionModel(QObject *parent) :
    QObject(parent)
{
    setCostStr("0");
}

void TransactionModel::setDataCore(DataCoreObject* data)
{
    if (data != m_data) {
        m_data = data;
        assert(m_data);
        emit dataSet();
    }
}

void TransactionModel::setPayerIndex(int payerIndex)
{
    if (m_data && payerIndex != m_payerIndex) {
        if (m_allPeople.isEmpty()) {
            qDebug() << "Error - TransactionModel::setPayerIndex - called when m_allPeople is empty";
        }
        m_payerIndex = payerIndex;
        emit payerIndexChanged();
    }
}

void TransactionModel::setCostDbl(double cost)
{
    if (m_data && cost != m_costDbl) {
        m_costDbl = cost;
        m_costStr = QString::number(m_costDbl, 'f', 2);
        emit costChanged();
    }
}

void TransactionModel::setCostStr(const QString& cost)
{
    if (m_data && cost != m_costStr) {
        m_costStr = cost;
        m_costDbl = m_costStr.toDouble();
        emit costChanged();
    }
}

void TransactionModel::setDescription(const QString& description)
{
    if (m_data && description != m_description) {
        m_description = description;
        emit descriptionChanged();
    }
}

void TransactionModel::load(double cost, const QString& payer, const QStringList& covering, const QString& description)
{
    assert(m_data);

    setCostDbl(cost);
    setDescription(description);

    m_allPeople.clear();
    m_coveringList.clear();
    for (int i = 0; i < m_data->numPeople(); ++i) {
        const QString& currId = m_data->getPersonIdentifier(i);
        m_allPeople.append(currId);
        m_coveringList.append(new PersonCheck(currId, covering.contains(currId), this));   // TODO: How does deleting work here?
        if (payer == currId) {
            setPayerIndex(i);
        }
    }
    emit allPeopleChanged();
    emit coveringListChanged();
}

void TransactionModel::loadDefault()
{
    assert(m_data);

    setCostDbl(0);
    setDescription("");

    m_allPeople.clear();
    m_coveringList.clear();
    for (int i = 0; i < m_data->numPeople(); ++i) {
        const QString& currId = m_data->getPersonIdentifier(i);
        m_allPeople.append(currId);
        m_coveringList.append(new PersonCheck(currId, true, this));   // TODO: How does deleting work here?
    }
    setPayerIndex(0);

    emit allPeopleChanged();
    emit coveringListChanged();
}

QString TransactionModel::getPayerName() const
{
    if (m_allPeople.isEmpty()) {
        qDebug() << "Error - TransactionModel::getPayerName - called when m_allPeople is empty";
        return QString();
    }

    return m_allPeople[m_payerIndex];
}

QStringList TransactionModel::getCoveringStringList() const
{
    QStringList ret;
    for (int i = 0; i < m_coveringList.length(); ++i) {
        if (m_coveringList[i]->getCheckStatus())
            ret.append(m_coveringList[i]->getName());
    }
    return ret;
}

void TransactionModel::setCheck(int index, bool check)
{
    if (index >= m_coveringList.length()) {
        qDebug() << "Error - TransactionModel::setCheck - trying to check an out of bounds element";
    } else {
        m_coveringList[index]->setCheckStatus(check);
    }
}
