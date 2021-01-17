#ifndef TRANSACTIONSWIDGET_H
#define TRANSACTIONSWIDGET_H

#include <QWidget>

namespace Ui {
class TransactionsWidget;
}

class TransactionsTableModel;
class TransactionModel;
class TransactionEditDialog;

class TransactionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionsWidget(QWidget* parent = nullptr);
    ~TransactionsWidget();

    void SetTransactionsModel(TransactionsTableModel* transactionsTableModel);
    void SetTransactionModel(TransactionModel* transactionModel);
    void ViewSelected(const QModelIndex& index);

private slots:
    void on_pushButtonNew_clicked();
    void on_pushButtonView_clicked();

private:
    Ui::TransactionsWidget* m_ui = nullptr;
    TransactionEditDialog* m_dialog = nullptr;
    TransactionsTableModel* m_model = nullptr;
};

#endif // TRANSACTIONSWIDGET_H
