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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_errorMessageBox(QMessageBox::Icon::Critical, "Error", ""),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
