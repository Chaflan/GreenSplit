#include "transactionmodel.h"

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
    m_name = std::move(name);
    emit nameChanged();
}

void PersonCheck::setCheckStatus(bool checkStatus)
{
    m_checkStatus = checkStatus;
    emit checkStatusChanged();
}

//--------------------------------------------------------

TransactionModel::TransactionModel(QObject *parent) :
    QObject(parent)
{
}

//TransactionModel::TransactionModel(DataCore& dataCore, QObject* parent):
//    QObject(parent),
//    m_data(dataCore)
//{
//    // TODO: hook all people to people model change emits
//    for (int i = 0; i < m_data->NumPeople(); ++i) {
//        m_allPeople.append(m_data->GetPersonByIndex(i).initials);
//        m_coveringList.append(new PersonCheck(m_allPeople[i], true, this));  // TODO: Is this right?  Qt parent system is weird
//    }
//    emit allPeopleChanged();
//    emit coveringListChanged();

//    clear();
//}

void TransactionModel::initialize(DataCore* data)
{
    m_data = data;
    assert(m_data);

    // TODO: hook all people to people model change emits
    for (int i = 0; i < m_data->NumPeople(); ++i) {
        m_allPeople.append(m_data->GetPersonByIndex(i).initials);
        m_coveringList.append(new PersonCheck(m_allPeople[i], true, this));  // TODO: Is this right?  Qt parent system is weird
    }
    emit allPeopleChanged();
    emit coveringListChanged();

    clear();
}

void TransactionModel::setPayerName(QString payerName)
{
    for (int i = 0; i < m_data->NumPeople(); ++i) {
        if (payerName == m_data->GetPersonByIndex(i).initials) {
            m_payerIndex = i;
            m_payerName = std::move(payerName);
            emit payerIndexChanged();
            emit payerNameChanged();
            break;
        }
    }
}

void TransactionModel::setPayerIndex(int payerIndex)
{
    if (payerIndex >= 0 && payerIndex < m_data->NumPeople()) {
        m_payerIndex = payerIndex;
        m_payerName = m_data->GetPersonByIndex(m_payerIndex).initials;
        emit payerIndexChanged();
        emit payerNameChanged();
    }
}

void TransactionModel::setCost(double cost)
{
    m_cost = cost;
    emit costChanged();
}

void TransactionModel::setDescription(QString description)
{
    m_description = std::move(description);
    emit descriptionChanged();
}

void TransactionModel::load(QString payerName, double cost, QString description, const QStringList& covering)
{
    setPayerName(std::move(payerName));
    setCost(cost);
    setDescription(std::move(description));
    setCoveringStringList(covering);
}

void TransactionModel::clear()
{
    setPayerIndex(0);
    setCost(0);
    setDescription("");

    for (auto& personCheckPtr : m_coveringList) {
        personCheckPtr->setCheckStatus(true);
    }
    emit coveringListChanged();
}

QStringList TransactionModel::getCoveringStringList() const
{
    // TODO: Cache this
    QStringList ret;
    for (int i = 0; i < m_coveringList.length(); ++i) {
        if (m_coveringList[i]->getCheckStatus())
            ret.append(m_coveringList[i]->getName());
    }
    return ret;
}

void TransactionModel::setCoveringStringList(const QStringList& stringList)
{
    for (int i = 0; i < m_coveringList.length(); ++i) {
        m_coveringList[i]->setCheckStatus(stringList.contains(m_coveringList[i]->getName()));
    }
    emit coveringListChanged();
}


