#ifndef CLOCKWINDOW_H
#define CLOCKWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

class QPushButton;
class QTimeEdit;
class QListWidget;
class QComboBox;
class QCheckBox;

class ClockWindow : public QWidget
{
    Q_OBJECT

public:
    enum RepeatType {
        NoRepeat,
        Daily,
        Weekdays,
        Weekends,
        Custom
    };

    struct Alarm {
        QTime time;
        bool enabled;
        QString name;
        RepeatType repeat;
        QList<int> customDays; // 0=Monday, ..., 6=Sunday
        QString soundFile;
        bool vibrate;
    };

    explicit ClockWindow(QWidget *parent = nullptr);
    ~ClockWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void updateTime();
    void checkAlarms();
    void addAlarm();
    void removeAlarm();
    void toggleAlarmEnabled();
    void updateAlarmList();
    void stopAlarm();
    void snoozeAlarm();

private:
    void initUI();
    void showAlarmNotification(const QString &message);
    void saveAlarms();
    void loadAlarms();
    bool shouldAlarmTrigger(const QTime &currentTime, const Alarm &alarm);
    void vibrate();

    QLabel *timeLabel = nullptr;
    QLabel *dateLabel = nullptr;
    QTimer *timer = nullptr;
    QTimer *alarmCheckTimer = nullptr;
    QTimer *snoozeTimer = nullptr;
    QPoint dragPosition;

    // 闹钟相关UI
    QWidget *alarmPanel = nullptr;
    QTimeEdit *alarmTimeEdit = nullptr;
    QComboBox *repeatCombo = nullptr;
    QComboBox *soundCombo = nullptr;
    QCheckBox *vibrateCheck = nullptr;
    QPushButton *addAlarmButton = nullptr;
    QPushButton *toggleAlarmButton = nullptr;
    QListWidget *alarmList = nullptr;
    QPushButton *snoozeButton = nullptr;
    QPushButton *stopButton = nullptr;

    QList<Alarm> alarms;
    QMediaPlayer *alarmPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;
    Alarm currentAlarm;
    bool isAlarmPlaying = false;

    //秒表和计时器
    void updateStopwatch();    // 更新秒表显示
    void updateTimer();       // 更新计时器显示
    void startStopwatch();    // 开始秒表
    void pauseStopwatch();    // 暂停秒表
    void resetStopwatch();    // 重置秒表
    void lapStopwatch();      // 计圈功能
    void startTimer();        // 开始计时器
    void pauseTimer();        // 暂停计时器
    void resetTimer();        // 重置计时器

    // 秒表相关
    QTime stopwatchTime;
    QTimer *stopwatchTimer;
    QList<QTime> lapTimes;

    // 计时器相关
    QTime timerTime;
    QTimer *countdownTimer;
    bool isTimerRunning;

    // UI控件
    QLabel *stopwatchLabel;
    QLabel *timerLabel;
    QListWidget *lapList;
    QTimeEdit *timerInput;
};

#endif // CLOCKWINDOW_H
