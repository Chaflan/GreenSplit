#include "transactionswidget.h"
#include "ui_transactionswidget.h"
#include "transactionstablemodel.h"
#include "transactioneditdialog.h"

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
}

void TransactionsWidget::SetTransactionModel(TransactionModel* transactionModel)
{
    m_dialog->SetModel(transactionModel);
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
