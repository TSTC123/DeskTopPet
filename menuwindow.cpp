#include "menuwindow.h"
#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QCloseEvent>
#include <QGraphicsDropShadowEffect>
#include <QShortcut>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPushButton>
#include <QDebug>
MenuWindow::MenuWindow(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(QGuiApplication::primaryScreen()->size());
    initUI();

    // 添加全屏切换快捷键 (F11)
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(toggleFullScreen()));
}

MenuWindow::~MenuWindow() {
    if (bgLabel) bgLabel->deleteLater();
    if (mainMenuContainer) mainMenuContainer->deleteLater();
    if (buttonContainer) buttonContainer->deleteLater();
    if (clockWindow) clockWindow->deleteLater();
    if (calendarWindow) calendarWindow->deleteLater();
    if (weatherWindow) weatherWindow->deleteLater();
    if (currentSubMenu) currentSubMenu->deleteLater();
}

void MenuWindow::initUI() {
    // 背景图
    bgLabel = new QLabel(this);
    QPixmap bgPixmap(":/cat/caiDanJM.png");
    if (!bgPixmap.isNull()) {
        bgPixmap = bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding);
        bgLabel->setPixmap(bgPixmap);
    }
    bgLabel->setGeometry(0, 0, width(), height());

    // 主菜单容器
    mainMenuContainer = new QWidget(this);
    mainMenuContainer->setAttribute(Qt::WA_TranslucentBackground);

    // 使用网格布局排列主菜单按钮
    QGridLayout *mainMenuLayout = new QGridLayout(mainMenuContainer);
    mainMenuLayout->setSpacing(60);
    mainMenuLayout->setContentsMargins(100, 150, 100, 150);

    // 添加主菜单按钮
    setupMainMenuButtons(mainMenuLayout);

    // 设置主菜单容器位置和大小
    mainMenuContainer->setGeometry(0, 0, width(), height());

    // 创建底部按钮容器
    createBottomButtons();

    // 设置窗口样式
    setStyleSheet("background: transparent;");
}

void MenuWindow::setupMainMenuButtons(QGridLayout *layout) {
    // 根据屏幕尺寸动态计算按钮大小
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    const int btnWidth = screenSize.width() / 4;
    const int btnHeight = screenSize.height() / 2;

    layout->setSpacing(50);
    layout->setContentsMargins(100, 50, 100, 50);

    // 养成互动按钮
    QPushButton *interactionBtn = createMenuButton(":/cat/interact.png", btnWidth, btnHeight);
    layout->addWidget(interactionBtn, 0, 0, Qt::AlignCenter);

    // 功能按钮
    QPushButton *functionBtn = createMenuButton(":/cat/function.png", btnWidth, btnHeight);
    layout->addWidget(functionBtn, 0, 1, Qt::AlignCenter);

    // 连接信号槽
    connect(interactionBtn, &QPushButton::clicked, this, &MenuWindow::onInteractionClicked);
    connect(functionBtn, &QPushButton::clicked, this, &MenuWindow::onFunctionClicked);
}

QPushButton* MenuWindow::createMenuButton(const QString &iconPath, int width, int height) {
    QPushButton *button = new QPushButton(this);
    button->setFixedSize(width, height);

    // 设置按钮为完全透明，无边框
    button->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   border: none;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 10%);"
        "}");

    // 创建图标标签
    QLabel *iconLabel = new QLabel(button);
    iconLabel->setFixedSize(width, height);

    // 加载图标并保持原始形状
    QPixmap icon(iconPath);
    if (!icon.isNull()) {
        // 保持原始透明度
        icon = icon.scaled(width, height,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);

        // 设置图标为不规则形状
        iconLabel->setPixmap(icon);
        iconLabel->setMask(icon.mask()); // 应用不规则遮罩
    }

    // 居中显示
    iconLabel->setAlignment(Qt::AlignCenter);

    // 使用布局确保图标居中
    QVBoxLayout *btnLayout = new QVBoxLayout(button);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->addWidget(iconLabel);

    // 添加悬停效果
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(button);
    button->setGraphicsEffect(effect);
    effect->setOpacity(0.9);

    // 悬停动画
    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(200);
    animation->setStartValue(0.9);
    animation->setEndValue(1.0);

    connect(button, &QPushButton::pressed, [animation]() {
        animation->setDirection(QAbstractAnimation::Forward);
        animation->start();
    });

    connect(button, &QPushButton::released, [animation]() {
        animation->setDirection(QAbstractAnimation::Backward);
        animation->start();
    });

    return button;
}

QPushButton* MenuWindow::createSubMenuButton(const QString &iconPath, int width, int height) {
    QPushButton *button = new QPushButton(this);
    button->setFixedSize(width, height);
    button->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   border: none;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 10%);"
        "}"
        );

    // 图标标签
    QLabel *iconLabel = new QLabel(button);
    iconLabel->setFixedSize(width, height);
    iconLabel->setAlignment(Qt::AlignCenter);

    // 加载图标
    QPixmap icon(iconPath);
    if (!icon.isNull()) {
        icon = icon.scaled(width - 20, height - 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 添加阴影效果（模拟边框）
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(iconLabel);
        shadow->setBlurRadius(15);                      // 阴影模糊半径（控制边框宽度）
        shadow->setColor(QColor(255, 255, 255, 100));  // 半透明白色阴影
        shadow->setOffset(0);                          // 无偏移
        iconLabel->setGraphicsEffect(shadow);

        iconLabel->setPixmap(icon);
    }

    // 布局
    QVBoxLayout *btnLayout = new QVBoxLayout(button);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->addWidget(iconLabel);

    return button;
}

void MenuWindow::createBottomButtons() {
    buttonContainer = new QWidget(this);
    buttonContainer->setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("返回", this);
    backButton->setFixedSize(120, 50);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 30%);"
        "   border: 2px solid rgba(0, 0, 0, 70%);"
        "   border-radius: 10px;"
        "   color: black;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 50%);"
        "}");
    connect(backButton, &QPushButton::clicked, this, &QWidget::close);

    exitButton = new QPushButton("退出", this);
    exitButton->setFixedSize(120, 50);
    exitButton->setStyleSheet(backButton->styleSheet());
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(exitButton);

    const int marginBottom = 30;
    buttonContainer->move((width() - buttonContainer->sizeHint().width()) / 2,
                          height() - 50 - marginBottom);
    buttonContainer->adjustSize();
}

void MenuWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0, 0, 0, 150));
}

void MenuWindow::closeEvent(QCloseEvent *event) {
    emit closed();
    QWidget::closeEvent(event);
}

void MenuWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        this->close();
    }
    QWidget::keyPressEvent(event);
}

void MenuWindow::toggleFullScreen() {
    if (isFullScreen) {
        showNormal();
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    } else {
        showFullScreen();
    }
    isFullScreen = !isFullScreen;
    show();
}

void MenuWindow::onInteractionClicked() {
    qDebug() << "养成互动按钮被点击";
}

void MenuWindow::onFunctionClicked() {
    // 切换到功能子菜单背景
    switchToSubMenu(":/cat/function_bg.png");

    // 隐藏主菜单和底部按钮
    mainMenuContainer->hide();
    buttonContainer->hide();

    // 创建功能子菜单容器
    if (currentSubMenu) {
        currentSubMenu->deleteLater();
    }
    currentSubMenu = new QWidget(this);
    currentSubMenu->setAttribute(Qt::WA_TranslucentBackground);
    QGridLayout *subMenuLayout = new QGridLayout(currentSubMenu);
    subMenuLayout->setSpacing(30);
    subMenuLayout->setContentsMargins(100, 50, 100, 50);

    // 动态计算按钮大小
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    const int btnWidth = screenSize.width() / 5;
    const int btnHeight = screenSize.height() / 3;

    // 创建功能按钮（带阴影）
    QPushButton *clockBtn = createSubMenuButton(":/cat/clock.png", btnWidth, btnHeight);
    QPushButton *calendarBtn = createSubMenuButton(":/cat/calendar.png", btnWidth, btnHeight);
    QPushButton *weatherBtn = createSubMenuButton(":/cat/weather.png", btnWidth, btnHeight);

    subMenuLayout->addWidget(clockBtn, 0, 0, Qt::AlignCenter);
    subMenuLayout->addWidget(calendarBtn, 0, 1, Qt::AlignCenter);
    subMenuLayout->addWidget(weatherBtn, 0, 2, Qt::AlignCenter);

    // 返回按钮（不带阴影）
    QPushButton *backBtn = new QPushButton("返回", currentSubMenu);
    backBtn->setFixedSize(120, 50);
    backBtn->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 30%);"
        "   border: 2px solid rgba(0, 0, 0, 70%);"
        "   border-radius: 10px;"
        "   color: black;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 50%);"
        "}"
        );
    connect(backBtn, &QPushButton::clicked, this, &MenuWindow::returnToMainMenu);
    subMenuLayout->addWidget(backBtn, 1, 1, Qt::AlignCenter);

    // 显示子菜单
    currentSubMenu->setGeometry(0, 0, width(), height());
    currentSubMenu->show();

    // 连接功能按钮信号槽
    connect(clockBtn, &QPushButton::clicked, this, &MenuWindow::showClockWindow);
    connect(calendarBtn, &QPushButton::clicked, this, &MenuWindow::showCalendarWindow);
    connect(weatherBtn, &QPushButton::clicked, this, &MenuWindow::showWeatherWindow);
}

void MenuWindow::switchToSubMenu(const QString &bgImagePath) {
    // 隐藏主菜单容器和底部按钮
    mainMenuContainer->hide();
    buttonContainer->hide();

    // 更新背景图
    QPixmap bgPixmap(bgImagePath);
    if (!bgPixmap.isNull()) {
        bgPixmap = bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding);
        bgLabel->setPixmap(bgPixmap);
    }

    // 创建功能子菜单
    if (currentSubMenu) {
        currentSubMenu->deleteLater();
    }
    currentSubMenu = new QWidget(this);
    currentSubMenu->setAttribute(Qt::WA_TranslucentBackground);
    QGridLayout *subMenuLayout = new QGridLayout(currentSubMenu);
    subMenuLayout->setSpacing(30);
    subMenuLayout->setContentsMargins(100, 50, 100, 50);

    // 添加功能按钮
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    const int btnWidth = screenSize.width() / 5;
    const int btnHeight = screenSize.height() / 3;

    QPushButton *clockBtn = createMenuButton(":/cat/clock.png", btnWidth, btnHeight);
    QPushButton *calendarBtn = createMenuButton(":/cat/calendar.png", btnWidth, btnHeight);
    QPushButton *weatherBtn = createMenuButton(":/cat/weather.png", btnWidth, btnHeight);

    subMenuLayout->addWidget(clockBtn, 0, 0, Qt::AlignCenter);
    subMenuLayout->addWidget(calendarBtn, 0, 1, Qt::AlignCenter);
    subMenuLayout->addWidget(weatherBtn, 0, 2, Qt::AlignCenter);

    // 返回按钮
    QPushButton *backBtn = new QPushButton("返回", currentSubMenu);
    backBtn->setFixedSize(120, 50);
    backBtn->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 30%);"
        "   border: 2px solid rgba(0, 0, 0, 70%);"
        "   border-radius: 10px;"
        "   color: black;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 50%);"
        "}");
    connect(backBtn, &QPushButton::clicked, this, &MenuWindow::returnToMainMenu);

    subMenuLayout->addWidget(backBtn, 1, 1, Qt::AlignCenter);

    // 设置子菜单位置和大小
    currentSubMenu->setGeometry(0, 0, width(), height());
    currentSubMenu->show();

    // 连接功能按钮的信号槽
    connect(clockBtn, &QPushButton::clicked, this, &MenuWindow::showClockWindow);
    connect(calendarBtn, &QPushButton::clicked, this, &MenuWindow::showCalendarWindow);
    connect(weatherBtn, &QPushButton::clicked, this, &MenuWindow::showWeatherWindow);
}

void MenuWindow::returnToMainMenu() {
    // 恢复主菜单背景
    QPixmap bgPixmap(":/cat/caiDanJM.png");
    if (!bgPixmap.isNull()) {
        bgPixmap = bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding);
        bgLabel->setPixmap(bgPixmap);
    }

    // 显示主菜单容器和底部按钮
    mainMenuContainer->show();
    buttonContainer->show();

    // 隐藏子菜单
    if (currentSubMenu) {
        currentSubMenu->hide();
    }
}

void MenuWindow::showClockWindow() {
    if (!clockWindow) {
        clockWindow = new ClockWindow();
    }
    clockWindow->show();
}

void MenuWindow::showCalendarWindow() {
    if (!calendarWindow) {
        calendarWindow = new CalendarWindow();
    }
    calendarWindow->show();
}

void MenuWindow::showWeatherWindow() {
    if (!weatherWindow) {
        weatherWindow = new WeatherWindow();
    }
    weatherWindow->show();
}
