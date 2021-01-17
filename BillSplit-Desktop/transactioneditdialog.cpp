#include "transactioneditdialog.h"
#include "ui_transactioneditdialog.h"
#include "transactionmodel.h"
#include <QDebug>

TransactionEditDialog::TransactionEditDialog(QWidget* parent) :
    QDialog(parent),
    m_ui(new Ui::transactioneditdialog)
{
    m_ui->setupUi(this);
    SetMode(Mode::Add);
}

TransactionEditDialog::~TransactionEditDialog()
{
    delete m_ui;
}

void TransactionEditDialog::SetModel(TransactionModel* model)
{
    assert(model);
    m_model = model;
}

void TransactionEditDialog::SetMode(TransactionEditDialog::Mode mode)
{
    m_ui->pushButtonDelete->setDisabled(mode == Mode::Add);
}

int TransactionEditDialog::exec()
{
    // load data from the model to the view

    m_ui->doubleSpinBoxCost->setValue(m_model->getCostDbl());
    m_ui->lineEditDescription->setText(m_model->getDescription());

    m_ui->comboBoxPayer->clear();
    QStringList tempLValue = m_model->getAllPeople();
    for (const QString& name : tempLValue) {
        m_ui->comboBoxPayer->addItem(name);
    }
    m_ui->comboBoxPayer->setCurrentIndex(m_model->getPayerIndex());

    m_ui->listWidget->clear();
    const auto& coveringList = m_model->getCoveringList();
    for(int i = 0; i < coveringList.length(); ++i)
    {
        m_ui->listWidget->addItem(coveringList[i]->getName());
        QListWidgetItem* item = m_ui->listWidget->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState((coveringList[i]->getCheckStatus() ? Qt::Checked : Qt::Unchecked));
    }

    return QDialog::exec();
}

void TransactionEditDialog::on_pushButtonSave_clicked()
{
    // Save view data to the model
    m_model->setCostDbl(m_ui->doubleSpinBoxCost->value());
    m_model->setDescription(m_ui->lineEditDescription->text());
    m_model->setPayerIndex(m_ui->comboBoxPayer->currentIndex());

    for(int i = 0; i < m_ui->listWidget->count(); ++i) {
        m_model->setCheck(i, m_ui->listWidget->item(i)->checkState() == Qt::Checked);
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
