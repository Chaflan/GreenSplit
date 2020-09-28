#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "peoplemodel.h"
#include "transactionsmodel.h"
#include "resultsmodel.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mPeopleModel = new PeopleModel(m_data, this);
    mTransactionsModel = new TransactionsModel(m_data, this);
    mResultsModel = new ResultsModel(m_data, this);

    ReadFromJsonFile();
    mResultsModel->updateCalculations();

    ui->peopleWidget->SetPeopleModel(mPeopleModel);
    ui->transactionsWidget->SetTransactionsModel(mTransactionsModel);
    ui->resultsWidget->SetResultsModel(mResultsModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
//    QMessageBox msgBox;
//    msgBox.setText("The document has been modified.");
//    msgBox.setInformativeText("Do you want to save your changes?");
//    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
//    msgBox.setDefaultButton(QMessageBox::Save);
//    int ret = msgBox.exec();

//    switch (ret)
//    {
//    case QMessageBox::Save: WriteJson();
//    }

    WriteJson();
    QMainWindow::closeEvent(event);
}


// TODO: This needs to be at the model level
void MainWindow::ReadFromJsonFile()
{
    QFile file("save.json");
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument jsonDoc(QJsonDocument::fromJson(file.readAll()));
        QJsonObject jsonObj = jsonDoc.object();
        mPeopleModel->jsonRead(jsonObj);
        mTransactionsModel->jsonRead(jsonObj);
    }
}

void MainWindow::WriteJson() const
{
    QFile file("save.json");
    if (file.open(QIODevice::WriteOnly))
    {
        QJsonObject json;
        mPeopleModel->jsonWrite(json);
        mTransactionsModel->jsonWrite(json);
        QJsonDocument saveDoc(json);
        file.write(saveDoc.toJson());
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 2)
    {
        mResultsModel->updateCalculations();
    }
}
