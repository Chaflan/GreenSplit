#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datacoreold.h"

// -Hide implementation
// -Some kind of inheritance for the transaction and people widgets.  They
// are very similar

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class PeopleModel;
class TransactionsTableModel;
class ResultsModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent* event) override;

    void ReadFromJsonFile();
    void WriteJson() const;

private slots:
    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    DataCoreOld m_data;
    PeopleModel* mPeopleModel;
    TransactionsTableModel* mTransactionsModel;
    ResultsModel* mResultsModel;
};
#endif // MAINWINDOW_H
