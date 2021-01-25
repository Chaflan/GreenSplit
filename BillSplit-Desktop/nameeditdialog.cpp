#include "nameeditdialog.h"
#include "ui_nameeditdialog.h"

#include <QMessageBox>

NameEditDialog::NameEditDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NameEditDialog)
{
    m_ui->setupUi(this);
    SetMode(Mode::Add);
}

NameEditDialog::~NameEditDialog()
{
    delete m_ui;
}

void NameEditDialog::SetName(const QString& name)
{
    m_ui->lineEditName->setText(name);
}

void NameEditDialog::SetInitials(const QString& initials)
{
    m_ui->lineEditInitials->setText(initials);
}

QString NameEditDialog::GetName() const
{
    return m_ui->lineEditName->text();
}

QString NameEditDialog::GetInitials() const
{
    return m_ui->lineEditInitials->text();
}

void NameEditDialog::SetMode(NameEditDialog::Mode mode)
{
    m_ui->pushButtonDelete->setDisabled(mode == Mode::Add);
    switch (mode) {
        case Mode::Add: setWindowTitle("Add Person"); break;
        case Mode::Edit: setWindowTitle("View Person"); break;
        default: setWindowTitle("Person");
    }
}

void NameEditDialog::Clear()
{
    SetName("");
    SetInitials("");
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
