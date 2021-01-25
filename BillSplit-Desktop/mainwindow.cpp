#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "datacoreobject.h"
#include "peopletablemodel.h"
#include "transactionstablemodel.h"
#include "transactionmodel.h"
#include "resultsmodel.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_errorMessageBox(QMessageBox::Icon::Critical, "Error", ""),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    QObject::connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::FileOpen);
    QObject::connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::FileSave);
    QObject::connect(m_ui->actionSave_As, &QAction::triggered, this, &MainWindow::FileSaveAs);
    QObject::connect(m_ui->actionNew, &QAction::triggered, this, &MainWindow::FileNew);

    m_data = new DataCoreObject(this);
    QObject::connect(m_data, &DataCoreObject::signalError, this, &MainWindow::ShowErrorMessage);

    m_peopleModel = new PeopleTableModel(this);
    QObject::connect(m_peopleModel, &PeopleTableModel::signalError, this, &MainWindow::ShowErrorMessage);
    m_peopleModel->setDataCore(m_data);

    m_transactionsModel = new TransactionsTableModel(this);
    QObject::connect(m_peopleModel, &PeopleTableModel::signalError, this, &MainWindow::ShowErrorMessage);
    m_transactionsModel->setDataCore(m_data);

    m_transactionModel = new TransactionModel(this);
    m_transactionModel->setDataCore(m_data);

    m_resultsModel = new ResultsModel(this);
    m_resultsModel->setDataCore(m_data);

    m_ui->peopleWidget->SetPeopleModel(m_peopleModel);
    m_ui->transactionsWidget->SetTransactionsModel(m_transactionsModel);
    m_ui->transactionsWidget->SetTransactionModel(m_transactionModel);
    m_ui->resultsWidget->SetResultsModel(m_resultsModel);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::ShowErrorMessage(const QString& message) const
{
    m_errorMessageBox.setText(message);
    m_errorMessageBox.exec();
}

void MainWindow::FileNew()
{
    m_data->clear();
    m_ui->actionSave->setEnabled(false);
    m_ui->actionSave->setText("Save");
}

void MainWindow::FileOpen()
{
    QUrl fileToOpen = QFileDialog::getOpenFileUrl(this, "Open", QUrl(), "*.json");
    if (!fileToOpen.isEmpty() && m_data->jsonRead(fileToOpen)) {
        m_ui->actionSave->setEnabled(true);
        m_ui->actionSave->setText("Save (" + fileToOpen.fileName() + ")");
        m_saveFile = std::move(fileToOpen);
    }
}

void MainWindow::FileSave()
{
    m_data->jsonWrite(m_saveFile);
}

void MainWindow::FileSaveAs()
{
    QUrl fileToSave = QFileDialog::getSaveFileUrl(this, "Save As", QUrl(), "*.json");
    if (!fileToSave.isEmpty() && m_data->jsonWrite(fileToSave)) {
        m_ui->actionSave->setEnabled(true);
        m_ui->actionSave->setText("Save (" + fileToSave.fileName() + ")");
        m_saveFile = std::move(fileToSave);
    }
}
