#include "comboboxitemdelegate.h"
#include "transactionstablemodel.h"
#include <QComboBox>

ComboBoxItemDelegate::ComboBoxItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

ComboBoxItemDelegate::~ComboBoxItemDelegate()
{
}

QWidget* ComboBoxItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);   // Currently no formatting
    Q_UNUSED(index);    // All indices get the same list

    // Create the combobox and populate it
    QComboBox* cb = new QComboBox(parent);
    if (m_model) {
        //cb->addItems(m_model->());
    }
    return cb;
}

void ComboBoxItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    Q_UNUSED(index);

    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);

    // get the index of the text in the combobox that matches the current value of the item
    const QString currentText;// = m_model->data(index, Qt::DisplayRole).toString();
    const int cbIndex = cb->findText(currentText);

    // if it is valid, adjust the combobox
    if (cbIndex >= 0)
       cb->setCurrentIndex(cbIndex);
}

void ComboBoxItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);

    model->setData(index, cb->currentText(), Qt::EditRole);
}

void ComboBoxItemDelegate::setModel(TransactionsTableModel* model)
{
    Q_UNUSED(model);
}

void ComboBoxItemDelegate::updateModel()
{
    // For connecting with changes to allPeople
}
