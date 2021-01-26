#ifndef PEOPLEWIDGET_H
#define PEOPLEWIDGET_H

#include <QWidget>

namespace Ui {
class PeopleWidget;
}

class QItemSelectionModel;
class PeopleTableModel;
class NameEditDialog;

class PeopleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PeopleWidget(QWidget* parent = nullptr);
    ~PeopleWidget();

    void SetPeopleModel(PeopleTableModel* peopleModel);
    void ViewSelected(const QModelIndex& index) const;

private slots:
    void on_pushButtonNew_clicked() const;
    void on_pushButtonView_clicked() const;

private:

    Ui::PeopleWidget* m_ui = nullptr;
    NameEditDialog* m_dialog = nullptr;
    PeopleTableModel* m_model = nullptr;
};

#endif // PEOPLEWIDGET_H
