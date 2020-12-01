#include "transactionmodel.h"
#include <QDebug>

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

QString TransactionModel::getCostStr() const
{
    return QString::number(m_cost, 'f', 2);
}

QStringList TransactionModel::getAllPeople() const
{
    if (m_data) {
        return m_data->getIdentifierList();
    }

    return QStringList();
}

void TransactionModel::setDataCore(DataCoreObject* data)
{
    if (data != m_data) {
        m_data = data;
        assert(m_data);
        emit dataSet();

        QObject::connect(m_data, &DataCoreObject::peopleChanged,
            this, &TransactionModel::identifierListChanged);
//        QObject::connect(m_data, &DataCoreObject::modelReset,
//            this, &TransactionModel::identifierListChanged);  // TODO: I think this is sufficient

        // TODO: Is this call necessary?
        emit allPeopleChanged();

        for (int i = 0; i < m_data->numPeople(); ++i) {
            PersonCheck* newPersonCheck = new PersonCheck(m_data->getPersonIdentifier(i), true, this);
            QObject::connect(newPersonCheck, &PersonCheck::checkStatusChanged, [this](){ emit coveringChecksChanged(); });
            m_coveringList.append(newPersonCheck);  // TODO: Is this right?  Qt parent system is weird
        }
        emit coveringListChanged();

        clear();
    }
}

void TransactionModel::setPayerName(QString payerName)
{
    if (m_data && payerName != m_payerName) {
        for (int i = 0; i < m_data->numPeople(); ++i) {
            if (payerName == m_data->getPersonIdentifier(i)) {
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
    if (m_data && payerIndex != m_payerIndex) {
        m_payerIndex = payerIndex;
        m_payerName = payerIndex == -1 ? "" : m_data->getPersonIdentifier(m_payerIndex);
        emit payerIndexChanged();
        emit payerNameChanged();
    }
}

void TransactionModel::setCost(double cost)
{
    if (m_data && cost != m_cost) {
        m_cost = cost;
        emit costChanged();
    }
}

void TransactionModel::setCostStr(QString cost)
{
    m_cost = cost.toDouble();
}

// TODO: probably should not use value here since it is most often not moved
void TransactionModel::setDescription(QString description)
{
    if (m_data && m_description != description) {
        m_description = std::move(description);
        emit descriptionChanged();
    }
}

void TransactionModel::setCoveringList(const QList<PersonCheck*>& coveringList)
{
    if (m_data) {
        if (m_coveringList.length() != coveringList.length()) {
            qDebug() << "Error - TransactionModel::setCoveringList - coveringLists unequal length";
        } else {
            bool coveringListHasChanged = false;
            for (int i = 0; i < coveringList.length(); ++i) {
                if (m_coveringList[i]->getCheckStatus() != coveringList[i]->getCheckStatus()) {
                    m_coveringList[i]->setCheckStatus(coveringList[i]->getCheckStatus());
                    coveringListHasChanged = true;
                }
            }

            if (coveringListHasChanged) {
                emit coveringListChanged();
            }
        }
    }
}

void TransactionModel::load(double cost, QString payer, const QStringList& covering, QString description)
{
    assert(m_data);
    setPayerName(std::move(payer));
    setCost(cost);
    setDescription(std::move(description));
    setCoveringStringList(covering);
}

void TransactionModel::clear()
{
    assert(m_data);
    setDefaultPayer();
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
    assert(m_data);
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
    assert(m_data);
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

void TransactionModel::identifierListChanged()
{
    // Update covering list.  Assumes same order, which is true in this design
    bool coveringListWasChanged = false;
    for (int i = 0; i < m_data->numPeople(); ++i) {
        if (i >= m_coveringList.size()) {
            PersonCheck* newPersonCheck = new PersonCheck(m_data->getPersonIdentifier(i), true, this);
            QObject::connect(newPersonCheck, &PersonCheck::checkStatusChanged, [this](){ emit coveringChecksChanged(); });
            m_coveringList.insert(i, newPersonCheck);
            coveringListWasChanged = true;
            continue;
        }

        if (m_data->getPersonIdentifier(i) == m_coveringList[i]->getName()) {
            continue;
        }

        bool found = false;
        for (int j = i + 1; j < m_coveringList.size(); ++j) {
            if (m_data->getPersonIdentifier(i) == m_coveringList[j]->getName()) {
                m_coveringList.erase(m_coveringList.begin() + i, m_coveringList.begin() + j);  // TODO: does this delete?
                if (i <= m_payerIndex && m_payerIndex < j) {
                    setDefaultPayer();
                }
                coveringListWasChanged = true;
                found = true;
                break;
            }
        }

        if (!found) {
            PersonCheck* newPersonCheck = new PersonCheck(m_data->getPersonIdentifier(i), true, this);
            QObject::connect(newPersonCheck, &PersonCheck::checkStatusChanged, [this](){ emit coveringChecksChanged(); });
            m_coveringList.insert(i, newPersonCheck);
            coveringListWasChanged = true;
        }
    }

    if (coveringListWasChanged) {
        emit coveringListChanged();
    }

    emit allPeopleChanged();
}

void TransactionModel::setDefaultPayer()
{
    if (m_data->numPeople() > 0) {
        setPayerIndex(0);
    } else {
        setPayerIndex(-1);
    }
}

