#ifndef PEOPLEWIDGET_H
#define PEOPLEWIDGET_H

#include <QWidget>

namespace Ui {
class PeopleWidget;
}

class QItemSelectionModel;
class PeopleModel;

class PeopleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PeopleWidget(QWidget *parent = nullptr);
    ~PeopleWidget();

    void SetPeopleModel(PeopleModel* peopleModel);
    void ViewSelected(const QModelIndex& index);

private slots:
    void on_pushButtonNew_clicked();
    void on_pushButtonView_clicked();

private:
    Ui::PeopleWidget *ui;
    PeopleModel* model;
};

#endif // PEOPLEWIDGET_H
