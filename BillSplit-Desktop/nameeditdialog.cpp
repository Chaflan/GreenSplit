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

void NameEditDialog::SetName(QString name)
{
    m_name = std::move(name);
    ui->lineEditName->setText(m_name);
}

void NameEditDialog::SetInitials(QString initials)
{
    m_initials = std::move(initials);
    ui->lineEditInitials->setText(m_initials);
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
    QString initialsEntry = ui->lineEditInitials->text();
    if (initialsEntry.isEmpty()) {
        QMessageBox message(QMessageBox::Icon::Critical, "Error", "Initials cannot be blank");
        message.exec();
        return;
    }

    QString nameEntry = ui->lineEditName->text();
    if (nameEntry.isEmpty()) {
        QMessageBox message(QMessageBox::Icon::Critical, "Error", "Name cannot be blank");
        message.exec();
        return;
    }

    m_initials = std::move(initialsEntry);
    m_name = std::move(nameEntry);
    this->done(CustomDialogCode::Save);
}
