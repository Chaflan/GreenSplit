#ifndef TRANSACTIONEDITDIALOG_H
#define TRANSACTIONEDITDIALOG_H

#include <QDialog>
#include <vector>

namespace Ui {
class transactioneditdialog;
}

class TransactionEditDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode { Add, Edit };
    enum CustomDialogCode { Cancel, Delete, Save };

public:
    explicit TransactionEditDialog(
            const QStringList& allNames,
            Mode mode,
            QWidget *parent = nullptr);
    explicit TransactionEditDialog(
            QString payer,
            double cost,
            QString description,
            QStringList coveringNames,
            const QStringList& allNames,
            Mode mode,
            QWidget *parent = nullptr);
    ~TransactionEditDialog();

private:
    void SetAllNames(const QStringList& allNames);
    void SetMode(Mode mode);

private slots:
    void on_pushButtonSave_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonCancel_clicked();

private:
    Ui::transactioneditdialog *ui;

public:
    // TODO: Gets only
    QString m_payer;
    double m_cost;
    QString m_description;
    QStringList m_coveringNames;
};

#endif // TRANSACTIONEDITDIALOG_H
