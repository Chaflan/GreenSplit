#include "peoplewidget.h"
#include "ui_peoplewidget.h"
#include "nameeditdialog.h"
#include "peopletablemodel.h"

#include <QMessageBox>
#include <QItemSelectionModel>

PeopleWidget::PeopleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PeopleWidget),
    m_model(nullptr)
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
    m_model = peopleModel;
    ui->tableView->setModel(m_model);

    ui->pushButtonView->setDisabled(true);
    QObject::connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]() { ui->pushButtonView->setDisabled(!ui->tableView->selectionModel()->hasSelection()); });
    QObject::connect(m_model, &PeopleTableModel::modelReset,
        [this]() { ui->pushButtonView->setDisabled(!ui->tableView->selectionModel()->hasSelection()); });

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
    dialog.SetInitials(m_model->data(index.row(), PeopleTableModel::Column::Identifier).toString());
    dialog.SetName(m_model->data(index.row(), PeopleTableModel::Column::Name).toString());

    for (bool isValid = false; !isValid;) {
        dialog.exec();

        if (dialog.result() == NameEditDialog::CustomDialogCode::Save) {
            if (m_model->setData(index.row(), PeopleTableModel::Column::Identifier, dialog.GetInitials()) &&
                m_model->setData(index.row(), PeopleTableModel::Column::Name, dialog.GetName()))
            {
                isValid = true;
            }
        }
        else if (dialog.result() == NameEditDialog::CustomDialogCode::Delete) {
            m_model->removeRow(index.row());
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
            m_model->addPerson(dialog.GetInitials(), dialog.GetName()))
        {
            isValid = true;
        }
    }
}

void PeopleWidget::on_pushButtonView_clicked()
{
    ViewSelected(ui->tableView->selectionModel()->currentIndex());
}
