#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QStackedWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showPage1();
    void showPage2();

private:
    QStackedWidget *stackedWidget;
    QWidget *page1;
    QWidget *page2;
    void createMenu();
};
#endif // MAINWINDOW_H
