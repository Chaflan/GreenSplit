#ifndef TRANSACTIONEDITDIALOG_H
#define TRANSACTIONEDITDIALOG_H

#include <QDialog>

// TODO: More research on how widget models work.  They are frustratingly different from QML models
// and so there isnt much carryover.  For instance a widget combo box uses a QStringListModel while
// the QML version uses QStringList.  Similarly QML is ok with a QList<QVariant> as a model but
// Widgets once again must have something that inherits from QAbstractItemModel.
// for now I am using the oldschool boilerplate connect code with clunky transfers.



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
    explicit TransactionEditDialog(QWidget *parent = nullptr);
    ~TransactionEditDialog();

    void SetMode(Mode mode);

    void SetModel(TransactionModel* model);
    TransactionModel* GetModel() const { return m_model; }

public slots:
    int exec() override;

private slots:
    void on_pushButtonSave_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonCancel_clicked();

private:
    Ui::transactioneditdialog *ui;
    TransactionModel* m_model = nullptr;
};

#endif // TRANSACTIONEDITDIALOG_H
