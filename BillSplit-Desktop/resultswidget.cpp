#include "resultswidget.h"
#include "ui_resultswidget.h"
#include "resultsmodel.h"

ResultsWidget::ResultsWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ResultsWidget),
    m_model(nullptr)
{
    m_ui->setupUi(this);
}

ResultsWidget::~ResultsWidget()
{
    delete m_ui;
}

void ResultsWidget::SetResultsModel(ResultsModel* m)
{
    m_model = m;
    m_ui->listView->setModel(m_model);
}
