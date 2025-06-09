#include "mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenuBar>  // 添加这行
#include <QMenu>     // 如果还没有包含的话

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置窗口属性
    setWindowTitle("菜单跳转示例");
    resize(400, 300);

    // 创建堆叠窗口
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // 创建页面1
    page1 = new QWidget();
    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    layout1->addWidget(new QLabel("欢迎来到页面1", page1));
    QPushButton *backButton1 = new QPushButton("返回主菜单", page1);
    layout1->addWidget(backButton1);
    page1->setLayout(layout1);
    stackedWidget->addWidget(page1);

    // 创建页面2
    page2 = new QWidget();
    QVBoxLayout *layout2 = new QVBoxLayout(page2);
    layout2->addWidget(new QLabel("这是页面2的内容", page2));
    QPushButton *backButton2 = new QPushButton("返回主菜单", page2);
    layout2->addWidget(backButton2);
    page2->setLayout(layout2);
    stackedWidget->addWidget(page2);

    // 创建菜单
    createMenu();

    // 连接返回按钮
    connect(backButton1, &QPushButton::clicked, [this](){
        stackedWidget->setCurrentIndex(0);
    });
    connect(backButton2, &QPushButton::clicked, [this](){
        stackedWidget->setCurrentIndex(0);
    });
}

void MainWindow::createMenu()
{
    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // 添加跳转菜单
    QMenu *jumpMenu = menuBar->addMenu("跳转菜单");

    // 添加菜单项
    QAction *actionPage1 = jumpMenu->addAction("跳转到页面1");
    QAction *actionPage2 = jumpMenu->addAction("跳转到页面2");

    // 连接信号槽
    connect(actionPage1, &QAction::triggered, this, &MainWindow::showPage1);
    connect(actionPage2, &QAction::triggered, this, &MainWindow::showPage2);
}

void MainWindow::showPage1()
{
    stackedWidget->setCurrentWidget(page1);
}

void MainWindow::showPage2()
{
    stackedWidget->setCurrentWidget(page2);
}

MainWindow::~MainWindow()
{
}
