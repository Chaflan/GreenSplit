#include "comboboxitemdelegate.h"
#include "transactionsmodel.h"
#include <QComboBox>

ComboBoxItemDelegate::ComboBoxItemDelegate(TransactionsModel* model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_model(model)
{
}

ComboBoxItemDelegate::~ComboBoxItemDelegate()
{
}

QWidget *ComboBoxItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);   // Currently no formatting
    Q_UNUSED(index);    // All indices get the same list

    // Create the combobox and populate it
    QComboBox* cb = new QComboBox(parent);
    cb->addItems(m_model->getAllInitials());
    return cb;
}

void ComboBoxItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);

    // get the index of the text in the combobox that matches the current value of the item
    const QString currentText = m_model->data(index, Qt::DisplayRole).toString();
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
