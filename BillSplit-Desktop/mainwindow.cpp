#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "datacoreobject.h"
#include "peopletablemodel.h"
#include "transactionstablemodel.h"
#include "transactionmodel.h"
#include "resultsmodel.h"
//#include <QFile>
//#include <QJsonDocument>
//#include <QJsonObject>
//#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_errorMessageBox(QMessageBox::Icon::Critical, "Error", ""),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::FileOpen);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::FileSave);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::FileSaveAs);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::FileNew);

    m_data = new DataCoreObject(this);
    connect(m_data, &DataCoreObject::signalError, this, &MainWindow::ShowErrorMessage);
    m_data->jsonRead();

    m_peopleModel = new PeopleTableModel(this);
    connect(m_peopleModel, &PeopleTableModel::signalError, this, &MainWindow::ShowErrorMessage);
    m_peopleModel->setDataCore(m_data);

    m_transactionsModel = new TransactionsTableModel(this);
    connect(m_peopleModel, &PeopleTableModel::signalError, this, &MainWindow::ShowErrorMessage);
    m_transactionsModel->setDataCore(m_data);

    m_transactionModel = new TransactionModel(this);
    m_transactionModel->setDataCore(m_data);

    mResultsModel = new ResultsModel(this);
    mResultsModel->setDataCore(m_data);

    ui->peopleWidget->SetPeopleModel(m_peopleModel);
    ui->transactionsWidget->SetTransactionsModel(m_transactionsModel);
    ui->transactionsWidget->SetTransactionModel(m_transactionModel);
    ui->resultsWidget->SetResultsModel(mResultsModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ShowErrorMessage(const QString& message) const
{
    m_errorMessageBox.setText(message);
    m_errorMessageBox.exec();
}

void MainWindow::FileNew()
{
    m_data->clear();
    ui->actionSave->setEnabled(false);
    ui->actionSave->setText("Save");
}

void MainWindow::FileOpen()
{
    QUrl fileToOpen = QFileDialog::getOpenFileUrl(this, "Select File To Open", QUrl(), "*.json");
    if (!fileToOpen.isEmpty() && m_data->jsonRead(fileToOpen)) {
        ui->actionSave->setEnabled(true);
        ui->actionSave->setText("Save (" + fileToOpen.fileName() + ")");
        m_saveFile = std::move(fileToOpen);
    }
}

void MainWindow::FileSave()
{
    m_data->jsonWrite(m_saveFile);
}

// TODO: Ensure QML does the Save file retargeting too
void MainWindow::FileSaveAs()
{
    QUrl fileToSave = QFileDialog::getSaveFileUrl(this, "Select File To Save", QUrl(), "*.json");
    if (!fileToSave.isEmpty() && m_data->jsonWrite(fileToSave)) {
        ui->actionSave->setEnabled(true);
        ui->actionSave->setText("Save (" + fileToSave.fileName() + ")");
        m_saveFile = std::move(fileToSave);
    }
}
