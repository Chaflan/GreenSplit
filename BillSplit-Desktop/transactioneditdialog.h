#ifndef TRANSACTIONEDITDIALOG_H
#define TRANSACTIONEDITDIALOG_H

#include <QDialog>

namespace Ui {
class transactioneditdialog;
}

class TransactionModel;

class TransactionEditDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode { Add, Edit };
    enum CustomDialogCode { Cancel, Delete, Save };

public:
    explicit TransactionEditDialog(QWidget* parent = nullptr);
    ~TransactionEditDialog();

    void SetModel(TransactionModel* model);
    TransactionModel* GetModel() const { return m_model; }

    void SetMode(Mode mode);

public slots:
    int exec() override;

private slots:
    void on_pushButtonSave_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonCancel_clicked();

private:
    Ui::transactioneditdialog* m_ui = nullptr;
    TransactionModel* m_model = nullptr;
};

#endif // TRANSACTIONEDITDIALOG_H
