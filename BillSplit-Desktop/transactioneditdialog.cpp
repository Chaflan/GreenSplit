#include "transactioneditdialog.h"
#include "ui_transactioneditdialog.h"

TransactionEditDialog::TransactionEditDialog(const QStringList& allNames,
                                             TransactionEditDialog::Mode mode,
                                             QWidget* parent) :
    QDialog(parent),
    ui(new Ui::transactioneditdialog)
{
    SetAllNames(allNames);
    SetMode(mode);
}

TransactionEditDialog::TransactionEditDialog(QString payer,
                                             double cost,
                                             QString description,
                                             QStringList coveringNames,
                                             const QStringList& allNames,
                                             Mode mode,
                                             QWidget *parent) :
    QDialog(parent),
    ui(new Ui::transactioneditdialog),
    m_payer(std::move(payer)),
    m_cost(cost),
    m_description(std::move(description)),
    m_coveringNames(std::move(coveringNames))
{
    ui->setupUi(this);
    ui->doubleSpinBoxCost->setValue(m_cost);
    ui->lineEditDescription->setText(m_description);

    SetAllNames(allNames);
    SetMode(mode);
}

TransactionEditDialog::~TransactionEditDialog()
{
    delete ui;
}

void TransactionEditDialog::SetAllNames(const QStringList& allNames)
{
    for (const QVariant& name : allNames)
    {
        QString nameString = name.toString();
        ui->comboBoxPayer->addItem(nameString);
        ui->listWidget->addItem(nameString);
    }
    if (!m_payer.isEmpty())
    {
        ui->comboBoxPayer->setCurrentText(m_payer);
    }

    int j = 0;
    for(int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        if (m_coveringNames.empty() || (j < m_coveringNames.size() && m_coveringNames[j] == item->text()))
        {
            ++j;
            item->setCheckState(Qt::Checked);
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void TransactionEditDialog::SetMode(TransactionEditDialog::Mode mode)
{
    if (mode == Mode::Add) {
        ui->pushButtonDelete->setDisabled(true);
    }
}

void TransactionEditDialog::on_pushButtonSave_clicked()
{
    m_payer = ui->comboBoxPayer->currentText();
    m_cost = ui->doubleSpinBoxCost->value();
    m_description = ui->lineEditDescription->text();

    m_coveringNames.clear();
    for(int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item->checkState() == Qt::Checked)
        {
            m_coveringNames.push_back(ui->listWidget->item(i)->text());
        }
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
