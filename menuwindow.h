#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include "clockwindow.h"
#include "calendarwindow.h"
#include "weatherwindow.h"

class MenuWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

signals:
    void closed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onInteractionClicked();
    void onFunctionClicked();
    void showClockWindow();
    void showCalendarWindow();
    void showWeatherWindow();
    void returnToMainMenu();

private:
    void initUI();
    void setupMainMenuButtons(QGridLayout *layout);
    QPushButton* createMenuButton(const QString &iconPath, int width, int height);
    QPushButton* createSubMenuButton(const QString &iconPath, int width, int height);
    void createBottomButtons();
    void toggleFullScreen();
    void switchToSubMenu(const QString &bgImagePath);

    QLabel *bgLabel = nullptr;
    QWidget *mainMenuContainer = nullptr;
    QWidget *buttonContainer = nullptr;
    QPushButton *backButton = nullptr;
    QPushButton *exitButton = nullptr;
    bool isFullScreen = false;

    ClockWindow *clockWindow = nullptr;
    CalendarWindow *calendarWindow = nullptr;
    WeatherWindow *weatherWindow = nullptr;

    QWidget *currentSubMenu = nullptr; // 当前子菜单
};

#endif // MENUWINDOW_H
