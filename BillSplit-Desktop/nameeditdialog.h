#ifndef NAMEEDITDIALOG_H
#define NAMEEDITDIALOG_H

#include <QDialog>

namespace Ui {
class NameEditDialog;
}

class NameEditDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode { Add, Edit };
    enum CustomDialogCode { Cancel, Delete, Save };

public:
    explicit NameEditDialog(Mode mode, QWidget *parent = nullptr);
    ~NameEditDialog();

    void SetName(QString name);
    void SetInitials(QString initials);
    QString GetName() const { return m_name; }
    QString GetInitials() const { return m_initials; }

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonSave_clicked();

private:
    Ui::NameEditDialog *ui;

    // TODO: Try not even having these, just pull from the UI
    QString m_initials;
    QString m_name;
};

#endif // NAMEEDITDIALOG_H
