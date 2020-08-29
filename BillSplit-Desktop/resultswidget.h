#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include <QWidget>

namespace Ui {
class ResultsWidget;
}

class ResultsModel;

class ResultsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsWidget(QWidget *parent = nullptr);
    ~ResultsWidget();

    void SetResultsModel(ResultsModel* m);

private:
    Ui::ResultsWidget *ui;
    ResultsModel* model;
};

#endif // RESULTSWIDGET_H
