#include "menuwindow.h"
#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QCloseEvent>
#include <QGraphicsDropShadowEffect>//文字阴影
MenuWindow::MenuWindow(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(QGuiApplication::primaryScreen()->size());
    initUI();
}

MenuWindow::~MenuWindow() {
    if (bgLabel) bgLabel->deleteLater();
    if (mainMenuContainer) mainMenuContainer->deleteLater();
    if (buttonContainer) buttonContainer->deleteLater();
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

    // 主菜单容器 (网格布局)
    mainMenuContainer = new QWidget(this);
    mainMenuContainer->setAttribute(Qt::WA_TranslucentBackground);

    // 使用网格布局排列主菜单按钮
    QGridLayout *mainMenuLayout = new QGridLayout(mainMenuContainer);
    mainMenuLayout->setSpacing(30);  // 按钮间距
    mainMenuLayout->setContentsMargins(50, 50, 50, 50); // 边距

    // 添加主菜单按钮
    setupMainMenuButtons(mainMenuLayout);

    // 设置主菜单容器位置和大小
    mainMenuContainer->setGeometry(0, 0, width(), height());

    // 创建底部按钮容器 (返回/退出)
    createBottomButtons();

    // 设置窗口样式
    setStyleSheet("background: transparent;");
}

void MenuWindow::setupMainMenuButtons(QGridLayout *layout) {
    // 根据图片中的菜单项创建按钮
    const int btnWidth = 180;
    const int btnHeight = 180;

    // 时钟按钮
    QPushButton *clockBtn = createMenuButton("时钟", ":/icons/clock.png", btnWidth, btnHeight);
    layout->addWidget(clockBtn, 0, 0);

    // 交互按钮
    QPushButton *interactionBtn = createMenuButton("交互", ":/icons/interact.png", btnWidth, btnHeight);
    layout->addWidget(interactionBtn, 0, 1);

    // 日历按钮
    QPushButton *calendarBtn = createMenuButton("日历", ":/icons/calendar.png", btnWidth, btnHeight);
    layout->addWidget(calendarBtn, 0, 2);

    // 素材按钮
    QPushButton *materialBtn = createMenuButton("素材", ":/icons/material.png", btnWidth, btnHeight);
    layout->addWidget(materialBtn, 1, 0);

    // 天气按钮
    QPushButton *weatherBtn = createMenuButton("天气", ":/icons/weather.png", btnWidth, btnHeight);
    layout->addWidget(weatherBtn, 1, 1);

    // 养成按钮
    QPushButton *trainingBtn = createMenuButton("养成", ":/icons/training.png", btnWidth, btnHeight);
    layout->addWidget(trainingBtn, 1, 2);

    // 连接信号槽
    connect(clockBtn, &QPushButton::clicked, this, &MenuWindow::onClockClicked);
    connect(interactionBtn, &QPushButton::clicked, this, &MenuWindow::onInteractionClicked);
    connect(calendarBtn, &QPushButton::clicked, this, &MenuWindow::onCalendarClicked);
    connect(materialBtn, &QPushButton::clicked, this, &MenuWindow::onMaterialClicked);
    connect(weatherBtn, &QPushButton::clicked, this, &MenuWindow::onWeatherClicked);
    connect(trainingBtn, &QPushButton::clicked, this, &MenuWindow::onTrainingClicked);
}

QPushButton* MenuWindow::createMenuButton(const QString &text, const QString &iconPath, int width, int height) {
    QPushButton *button = new QPushButton(this);
    button->setFixedSize(width, height);

    // 设置按钮样式 - 圆形图标按钮
    button->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 30%);"
        "   border: 2px solid rgba(0, 0, 0, 70%);"  // 黑色半透明边框
        "   border-radius: 50%;"                    // 完美圆形（自适应按钮大小）
        "   color: black;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 50%);"
        "   border: 2px solid black;"               // 悬停时纯黑边框
        "}"
        );

    // 垂直布局：图标 + 文字
    QVBoxLayout *btnLayout = new QVBoxLayout(button);
    btnLayout->setContentsMargins(0, 15, 0, 15);  // 调整上下边距
    btnLayout->setSpacing(8);  // 图标和文字的间距

    // 图标（居中，固定大小）
    QLabel *iconLabel = new QLabel(button);
    QPixmap icon(iconPath);
    if (!icon.isNull()) {
        // 动态计算尺寸（按钮高度的45%）
        int iconSize = qMin(button->height() * 0.45, 100.0); // 不超过100px
        icon = icon.scaled(iconSize, iconSize,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
    }
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel *textLabel = new QLabel(text, button);
    textLabel->setAlignment(Qt::AlignCenter);

    // 创建阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(textLabel);
    shadowEffect->setBlurRadius(3);       // 阴影模糊半径
    shadowEffect->setColor(Qt::black);    // 阴影颜色
    shadowEffect->setOffset(1, 1);        // 阴影偏移量
    textLabel->setGraphicsEffect(shadowEffect);

    textLabel->setStyleSheet(
        "color: black;"
        "font-size: 18px;"
        "font-weight: bold;"
        );

    btnLayout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    btnLayout->addWidget(textLabel, 0, Qt::AlignHCenter);

    return button;
}

void MenuWindow::createBottomButtons() {
    // 创建底部按钮容器
    buttonContainer = new QWidget(this);
    buttonContainer->setAttribute(Qt::WA_TranslucentBackground);

    // 水平布局排列按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    // 添加底部按钮
    backButton = new QPushButton("返回", this);
    backButton->setFixedSize(120, 50);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 30%);"
        "   border: 2px solid rgba(0, 0, 0, 70%);" //黑色边框
        "   border-radius: 10px;"
        "   color: black;" //黑色文字
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 50%);"
        "}"
        );
    connect(backButton, &QPushButton::clicked, this, &QWidget::close);

    exitButton = new QPushButton("退出", this);
    exitButton->setFixedSize(120, 50);
    exitButton->setStyleSheet(backButton->styleSheet());
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(exitButton);

    // 定位底部按钮容器
    const int marginBottom = 30;
    buttonContainer->move((width() - buttonContainer->sizeHint().width()) / 2,
                          height() - 50 - marginBottom);
    buttonContainer->adjustSize();
}

// 以下是各个菜单项的槽函数实现
void MenuWindow::onClockClicked() {
    //打开时钟功能
    if (!clockWindow) {
        clockWindow = new ClockWindow();
        connect(clockWindow, &ClockWindow::destroyed, this, [this]() {
            clockWindow = nullptr;
        });
    }
    clockWindow->show();
}

void MenuWindow::onInteractionClicked() {
    // 打开交互功能
}

void MenuWindow::onCalendarClicked() {
    // 打开日历功能
}

void MenuWindow::onMaterialClicked() {
    // 打开素材功能
}

void MenuWindow::onWeatherClicked() {
    // 打开天气功能
}

void MenuWindow::onTrainingClicked() {
    // 打开养成功能
}

void MenuWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0, 0, 0, 150)); // 半透明黑色背景
}

void MenuWindow::closeEvent(QCloseEvent *event)
{
    emit closed();       // 发射关闭信号
    QWidget::closeEvent(event);  // 调用基类实现
}

//ESC退出全屏模式
void MenuWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        this->close();
    }
    QWidget::keyPressEvent(event);
}

