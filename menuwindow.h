#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include "clockwindow.h"
class MenuWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

signals:
    void closed();  // 窗口关闭信号

protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override; //ESC退出全屏模式

private slots:
    // 主菜单按钮槽函数
    void onClockClicked();//时钟按钮的点击事件
    void onInteractionClicked();
    void onCalendarClicked();
    void onMaterialClicked();
    void onWeatherClicked();
    void onTrainingClicked();

private:
    void initUI();
    void setupMainMenuButtons(QGridLayout *layout);
    QPushButton* createMenuButton(const QString &text, const QString &iconPath, int width, int height);
    void createBottomButtons();

    ClockWindow *clockWindow = nullptr; // 时钟功能

    // UI 成员
    QLabel *bgLabel = nullptr;          // 背景图片
    QWidget *mainMenuContainer = nullptr; // 主菜单容器
    QWidget *buttonContainer = nullptr;   // 底部按钮容器

    // 底部按钮
    QPushButton *backButton = nullptr;
    QPushButton *exitButton = nullptr;
};

#endif // MENUWINDOW_H
