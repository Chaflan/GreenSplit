#include "peoplewidget.h"
#include "ui_peoplewidget.h"
#include "nameeditdialog.h"
#include "peoplemodel.h"

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

// TODO: Maybe call this init?
void PeopleWidget::SetPeopleModel(PeopleModel* peopleModel)
{
    model = peopleModel;
    ui->tableView->setModel(model);

    ui->pushButtonView->setDisabled(true);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]()
        {
            ui->pushButtonView->setDisabled(!ui->tableView->selectionModel()->hasSelection());
        });

    // Table formatting
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setStyleSheet("QHeaderView::section { background-color:gray }");
}

void PeopleWidget::ViewSelected(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    QString oldInitials = model->data(index, PeopleModel::InitialsRole).toString();
    QString oldName = model->data(index, PeopleModel::NameRole).toString();

    QString newInitials(oldInitials);
    QString newName(oldName);

    for (bool isValid = false; !isValid;)
    {
        NameEditDialog dialog(newInitials, newName, NameEditDialog::Mode::Edit);
        dialog.setModal(true);
        dialog.exec();

        if (dialog.result() == NameEditDialog::CustomDialogCode::Save)
        {
            bool initialsSuccess = true;
            if (oldInitials != newInitials &&
                !model->setData(index, newInitials, PeopleModel::Roles::InitialsRole))
            {
                QMessageBox message(QMessageBox::Icon::Critical, "Error",
                    "The chosen initials are invalid.  Please choose non-empty and unique initials.");
                message.exec();
                initialsSuccess = false;
            }

            bool nameSuccess = true;
            if (oldName != newName &&
                !model->setData(index, newName, PeopleModel::Roles::NameRole))
            {
                QMessageBox message(QMessageBox::Icon::Critical, "Error",
                    "The chosen name is invalid.  Please choose a non-empty name.");
                message.exec();
                nameSuccess = false;
            }

            // If one failed and the other succeeded, attempt to revert the
            // one that succeeded in case the user cancels
            if (!initialsSuccess && nameSuccess)
            {
                model->setData(index, oldName, PeopleModel::Roles::NameRole);
            }
            else if (!nameSuccess && initialsSuccess)
            {
                model->setData(index, oldInitials, PeopleModel::Roles::InitialsRole);
            }

            isValid = initialsSuccess && nameSuccess;
        }
        else if (dialog.result() == NameEditDialog::CustomDialogCode::Delete)
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
    ViewSelected(ui->tableView->selectionModel()->currentIndex());
}
