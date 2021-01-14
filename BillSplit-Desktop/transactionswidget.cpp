#include "transactionswidget.h"
#include "ui_transactionswidget.h"
#include "transactionstablemodel.h"
#include "transactioneditdialog.h"
//#include "comboboxitemdelegate.h"

TransactionsWidget::TransactionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransactionsWidget),
    m_dialog(new TransactionEditDialog(this))
{
    ui->setupUi(this);
    m_dialog->setModal(true);
}

TransactionsWidget::~TransactionsWidget()
{
    delete m_dialog;
    delete ui;
}

void TransactionsWidget::SetTransactionsModel(TransactionsTableModel* transactionsTableModel)
{
    m_model = transactionsTableModel;
    ui->tableView->setModel(m_model);

    ui->pushButtonView->setDisabled(true);
    QObject::connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]() { ui->pushButtonView->setDisabled(!ui->tableView->selectionModel()->hasSelection()); });
    QObject::connect(m_model, &TransactionsTableModel::modelReset,
        [this]() { ui->pushButtonView->setDisabled(!ui->tableView->selectionModel()->hasSelection()); });

    // Table formatting
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setStyleSheet("QHeaderView::section { background-color:gray }");

    // TODO: Reinstate this with your new models after some thought
    // Format second column to have a persistent combo box
    //    const int payerColumn = 1;
    //    ui->tableView->setItemDelegateForColumn(payerColumn, new ComboBoxItemDelegate(this));

    //    for (int row = 0; row < m_model->rowCount(); ++row) {
    //        ui->tableView->openPersistentEditor(m_model->index(row, payerColumn));
    //    }
}

void TransactionsWidget::SetTransactionModel(TransactionModel* transactionModel)
{
    m_dialog->SetModel(transactionModel);
    // TODO: for each comboboxitemdelegate set the model
    // TODO: make it so you can set these in either order
}

void TransactionsWidget::ViewSelected(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    m_model->loadToModel(index.row(), m_dialog->GetModel());
    m_dialog->SetMode(TransactionEditDialog::Mode::Edit);

    for (bool isValid = false; !isValid;) {
        m_dialog->exec();
        if (m_dialog->result() == TransactionEditDialog::CustomDialogCode::Save) {
            isValid = m_model->editFromModel(index.row(), m_dialog->GetModel());
        }
        else if (m_dialog->result() == TransactionEditDialog::CustomDialogCode::Delete) {
            m_model->removeRow(index.row());
            isValid = true;
        }
        else { // Cancel, do nothing
            isValid = true;
        }
    }
}

void TransactionsWidget::on_pushButtonNew_clicked()
{
    m_dialog->GetModel()->loadDefault();
    m_dialog->SetMode(TransactionEditDialog::Mode::Add);

    for (bool isValid = false; !isValid;) {
        m_dialog->exec();

        if (m_dialog->result() == TransactionEditDialog::CustomDialogCode::Save) {
            isValid = m_model->addFromModel(m_dialog->GetModel());
        }
        else { // Cancel or Delete, do nothing
            isValid = true;
        }
    }
}

void TransactionsWidget::on_pushButtonView_clicked()
{
    ViewSelected(ui->tableView->selectionModel()->currentIndex());
}
