#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class PeopleModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent* event) override;

    void ReadFromJsonFile();
    void WriteJson() const;

private:
    Ui::MainWindow *ui;
    PeopleModel* mPeopleModel;
};
#endif // MAINWINDOW_H
