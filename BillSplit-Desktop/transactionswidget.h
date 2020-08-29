#ifndef TRANSACTIONSWIDGET_H
#define TRANSACTIONSWIDGET_H

#include <QWidget>

namespace Ui {
class TransactionsWidget;
}

class TransactionsModel;

class TransactionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionsWidget(QWidget *parent = nullptr);
    ~TransactionsWidget();

    void SetTransactionsModel(TransactionsModel* transactionsModel);
    void ViewSelected(const QModelIndex& index);

private slots:
    void on_pushButtonNew_clicked();
    void on_pushButtonView_clicked();
    void on_listView_doubleClicked(const QModelIndex &index);

private:
    Ui::TransactionsWidget *ui;
    TransactionsModel* model;
};

#endif // TRANSACTIONSWIDGET_H
