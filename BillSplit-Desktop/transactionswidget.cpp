#include "transactionswidget.h"
#include "ui_transactionswidget.h"
#include "transactionsmodel.h"
#include "transactioneditdialog.h"
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
    ui->listView->setModel(model);

    ui->pushButtonView->setDisabled(true);
    connect(ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]()
        {
            ui->pushButtonView->setDisabled(!ui->listView->selectionModel()->hasSelection());
        });
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
    ViewSelected(ui->listView->selectionModel()->currentIndex());
}

void TransactionsWidget::on_listView_doubleClicked(const QModelIndex &index)
{
    ViewSelected(index);
}
