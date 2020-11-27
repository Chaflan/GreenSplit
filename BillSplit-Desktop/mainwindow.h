#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

// -Hide implementation
// -Some kind of inheritance for the transaction and people widgets.  They
// are very similar

// TODO: standardize on m_

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DataCoreObject;
class PeopleTableModel;
class TransactionsTableModel;
class ResultsModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void ShowErrorMessage(const QString& message) const;

private:
    mutable QMessageBox m_errorMessageBox;

    Ui::MainWindow *ui;
    DataCoreObject* m_data;
    PeopleTableModel* mPeopleModel;
    TransactionsTableModel* mTransactionsModel;
    ResultsModel* mResultsModel;
};
#endif // MAINWINDOW_H
