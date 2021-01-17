#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QUrl>

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
    MainWindow(QWidget* parent = nullptr);
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

    Ui::MainWindow* m_ui = nullptr;
    DataCoreObject* m_data = nullptr;
    PeopleTableModel* m_peopleModel = nullptr;
    TransactionsTableModel* m_transactionsModel = nullptr;
    TransactionModel* m_transactionModel = nullptr;
    ResultsModel* m_resultsModel = nullptr;
};
#endif // MAINWINDOW_H
