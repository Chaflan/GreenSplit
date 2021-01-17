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
    explicit ResultsWidget(QWidget* parent = nullptr);
    ~ResultsWidget();

    void SetResultsModel(ResultsModel* m);

private:
    Ui::ResultsWidget* m_ui = nullptr;
    ResultsModel* m_model= nullptr;
};

#endif // RESULTSWIDGET_H
