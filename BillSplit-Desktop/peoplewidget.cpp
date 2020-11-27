#include "peoplewidget.h"
#include "ui_peoplewidget.h"
#include "nameeditdialog.h"
#include "peopletablemodel.h"

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
void PeopleWidget::SetPeopleModel(PeopleTableModel* peopleModel)
{
    model = peopleModel;
    ui->tableView->setModel(model);

    ui->pushButtonView->setDisabled(true);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]() {
            ui->pushButtonView->setDisabled(!ui->tableView->selectionModel()->hasSelection());
        });

    // Table formatting
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setStyleSheet("QHeaderView::section { background-color:gray }");
}

void PeopleWidget::ViewSelected(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    // TODO: Cache this in PeopleWidget class?  Will have to make a set mode
    NameEditDialog dialog(NameEditDialog::Mode::Edit);
    dialog.setModal(true);
    dialog.SetInitials(model->data(index, PeopleTableModel::Column::Identifier).toString());
    dialog.SetName(model->data(index, PeopleTableModel::Column::FullName).toString());

    for (bool isValid = false; !isValid;) {
        dialog.exec();

        if (dialog.result() == NameEditDialog::CustomDialogCode::Save) {
            if (model->setData(index.row(), PeopleTableModel::Column::Identifier, dialog.GetInitials()) &&
                model->setData(index.row(), PeopleTableModel::Column::FullName, dialog.GetName()))
            {
                isValid = true;
            }
        }
        else if (dialog.result() == NameEditDialog::CustomDialogCode::Delete) {
            model->removeRow(index.row());
            isValid = true;
        }
        else {
            // Cancel, do nothing
            isValid = true;
        }
    }
}

void PeopleWidget::on_pushButtonNew_clicked()
{
    for (bool isValid = false; !isValid;) {
        NameEditDialog dialog(NameEditDialog::Mode::Add);
        dialog.setModal(true);
        dialog.exec();

        if (dialog.result() != NameEditDialog::CustomDialogCode::Save ||
            model->addPerson(dialog.GetInitials(), dialog.GetName()))
        {
            isValid = true;
        }
    }
}

void PeopleWidget::on_pushButtonView_clicked()
{
    ViewSelected(ui->tableView->selectionModel()->currentIndex());
}
