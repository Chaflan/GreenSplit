#include "peoplewidget.h"
#include "ui_peoplewidget.h"
#include "nameeditdialog.h"

#include <QMessageBox>
#include <QItemSelectionModel>

PeopleWidget::PeopleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PeopleWidget),
    model(nullptr)
{
    ui->setupUi(this);
}

PeopleWidget::~PeopleWidget()
{
    delete ui;
}

void PeopleWidget::SetPeopleModel(PeopleModel* peopleModel)
{
    model = peopleModel;
    ui->listView->setModel(model);

    ui->pushButtonView->setDisabled(true);
    connect(ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]()
        {
            ui->pushButtonView->setDisabled(!ui->listView->selectionModel()->hasSelection());
        });
}

void PeopleWidget::on_pushButtonNew_clicked()
{
    QString initials, name;

    for (bool isValid = false; !isValid;)
    {
        NameEditDialog dialog(initials, name, NameEditDialog::Mode::Add);
        dialog.setModal(true);
        dialog.exec();

        if (dialog.result() == NameEditDialog::CustomDialogCode::Save)
        {
            if (model->addPerson(initials, name))
            {
                isValid = true;
            }
            else
            {
                QMessageBox message(QMessageBox::Icon::Critical, "Error",
                    "The chosen initials are already being used.  Please choose unique initials.");
                message.exec();
            }
        }
        else // Cancel or Delete, do nothing
        {
            isValid = true;
        }
    }
}

void PeopleWidget::on_pushButtonView_clicked()
{
    QModelIndex currSelectionIndex = ui->listView->selectionModel()->currentIndex();

    QString oldInitials = model->data(currSelectionIndex, PeopleModel::InitialsRole).toString();
    QString oldName = model->data(currSelectionIndex, PeopleModel::NameRole).toString();

    QString newInitials(oldInitials);
    QString newName(oldName);

    for (bool isValid = false; !isValid;)
    {
        NameEditDialog dialog(newInitials, newName, NameEditDialog::Mode::Edit);
        dialog.setModal(true);
        dialog.exec();

        if (dialog.result() == NameEditDialog::CustomDialogCode::Save)
        {
            // Validate first, then insert.  This is slower but better for the user.
            isValid = true;
            if (oldInitials != newInitials && !model->validateData(currSelectionIndex, newInitials, PeopleModel::Roles::InitialsRole))
            {
                QMessageBox message(QMessageBox::Icon::Critical, "Error",
                    "The chosen initials are invalid.  Please choose non-empty and unique initials.");
                message.exec();
                isValid = false;
            }
            if (oldName != newName && !model->validateData(currSelectionIndex, newName, PeopleModel::Roles::NameRole))
            {
                QMessageBox message(QMessageBox::Icon::Critical, "Error",
                    "The chosen name is invalid.  Please choose a non-empty name.");
                message.exec();
                isValid = false;
            }

            if (isValid)
            {
                if (oldInitials != newInitials)
                {
                    model->setData(currSelectionIndex, newInitials, PeopleModel::Roles::InitialsRole);
                }
                if (oldName != newName)
                {
                    model->setData(currSelectionIndex, newName, PeopleModel::Roles::NameRole);
                }
            }

        }
        else if (dialog.result() == NameEditDialog::CustomDialogCode::Delete)
        {
            model->removeRow(currSelectionIndex.row());
            isValid = true;
        }
        else // Cancel, do nothing
        {
            isValid = true;
        }
    }
}
