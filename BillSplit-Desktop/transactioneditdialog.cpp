#include "transactioneditdialog.h"
#include "ui_transactioneditdialog.h"
#include "transactionmodel.h"

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
    
    LoadFromModelCost();
    LoadFromModelDescription();
    LoadFromModelAllPeople();
    LoadFromModelPayerIndex();
    LoadFromModelChecks();

    QObject::connect(m_model, &TransactionModel::costChanged, this, &TransactionEditDialog::LoadFromModelCost);
    QObject::connect(m_model, &TransactionModel::descriptionChanged, this, &TransactionEditDialog::LoadFromModelDescription);
    QObject::connect(m_model, &TransactionModel::allPeopleChanged, this, &TransactionEditDialog::LoadFromModelAllPeople);
    QObject::connect(m_model, &TransactionModel::payerIndexChanged, this, &TransactionEditDialog::LoadFromModelPayerIndex);
    QObject::connect(m_model, &TransactionModel::coveringChecksChanged, this, &TransactionEditDialog::LoadFromModelChecks);
}

void TransactionEditDialog::LoadFromModelCost()
{
    ui->doubleSpinBoxCost->setValue(m_model->getCost());
}

void TransactionEditDialog::LoadFromModelDescription()
{
    ui->lineEditDescription->setText(m_model->getDescription());
}

void TransactionEditDialog::LoadFromModelAllPeople()
{
    ui->comboBoxPayer->clear();
    QStringList allPeople = m_model->getAllPeople();
    for (const QString& name : allPeople) {
        ui->comboBoxPayer->addItem(name);
    }

    ui->listWidget->clear();
    ui->listWidget->addItems(allPeople);
}

void TransactionEditDialog::LoadFromModelPayerIndex()
{
    if (!m_model->getPayerName().isEmpty()) {
        ui->comboBoxPayer->setCurrentIndex(m_model->getPayerIndex());
    }
}

void TransactionEditDialog::LoadFromModelChecks()
{
    const auto& coveringList = m_model->getCoveringList();
    for(int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState((coveringList[i]->getCheckStatus() ? Qt::Checked : Qt::Unchecked));
    }
}

void TransactionEditDialog::SetMode(TransactionEditDialog::Mode mode)
{
    ui->pushButtonDelete->setDisabled(mode == Mode::Add);
}

void TransactionEditDialog::on_pushButtonSave_clicked()
{
    // Save all this data to the model
    m_model->setCost(ui->doubleSpinBoxCost->value());
    m_model->setDescription(ui->lineEditDescription->text());
    m_model->setPayerIndex(ui->comboBoxPayer->currentIndex());

    // Pull out the model, edit it, and resubmit it.  Very wasteful.
    auto coveringList = m_model->getCoveringList();
    for(int i = 0; i < ui->listWidget->count(); ++i) {
        coveringList[i]->setCheckStatus(ui->listWidget->item(i)->checkState() == Qt::Checked);
    }
    m_model->setCoveringList(coveringList);

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
