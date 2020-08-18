#include "nameeditdialog.h"
#include "ui_nameeditdialog.h"

#include <QMessageBox>

NameEditDialog::NameEditDialog(QString& initials, QString& name, Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameEditDialog),
    m_initials(initials),
    m_name(name)
{
    ui->setupUi(this);
    ui->lineEditInitials->setText(initials);
    ui->lineEditName->setText(name);

    if (mode == Mode::Add)
    {
        ui->pushButtonDelete->setDisabled(true);
    }
}

NameEditDialog::~NameEditDialog()
{
    delete ui;
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
