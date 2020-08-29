#ifndef NAMEEDITDIALOG_H
#define NAMEEDITDIALOG_H

#include <QDialog>

namespace Ui {
class NameEditDialog;
}

class QString;

// Do this one like you did the other dialog.  No more reference member variables
class NameEditDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode { Add, Edit };
    enum CustomDialogCode { Cancel, Delete, Save };

public:
    explicit NameEditDialog(QString& initials, QString& name, Mode mode, QWidget *parent = nullptr);
    ~NameEditDialog();

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonSave_clicked();

private:
    Ui::NameEditDialog *ui;
    QString& m_initials;
    QString& m_name;
};

#endif // NAMEEDITDIALOG_H
