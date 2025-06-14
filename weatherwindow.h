#ifndef WEATHERWINDOW_H
#define WEATHERWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
class WeatherWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherWindow(QWidget *parent = nullptr);
    ~WeatherWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void fetchWeatherData();
    void handleWeatherData(QNetworkReply *reply);
    void onBackClicked();

private:
    void initUI();
    void updateWeatherDisplay(const QJsonObject &weatherData);

    QNetworkAccessManager *networkManager;

    // UI 元素
    QLabel *locationLabel;
    QLabel *tempLabel;
    QLabel *weatherIcon;
    QLabel *weatherDesc;
    QLabel *humidityLabel;
    QLabel *windLabel;
    QPushButton *refreshButton;
    QPushButton *backButton;

    // 天气数据
    QString currentLocation;
};

#endif // WEATHERWINDOW_H
