#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QUrl>

// -Hide implementation
// -Some kind of inheritance for the transaction and people widgets.  They
// are very similar

// TODO: standardize on m_
// TODO: standardize on singleTransactionModel

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DataCoreObject;
class PeopleTableModel;
class TransactionsTableModel;
class TransactionModel;
class ResultsModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void ShowErrorMessage(const QString& message) const;

private slots:
    void FileNew();
    void FileOpen();
    void FileSave();
    void FileSaveAs();

private:
    mutable QMessageBox m_errorMessageBox;
    mutable QUrl m_saveFile;

    Ui::MainWindow* ui;
    DataCoreObject* m_data;
    PeopleTableModel* m_peopleModel;
    TransactionsTableModel* m_transactionsModel;
    TransactionModel* m_transactionModel;
    ResultsModel* mResultsModel;
};
#endif // MAINWINDOW_H
