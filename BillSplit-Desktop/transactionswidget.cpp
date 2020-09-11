#include "transactionswidget.h"
#include "ui_transactionswidget.h"
#include "transactionsmodel.h"
#include "transactioneditdialog.h"
#include "comboboxitemdelegate.h"
#include <QMessageBox>

TransactionsWidget::TransactionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransactionsWidget),
    model(nullptr)
{
    ui->setupUi(this);
}

TransactionsWidget::~TransactionsWidget()
{
    delete ui;
}

void TransactionsWidget::SetTransactionsModel(TransactionsModel* transactionsModel)
{
    model = transactionsModel;
    ui->tableView->setModel(model);

    ui->pushButtonView->setDisabled(true);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]()
        {
            ui->pushButtonView->setDisabled(!ui->tableView->selectionModel()->hasSelection());
        });

    // Table formatting
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setStyleSheet("QHeaderView::section { background-color:gray }");

    // Format second column to have a persistent combo box
    const int payerColumn = 1;
    ui->tableView->setItemDelegateForColumn(payerColumn, new ComboBoxItemDelegate(model, this));

    for (int row = 0; row < model->rowCount(); ++row) {
        ui->tableView->openPersistentEditor(model->index(row, payerColumn));
    }
}

void TransactionsWidget::ViewSelected(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    QString payerName = model->data(index, TransactionsModel::Roles::PayerRole).toString();
    double cost = model->data(index, TransactionsModel::Roles::CostRole).toDouble();
    QString description = model->data(index, TransactionsModel::Roles::DescriptionRole).toString();
    QStringList coveringNames = model->data(index, TransactionsModel::Roles::CoveringInitialsRole).toStringList();

    for (bool isValid = false; !isValid;)
    {
        TransactionEditDialog dialog(payerName,
                                     cost,
                                     description,
                                     coveringNames,
                                     model->getAllInitials(),
                                     TransactionEditDialog::Mode::Edit);
        dialog.setModal(true);
        dialog.exec();

        if (dialog.result() == TransactionEditDialog::CustomDialogCode::Save)
        {
            if (payerName != dialog.m_payer) {
                model->setData(index, std::move(dialog.m_payer), TransactionsModel::Roles::PayerPidRole);
            }

            if (coveringNames != dialog.m_coveringNames) {
                model->setData(index, dialog.m_coveringNames, TransactionsModel::Roles::CoveringInitialsRole);
            }

            if (description != dialog.m_description) {
                model->setData(index, dialog.m_description, TransactionsModel::Roles::DescriptionRole);
            }

            if (cost != dialog.m_cost) {
                model->setData(index, dialog.m_cost, TransactionsModel::Roles::CostRole);
            }
        }
        else if (dialog.result() == TransactionEditDialog::CustomDialogCode::Delete)
        {
            model->removeRow(index.row());
            isValid = true;
        }
        else // Cancel, do nothing
        {
            isValid = true;
        }
    }
}

void TransactionsWidget::on_pushButtonNew_clicked()
{
    for (bool isValid = false; !isValid;)
    {
        TransactionEditDialog dialog(model->getAllInitials(), TransactionEditDialog::Mode::Add);
        dialog.setModal(true);
        dialog.exec();

        if (dialog.result() == TransactionEditDialog::CustomDialogCode::Save)
        {
            if (model->addTransaction(dialog.m_payer, dialog.m_cost, dialog.m_description, dialog.m_coveringNames))
            {
                isValid = true;
            }
            else
            {
                QMessageBox message(QMessageBox::Icon::Critical, "Error",
                    "This transaction is invalid.");
                message.exec();
            }
        }
        else // Cancel or Delete, do nothing
        {
            isValid = true;
        }
    }
}

void TransactionsWidget::on_pushButtonView_clicked()
{
    ViewSelected(ui->tableView->selectionModel()->currentIndex());
}
