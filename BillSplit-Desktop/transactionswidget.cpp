#include "transactionswidget.h"
#include "ui_transactionswidget.h"

TransactionsWidget::TransactionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransactionsWidget)
{
    ui->setupUi(this);
}

TransactionsWidget::~TransactionsWidget()
{
    delete ui;
}
