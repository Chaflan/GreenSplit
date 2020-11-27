#ifndef TRANSACTIONSWIDGET_H
#define TRANSACTIONSWIDGET_H

#include <QWidget>

namespace Ui {
class TransactionsWidget;
}

class TransactionsTableModel;

class TransactionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionsWidget(QWidget *parent = nullptr);
    ~TransactionsWidget();

    void SetTransactionsModel(TransactionsTableModel* TransactionsTableModel);
    void ViewSelected(const QModelIndex& index);

private slots:
    void on_pushButtonNew_clicked();
    void on_pushButtonView_clicked();

private:
    Ui::TransactionsWidget *ui;
    TransactionsTableModel* model;
};

#endif // TRANSACTIONSWIDGET_H
