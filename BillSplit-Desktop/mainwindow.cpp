#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "peoplemodel.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mPeopleModel = new PeopleModel(this);
    ReadFromJsonFile();

    ui->peopleWidget->SetPeopleModel(mPeopleModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox msgBox;
    msgBox.setText("The document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    switch (ret)
    {
    case QMessageBox::Save: WriteJson();
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::ReadFromJsonFile()
{
    QFile file("save.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QJsonDocument jsonDoc(QJsonDocument::fromJson(file.readAll()));
    mPeopleModel->jsonRead(jsonDoc.object());
}

void MainWindow::WriteJson() const
{
    QFile file("save.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonObject json;
    mPeopleModel->jsonWrite(json);
    QJsonDocument saveDoc(json);
    file.write(saveDoc.toJson());
}

