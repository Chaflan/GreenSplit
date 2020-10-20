#include "peoplemodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>      // This requires widgets, see if there is another way to do this
#include <QGuiApplication>
#include <QtGlobal>

PeopleModel::PeopleModel(DataCoreOld& dataCore, QObject* parent) :
    QAbstractTableModel(parent),
    m_data(dataCore),
    m_columnWidths(Column::COUNT)
{
}

int PeopleModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_data.NumPeople();
}

int PeopleModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : Column::COUNT;
}

QVariant PeopleModel::data(const QModelIndex& index, int role) const
{
    if (isIndexValid(index))
    {
        auto person = m_data.GetPersonByIndex(index.row());

        switch (role)
        {
            case Qt::DisplayRole:
            {
                switch (index.column())
                {
                    case Column::Initials: return person.initials;
                    case Column::Name: return person.name;
                }
                break;
            }
            case Roles::IdRole: return person.id;
            case Roles::InitialsRole: return person.initials;
            case Roles::NameRole: return person.name;
        }
    }

    return QVariant();
}

// TODO: Is this really the best way to do this, recursion?  Maybe we need an overload
// The use of roles like this is really not as the model was intended.  Perhaps we
// Should instead have a different method that can be called directly.  EditRoles?

bool PeopleModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // TODO!!!!
    // When reimplementing the setData() function, this signal must be emitted explicitly.
    // https://doc.qt.io/qt-5/qabstractitemmodel.html#dataChanged
    // THIS IS TRUE OF MANY OF THESE MODEL OVERLOADS

    if (!isIndexValid(index)) {
        return false;
    }

    if (role == Qt::EditRole) {
        switch(index.column()) {
            case Column::Initials: return setData(index, value, Roles::InitialsRole);
            case Column::Name: return setData(index, value, Roles::NameRole);
        }

    } else if (role == Roles::InitialsRole || role == Roles::NameRole) {
        Person personCopy(m_data.GetPersonByIndex(index.row()));

        switch (role) {
            case Roles::InitialsRole: personCopy.initials = value.toString(); break;
            case Roles::NameRole: personCopy.name = value.toString(); break;
        }

        if (m_data.EditPerson(index.row(), personCopy)) {
            emit dataChanged(index, index);
            emit allPeopleChanged();
            return true;
        } else {
            if (role == Roles::InitialsRole) {

                // THIS CRASHES QML
//                QMessageBox msg;
//                msg.setText("Initials must be unique");
//                msg.exec();
                //TODO: Hook widgets into this signal below

                emit signalError("Initials must be unique.");
            }
        }
    }

    return false;
}

bool PeopleModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    bool result = m_data.DeletePeople(row, count);
    endRemoveRows();

    if (result) {
        emit allPeopleChanged();
    }

    return result;
}

QVariant PeopleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case Column::Initials: return "Initials";
            case Column::Name: return "Name";
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags PeopleModel::flags(const QModelIndex& index) const
{
    // TODO: I don't understand the purpose of this return variable.
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

// TODO: Might be able to remove this at the end if you don't use the roles
QHash<int, QByteArray> PeopleModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();
    roles[Roles::InitialsRole] = "initialsRole";
    roles[Roles::NameRole] = "nameRole";
    return roles;
}

// TODO: Combine this with the header data
int PeopleModel::getColumnFromRole(const QString& role) const
{
    if (role == "Initials")
        return Column::Initials;
    else if (role == "Name")
        return Column::Name;

    return Column::COUNT;
}

QVariant PeopleModel::data(int row, int column, int role) const
{
    return data(index(row, column), role);
}

QVariant PeopleModel::data(int row, const QString& roleString, int role) const
{
    return data(row, getColumnFromRole(roleString), role);
}

bool PeopleModel::setData(int row, int column, const QVariant& value, int role)
{
    return setData(index(row, column), value, role);
}

bool PeopleModel::setDataString(int row, const QString& roleString, const QVariant& value, int role)
{
    return setData(row, getColumnFromRole(roleString), value, role);
}

bool PeopleModel::addPerson(QString initials, QString name)
{
    if (initials.isEmpty()) {
        emit signalError("Initials cannot be empty.");
        return false;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    bool result = m_data.AddPerson({std::move(initials), std::move(name)});
    endInsertRows();

    if (result) {
        emit allPeopleChanged();
    } else {
        emit signalError("Initials must be unique.");
    }

    return result;
}

// TODO: Resize on value change
int PeopleModel::columnWidth(int c, const QFont* font)
{
    Q_UNUSED(font);

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

// TODO: This algo is slow with use of contains
QList<bool> PeopleModel::getSelectionFromPeople(const QStringList& people) const
{
    QList<bool> ret;
    for (int i = 0, trueCount = 0; i < rowCount(); ++i) {
        if (trueCount == people.size() || !people.contains(m_data.GetPersonByIndex(i).initials)) {
            ret.append(false);
        } else {
            ret.append(true);
            trueCount++;
        }
    }
    return ret;
}

QStringList PeopleModel::getSelectedPeople(const QList<bool>& selection) const
{
    QStringList ret;
    for (int i = 0; i < rowCount() && i < selection.count(); ++i) {
        if (selection[i]) {
            ret.append(m_data.GetPersonByIndex(i).initials);
        }
    }
    return ret;
}

QStringList PeopleModel::getAllPeople() const
{
    QStringList ret;
    for (int i = 0; i < m_data.NumPeople(); ++i) {
        ret.append(m_data.GetPersonByIndex(i).initials);
    }
    return ret;
}

void PeopleModel::jsonRead(const QJsonObject& json)
{
    beginRemoveRows(QModelIndex(), 0, m_data.NumPeople() - 1);
    m_data.ClearPeople();
    endRemoveRows();

    QJsonArray peopleArray = json["people"].toArray();
    beginInsertRows(QModelIndex(), 0, peopleArray.count() - 1);
    for (const auto& element : peopleArray)
    {
        QJsonObject personObj = element.toObject();
        m_data.AddPerson({personObj["initials"].toString(), personObj["name"].toString()});
    }
    endInsertRows();
}

void PeopleModel::jsonWrite(QJsonObject& json) const
{
    QJsonArray peopleArray;
    for(int i = 0; i < m_data.NumPeople(); ++i)
    {
        QJsonObject curr;
        curr["initials"] = m_data.GetPersonByIndex(i).initials;
        curr["name"] = m_data.GetPersonByIndex(i).name;
        peopleArray.append(curr);
    }
    json["people"] = peopleArray;
}

bool PeopleModel::isIndexValid(const QModelIndex& index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}
