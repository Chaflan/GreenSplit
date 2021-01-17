#include "peoplewidget.h"
#include "ui_peoplewidget.h"
#include "nameeditdialog.h"
#include "peopletablemodel.h"

#include <QMessageBox>
#include <QItemSelectionModel>

PeopleWidget::PeopleWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PeopleWidget),
    m_dialog(new NameEditDialog(this))
{
    m_ui->setupUi(this);
    m_dialog->setModal(true);
}

PeopleWidget::~PeopleWidget()
{
    delete m_ui;
}

void PeopleWidget::SetPeopleModel(PeopleTableModel* peopleModel)
{
    m_model = peopleModel;
    m_ui->tableView->setModel(m_model);

    m_ui->pushButtonView->setDisabled(true);
    QObject::connect(m_ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this]() { m_ui->pushButtonView->setDisabled(!m_ui->tableView->selectionModel()->hasSelection()); });
    QObject::connect(m_model, &PeopleTableModel::modelReset,
        [this]() { m_ui->pushButtonView->setDisabled(!m_ui->tableView->selectionModel()->hasSelection()); });

    // Table formatting
    m_ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_ui->tableView->horizontalHeader()->setStretchLastSection(true);
    m_ui->tableView->setStyleSheet("QHeaderView::section { background-color:gray }");
}

void PeopleWidget::ViewSelected(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    m_dialog->SetMode(NameEditDialog::Mode::Edit);
    m_dialog->SetInitials(m_model->data(index.row(), PeopleTableModel::Column::Identifier).toString());
    m_dialog->SetName(m_model->data(index.row(), PeopleTableModel::Column::Name).toString());

    for (bool isValid = false; !isValid;) {
        m_dialog->exec();

        if (m_dialog->result() == NameEditDialog::CustomDialogCode::Save) {
            if (m_model->setData(index.row(), PeopleTableModel::Column::Identifier, m_dialog->GetInitials()) &&
                m_model->setData(index.row(), PeopleTableModel::Column::Name, m_dialog->GetName()))
            {
                isValid = true;
            }
        }
        else if (m_dialog->result() == NameEditDialog::CustomDialogCode::Delete) {
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
        m_dialog->SetMode(NameEditDialog::Mode::Add);
        m_dialog->Clear();
        m_dialog->exec();

        if (m_dialog->result() != NameEditDialog::CustomDialogCode::Save ||
            m_model->addPerson(m_dialog->GetInitials(), m_dialog->GetName()))
        {
            isValid = true;
        }
    }
}

void PeopleWidget::on_pushButtonView_clicked()
{
    ViewSelected(m_ui->tableView->selectionModel()->currentIndex());
}
