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
}

QStringList TransactionModel::getAllPeople() const
{
    if (m_data) {
        return m_data->GetIdentifierList();
    }

    return QStringList();
}

void TransactionModel::setData(DataCoreObject* data)
{
    if (data != m_data) {
        m_data = data;
        assert(m_data);
        emit dataChanged();

        // TODO: Try to connect this directly, no call to connect
        QObject::connect(m_data, &DataCoreObject::identifierListChanged, this, &TransactionModel::allPeopleChanged);

        // TODO: Is this call necessary?
        emit allPeopleChanged();

        for (int i = 0; i < m_data->NumPeople(); ++i) {
            m_coveringList.append(new PersonCheck(m_data->GetPersonIdentifier(i), true, this));  // TODO: Is this right?  Qt parent system is weird
        }
        emit coveringListChanged();

        clear();
    }
}

void TransactionModel::setPayerName(QString payerName)
{
    if (payerName != m_payerName) {
        for (int i = 0; i < m_data->NumPeople(); ++i) {
            if (payerName == m_data->GetPersonIdentifier(i)) {
                m_payerIndex = i;
                m_payerName = std::move(payerName);
                emit payerIndexChanged();
                emit payerNameChanged();
                break;
            }
        }
    }
}

void TransactionModel::setPayerIndex(int payerIndex)
{
    if (payerIndex != m_payerIndex) {
        m_payerIndex = payerIndex;
        m_payerName = m_data->GetPersonIdentifier(m_payerIndex);
        emit payerIndexChanged();
        emit payerNameChanged();
    }
}

void TransactionModel::setCost(double cost)
{
    if (cost != m_cost) {
        m_cost = cost;
        emit costChanged();
    }
}

void TransactionModel::setDescription(QString description)
{
    if (m_description != description) {
        m_description = std::move(description);
        emit descriptionChanged();
    }
}

void TransactionModel::load(double cost, QString payer, const QStringList& covering, QString description)
{
    setPayerName(std::move(payer));
    setCost(cost);
    setDescription(std::move(description));
    setCoveringStringList(covering);
}

void TransactionModel::clear()
{
    setPayerIndex(0);
    setCost(0);
    setDescription("");

    bool coveringListWasChanged = false;
    for (auto& personCheckPtr : m_coveringList) {
        if (personCheckPtr->getCheckStatus() == false) {
            personCheckPtr->setCheckStatus(true);
            coveringListWasChanged = true;
        }
    }

    if (coveringListWasChanged) {
        emit coveringListChanged();
    }
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
    // TODO: Can use the cache here too
    bool coveringListWasChanged = false;
    for (int i = 0; i < m_coveringList.length(); ++i) {
        bool newCheckStatus = stringList.contains(m_coveringList[i]->getName());
        if (m_coveringList[i]->getCheckStatus() != newCheckStatus) {
            m_coveringList[i]->setCheckStatus(newCheckStatus);
            coveringListWasChanged = true;
        }
    }
    if (coveringListWasChanged) {
        emit coveringListChanged();
    }
}

