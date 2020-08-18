#ifndef TRANSACTIONSWIDGET_H
#define TRANSACTIONSWIDGET_H

#include <QWidget>

namespace Ui {
class TransactionsWidget;
}

class TransactionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionsWidget(QWidget *parent = nullptr);
    ~TransactionsWidget();

private:
    Ui::TransactionsWidget *ui;
};

#endif // TRANSACTIONSWIDGET_H
