#include "transactioneditdialog.h"
#include "ui_transactioneditdialog.h"
#include "transactionmodel.h"
#include <QDebug>

// TODO: you are not deleting things

TransactionEditDialog::TransactionEditDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::transactioneditdialog)
{
    ui->setupUi(this);
}

TransactionEditDialog::~TransactionEditDialog()
{
    delete ui;
}

void TransactionEditDialog::SetModel(TransactionModel* model)
{
    m_model = model;
}

int TransactionEditDialog::exec()
{
    if (!m_model) {
        qDebug() << "Error - TransactionEditDialog::exec - called with null model";
    } else {
        // load data from the model to the view

        ui->doubleSpinBoxCost->setValue(m_model->getCostDbl());
        ui->lineEditDescription->setText(m_model->getDescription());

        ui->comboBoxPayer->clear();
        for (const QString& name : m_model->getAllPeople()) {
            ui->comboBoxPayer->addItem(name);
        }
        ui->comboBoxPayer->setCurrentIndex(m_model->getPayerIndex());

        ui->listWidget->clear();
        const auto& coveringList = m_model->getCoveringList();
        for(int i = 0; i < coveringList.length(); ++i)
        {
            ui->listWidget->addItem(coveringList[i]->getName());
            QListWidgetItem* item = ui->listWidget->item(i);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState((coveringList[i]->getCheckStatus() ? Qt::Checked : Qt::Unchecked));
        }
    }

    return QDialog::exec();
}

void TransactionEditDialog::SetMode(TransactionEditDialog::Mode mode)
{
    ui->pushButtonDelete->setDisabled(mode == Mode::Add);
}

void TransactionEditDialog::on_pushButtonSave_clicked()
{
    // Save view data to the model
    m_model->setCostDbl(ui->doubleSpinBoxCost->value());
    m_model->setDescription(ui->lineEditDescription->text());
    m_model->setPayerIndex(ui->comboBoxPayer->currentIndex());

    for(int i = 0; i < ui->listWidget->count(); ++i) {
        m_model->setCheck(i, ui->listWidget->item(i)->checkState() == Qt::Checked);
    }

    this->done(CustomDialogCode::Save);
}

void TransactionEditDialog::on_pushButtonDelete_clicked()
{
    this->done(CustomDialogCode::Delete);
}

void TransactionEditDialog::on_pushButtonCancel_clicked()
{
    this->done(CustomDialogCode::Cancel);
}
