#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "datacoreobject.h"
#include "peopletablemodel.h"
#include "transactionstablemodel.h"
#include "resultsmodel.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_errorMessageBox(QMessageBox::Icon::Critical, "Error", ""),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_data = new DataCoreObject(this);
    connect(m_data, &DataCoreObject::signalError, this, &MainWindow::ShowErrorMessage);
    m_data->jsonRead();

    mPeopleModel = new PeopleTableModel(this);
    connect(mPeopleModel, &PeopleTableModel::signalError, this, &MainWindow::ShowErrorMessage);
    mPeopleModel->setDataCore(m_data);

    mTransactionsModel = new TransactionsTableModel(this);
    connect(mPeopleModel, &PeopleTableModel::signalError, this, &MainWindow::ShowErrorMessage);
    mTransactionsModel->setDataCore(m_data);

    mResultsModel = new ResultsModel(this);
    mResultsModel->setDataCore(m_data);

    ui->peopleWidget->SetPeopleModel(mPeopleModel);
    ui->transactionsWidget->SetTransactionsModel(mTransactionsModel);
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
