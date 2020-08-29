#include "resultswidget.h"
#include "ui_resultswidget.h"

#include "resultsmodel.h"

ResultsWidget::ResultsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsWidget),
    model(nullptr)
{
    ui->setupUi(this);
}

ResultsWidget::~ResultsWidget()
{
    delete ui;
}

void ResultsWidget::SetResultsModel(ResultsModel* m)
{
    model = m;
    ui->listView->setModel(model);
}
