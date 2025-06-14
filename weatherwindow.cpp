#include "weatherwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>

WeatherWindow::WeatherWindow(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(QGuiApplication::primaryScreen()->size());

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &WeatherWindow::handleWeatherData);

    initUI();
    fetchWeatherData();
}

WeatherWindow::~WeatherWindow()
{
    networkManager->deleteLater();
}

void WeatherWindow::initUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(20);

    // 天气信息区域
    QWidget *weatherInfoWidget = new QWidget(this);
    weatherInfoWidget->setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *weatherLayout = new QVBoxLayout(weatherInfoWidget);
    weatherLayout->setSpacing(15);
    weatherLayout->setAlignment(Qt::AlignCenter);

    // 位置标签
    locationLabel = new QLabel("获取天气中...", weatherInfoWidget);
    locationLabel->setStyleSheet("QLabel { color: white; font-size: 28px; font-weight: bold; }");
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(locationLabel);
    shadowEffect->setBlurRadius(5);
    shadowEffect->setColor(Qt::black);
    shadowEffect->setOffset(1, 1);
    locationLabel->setGraphicsEffect(shadowEffect);

    // 天气图标
    weatherIcon = new QLabel(weatherInfoWidget);
    weatherIcon->setFixedSize(100, 100);
    weatherIcon->setAlignment(Qt::AlignCenter);

    // 温度标签
    tempLabel = new QLabel(weatherInfoWidget);
    tempLabel->setStyleSheet("QLabel { color: white; font-size: 48px; font-weight: bold; }");

    // 创建新的阴影效果而不是克隆
    QGraphicsDropShadowEffect *tempShadow = new QGraphicsDropShadowEffect(tempLabel);
    tempShadow->setBlurRadius(5);
    tempShadow->setColor(Qt::black);
    tempShadow->setOffset(1, 1);
    tempLabel->setGraphicsEffect(tempShadow);

    // 天气描述
    weatherDesc = new QLabel(weatherInfoWidget);
    weatherDesc->setStyleSheet("QLabel { color: white; font-size: 20px; }");

    QGraphicsDropShadowEffect *descShadow = new QGraphicsDropShadowEffect(weatherDesc);
    descShadow->setBlurRadius(5);
    descShadow->setColor(Qt::black);
    descShadow->setOffset(1, 1);
    weatherDesc->setGraphicsEffect(descShadow);

    // 湿度标签
    humidityLabel = new QLabel(weatherInfoWidget);
    humidityLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");

    QGraphicsDropShadowEffect *humidityShadow = new QGraphicsDropShadowEffect(humidityLabel);
    humidityShadow->setBlurRadius(5);
    humidityShadow->setColor(Qt::black);
    humidityShadow->setOffset(1, 1);
    humidityLabel->setGraphicsEffect(humidityShadow);

    // 风力标签
    windLabel = new QLabel(weatherInfoWidget);
    windLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");

    QGraphicsDropShadowEffect *windShadow = new QGraphicsDropShadowEffect(windLabel);
    windShadow->setBlurRadius(5);
    windShadow->setColor(Qt::black);
    windShadow->setOffset(1, 1);
    windLabel->setGraphicsEffect(windShadow);

    // 添加到天气布局
    weatherLayout->addWidget(locationLabel, 0, Qt::AlignCenter);
    weatherLayout->addWidget(weatherIcon, 0, Qt::AlignCenter);
    weatherLayout->addWidget(tempLabel, 0, Qt::AlignCenter);
    weatherLayout->addWidget(weatherDesc, 0, Qt::AlignCenter);
    weatherLayout->addWidget(humidityLabel, 0, Qt::AlignCenter);
    weatherLayout->addWidget(windLabel, 0, Qt::AlignCenter);

    // 刷新按钮
    refreshButton = new QPushButton("刷新天气", this);
    refreshButton->setFixedSize(120, 40);
    refreshButton->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 30%);"
        "   border: 2px solid white;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 50%);"
        "}");
    connect(refreshButton, &QPushButton::clicked, this, &WeatherWindow::fetchWeatherData);

    // 返回按钮
    backButton = new QPushButton("返回", this);
    backButton->setFixedSize(120, 40);
    backButton->setStyleSheet(refreshButton->styleSheet());
    connect(backButton, &QPushButton::clicked, this, &WeatherWindow::onBackClicked);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(20);

    // 添加到主布局
    mainLayout->addWidget(weatherInfoWidget);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setAlignment(buttonLayout, Qt::AlignCenter);

    setLayout(mainLayout);
}

void WeatherWindow::fetchWeatherData()
{
    // 这里使用和风天气API (需要申请免费API key)
    QString apiKey = "YOUR_API_KEY"; // 替换为你的API key
    QString location = "auto_ip"; // 自动获取当前位置

    QUrl url(QString("https://devapi.qweather.com/v7/weather/now?location=%1&key=%2")
                 .arg(location).arg(apiKey));

    QNetworkRequest request(url);
    networkManager->get(request);
}

void WeatherWindow::handleWeatherData(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        locationLabel->setText("获取天气失败");
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        locationLabel->setText("数据解析失败");
        return;
    }

    QJsonObject obj = doc.object();
    if (obj["code"].toString() != "200") {
        locationLabel->setText("天气服务错误: " + obj["code"].toString());
        return;
    }

    updateWeatherDisplay(obj);
}

void WeatherWindow::updateWeatherDisplay(const QJsonObject &weatherData)
{
    QJsonObject now = weatherData["now"].toObject();

    // 更新位置信息
    currentLocation = weatherData["location"].toObject()["name"].toString();
    locationLabel->setText(currentLocation);

    // 更新温度
    tempLabel->setText(now["temp"].toString() + "°C");

    // 更新天气描述
    weatherDesc->setText(now["text"].toString());

    // 更新湿度
    humidityLabel->setText("湿度: " + now["humidity"].toString() + "%");

    // 更新风力
    windLabel->setText("风力: " + now["windScale"].toString() + "级");

    // 设置天气图标 (需要准备相应的图标资源)
    QString iconCode = now["icon"].toString();
    QPixmap icon(QString(":/weather/icons/%1.png").arg(iconCode));
    if (!icon.isNull()) {
        weatherIcon->setPixmap(icon.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void WeatherWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0, 0, 0, 180)); // 半透明黑色背景
}

void WeatherWindow::onBackClicked()
{
    this->close();
}
