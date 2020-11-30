#include "nameeditdialog.h"
#include "ui_nameeditdialog.h"

#include <QMessageBox>

NameEditDialog::NameEditDialog(Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameEditDialog)
{
    ui->setupUi(this);
    if (mode == Mode::Add) {
        ui->pushButtonDelete->setDisabled(true);
    }
}

NameEditDialog::~NameEditDialog()
{
    delete ui;
}

void NameEditDialog::SetName(const QString& name)
{
    ui->lineEditName->setText(name);
}

void NameEditDialog::SetInitials(const QString& initials)
{
    ui->lineEditInitials->setText(initials);
}

QString NameEditDialog::GetName() const
{
    return ui->lineEditName->text();
}

QString NameEditDialog::GetInitials() const
{
    return ui->lineEditInitials->text();
}

void NameEditDialog::on_pushButtonCancel_clicked()
{
    this->done(CustomDialogCode::Cancel);
}

void NameEditDialog::on_pushButtonDelete_clicked()
{
    this->done(CustomDialogCode::Delete);
}

void NameEditDialog::on_pushButtonSave_clicked()
{
    this->done(CustomDialogCode::Save);
}
