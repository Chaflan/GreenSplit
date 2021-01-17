#ifndef PEOPLEWIDGET_H
#define PEOPLEWIDGET_H

#include <QWidget>
#include "nameeditdialog.h"


// TODO: SetPeopleModel -> SetModel for all
// TODO: Do a const check

namespace Ui {
class PeopleWidget;
}

class QItemSelectionModel;
class PeopleTableModel;

class PeopleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PeopleWidget(QWidget* parent = nullptr);
    ~PeopleWidget();

    void SetPeopleModel(PeopleTableModel* peopleModel);
    void ViewSelected(const QModelIndex& index);

private slots:
    void on_pushButtonNew_clicked();
    void on_pushButtonView_clicked();

private:

    Ui::PeopleWidget* ui;
    PeopleTableModel* m_model;
};

#endif // PEOPLEWIDGET_H
