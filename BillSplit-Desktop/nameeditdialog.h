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
    explicit NameEditDialog(Mode mode, QWidget* parent = nullptr);
    ~NameEditDialog();

    void SetName(const QString& name);
    void SetInitials(const QString& initials);
    QString GetName() const;
    QString GetInitials() const;

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonSave_clicked();

private:
    Ui::NameEditDialog *ui;
};

#endif // NAMEEDITDIALOG_H
