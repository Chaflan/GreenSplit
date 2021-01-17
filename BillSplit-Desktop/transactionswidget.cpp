#include "transactionswidget.h"
#include "ui_transactionswidget.h"
#include "transactionstablemodel.h"
#include "transactioneditdialog.h"

TransactionsWidget::TransactionsWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TransactionsWidget),
    m_dialog(new TransactionEditDialog(this))
{
    m_ui->setupUi(this);
    m_dialog->setModal(true);
}

TransactionsWidget::~TransactionsWidget()
{
    delete m_ui;
}

void TransactionsWidget::SetTransactionsModel(TransactionsTableModel* transactionsTableModel)
{
    m_model = transactionsTableModel;
    m_ui->tableView->setModel(m_model);

    m_ui->pushButtonView->setDisabled(true);
    QObject::connect(m_ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]() { m_ui->pushButtonView->setDisabled(!m_ui->tableView->selectionModel()->hasSelection()); });
    QObject::connect(m_model, &TransactionsTableModel::modelReset,
        [this]() { m_ui->pushButtonView->setDisabled(!m_ui->tableView->selectionModel()->hasSelection()); });

    // Table formatting
    m_ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_ui->tableView->horizontalHeader()->setStretchLastSection(true);
    m_ui->tableView->setStyleSheet("QHeaderView::section { background-color:gray }");
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
    ViewSelected(m_ui->tableView->selectionModel()->currentIndex());
}
