#include "clockwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QDateTime>
#include <QMouseEvent>
#include <QTimeEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QLineEdit>
#include <QSoundEffect>
#include <QSettings>
#include <QMessageBox>
#include <QPushButton>
#include <QComboBox>
#include <QMediaPlayer>
#include <QGuiApplication>
#include <QWindow>
#include <QAudioOutput>

ClockWindow::ClockWindow(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(500, 700); // 增大窗口以容纳新功能，能设置时钟工具界面的大小
    initUI();
    loadAlarms();

    // 初始化闹钟播放器
    // 修改播放器初始化代码
    alarmPlayer = new QMediaPlayer(this);
    QAudioOutput *audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0.5);  // 0.0~1.0范围
    alarmPlayer->setAudioOutput(audioOutput);  // 关联音频输出

    updateTime();
}

ClockWindow::~ClockWindow()
{
    saveAlarms();
    if (timer) timer->deleteLater();
    if (alarmCheckTimer) alarmCheckTimer->deleteLater();
    if (snoozeTimer) snoozeTimer->deleteLater();
    if (alarmPlayer) alarmPlayer->deleteLater();
}

void ClockWindow::initUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    // 时间日期显示
    QWidget *timeDateWidget = new QWidget(this);
    QVBoxLayout *timeDateLayout = new QVBoxLayout(timeDateWidget);
    timeDateLayout->setContentsMargins(0, 0, 0, 0);

    timeLabel = new QLabel(this);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 48px;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "}"
        );

    dateLabel = new QLabel(this);
    dateLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 18px;"
        "   padding: 10px;"
        "}"
        );

    timeDateLayout->addWidget(timeLabel);
    timeDateLayout->addWidget(dateLabel);
    mainLayout->addWidget(timeDateWidget);

    // 闹钟面板
    alarmPanel = new QWidget(this);
    alarmPanel->setStyleSheet("background: rgba(255, 255, 255, 20%); border-radius: 10px;");
    QVBoxLayout *alarmLayout = new QVBoxLayout(alarmPanel);
    alarmLayout->setContentsMargins(10, 10, 10, 10);

    // 添加闹钟控件
    QWidget *addAlarmWidget = new QWidget(alarmPanel);
    QVBoxLayout *addAlarmLayout = new QVBoxLayout(addAlarmWidget);
    addAlarmLayout->setContentsMargins(0, 0, 0, 0);

    // 时间行
    QWidget *timeRow = new QWidget(addAlarmWidget);
    QHBoxLayout *timeRowLayout = new QHBoxLayout(timeRow);
    timeRowLayout->setContentsMargins(0, 0, 0, 0);

    alarmTimeEdit = new QTimeEdit(timeRow);
    alarmTimeEdit->setDisplayFormat("hh:mm");
    alarmTimeEdit->setStyleSheet(
        "QTimeEdit {"
        "   background: white;"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        );

    QLineEdit *alarmNameEdit = new QLineEdit(timeRow);
    alarmNameEdit->setPlaceholderText("闹钟名称");
    alarmNameEdit->setStyleSheet(
        "QLineEdit {"
        "   background: white;"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        );

    timeRowLayout->addWidget(alarmTimeEdit);
    timeRowLayout->addWidget(alarmNameEdit);
    addAlarmLayout->addWidget(timeRow);

    // 设置行
    QWidget *settingsRow = new QWidget(addAlarmWidget);
    QHBoxLayout *settingsRowLayout = new QHBoxLayout(settingsRow);
    settingsRowLayout->setContentsMargins(0, 0, 0, 0);

    repeatCombo = new QComboBox(settingsRow);
    repeatCombo->addItem("不重复", NoRepeat);
    repeatCombo->addItem("每天", Daily);
    repeatCombo->addItem("工作日", Weekdays);
    repeatCombo->addItem("周末", Weekends);
    repeatCombo->addItem("自定义", Custom);
    repeatCombo->setStyleSheet(
        "QComboBox {"
        "   background: white;"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        );

    soundCombo = new QComboBox(settingsRow);
    soundCombo->addItem("默认", ":/sounds/alarm.wav");
    soundCombo->addItem("蜂鸣", ":/sounds/beep.wav");
    soundCombo->addItem("鸟鸣", ":/sounds/bird.wav");
    soundCombo->setStyleSheet(repeatCombo->styleSheet());

    vibrateCheck = new QCheckBox("振动", settingsRow);
    vibrateCheck->setStyleSheet("QCheckBox { color: white; background: transparent; }");

    settingsRowLayout->addWidget(repeatCombo);
    settingsRowLayout->addWidget(soundCombo);
    settingsRowLayout->addWidget(vibrateCheck);
    addAlarmLayout->addWidget(settingsRow);

    // 添加按钮
    addAlarmButton = new QPushButton("添加", addAlarmWidget);
    addAlarmButton->setStyleSheet(
        "QPushButton {"
        "   background: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 5px 10px;"
        "}"
        "QPushButton:hover {"
        "   background: #45a049;"
        "}"
        );

    connect(addAlarmButton, &QPushButton::clicked, this, [this, alarmNameEdit]() {
        Alarm alarm;
        alarm.time = alarmTimeEdit->time();
        alarm.name = alarmNameEdit->text().isEmpty() ? "闹钟" : alarmNameEdit->text();
        alarm.enabled = true;
        alarm.repeat = static_cast<RepeatType>(repeatCombo->currentData().toInt());
        alarm.soundFile = soundCombo->currentData().toString();
        alarm.vibrate = vibrateCheck->isChecked();
        alarms.append(alarm);
        updateAlarmList();
        alarmNameEdit->clear();
    });

    addAlarmLayout->addWidget(addAlarmButton);
    alarmLayout->addWidget(addAlarmWidget);

    // 闹钟列表
    alarmList = new QListWidget(alarmPanel);
    alarmList->setStyleSheet(
        "QListWidget {"
        "   background: white;"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "}"
        "QListWidget::item {"
        "   border-bottom: 1px solid #eee;"
        "}"
        );
    alarmLayout->addWidget(alarmList);

    // 闹钟控制按钮
    QWidget *alarmControlWidget = new QWidget(alarmPanel);
    QHBoxLayout *alarmControlLayout = new QHBoxLayout(alarmControlWidget);
    alarmControlLayout->setContentsMargins(0, 0, 0, 0);

    toggleAlarmButton = new QPushButton("禁用选中", alarmControlWidget);
    toggleAlarmButton->setStyleSheet(
        "QPushButton {"
        "   background: #f44336;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 5px 10px;"
        "}"
        "QPushButton:hover {"
        "   background: #d32f2f;"
        "}"
        );

    QPushButton *removeAlarmButton = new QPushButton("删除选中", alarmControlWidget);
    removeAlarmButton->setStyleSheet(toggleAlarmButton->styleSheet());

    connect(toggleAlarmButton, &QPushButton::clicked, this, &ClockWindow::toggleAlarmEnabled);
    connect(removeAlarmButton, &QPushButton::clicked, this, &ClockWindow::removeAlarm);

    alarmControlLayout->addWidget(toggleAlarmButton);
    alarmControlLayout->addWidget(removeAlarmButton);
    alarmLayout->addWidget(alarmControlWidget);

    mainLayout->addWidget(alarmPanel);

    // 关闭按钮
    QPushButton *closeButton = new QPushButton("×", this);
    closeButton->setFixedSize(30, 30);
    closeButton->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 30%);"
        "   border: none;"
        "   border-radius: 15px;"
        "   color: white;"
        "   font-size: 20px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 0, 0, 50%);"
        "}"
        );
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

    // 将关闭按钮放在右上角
    closeButton->move(width() - closeButton->width() - 10, 10);

    // 定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ClockWindow::updateTime);
    timer->start(1000);

    alarmCheckTimer = new QTimer(this);
    connect(alarmCheckTimer, &QTimer::timeout, this, &ClockWindow::checkAlarms);
    alarmCheckTimer->start(1000); // 每秒检查一次闹钟

    // 贪睡定时器
    snoozeTimer = new QTimer(this);
    snoozeTimer->setSingleShot(true);
    connect(snoozeTimer, &QTimer::timeout, this, [this]() {
        if (isAlarmPlaying) {
            alarmPlayer->play();
            if (currentAlarm.vibrate) vibrate();
        }
    });

    // // 初始化闹钟播放器
    // alarmPlayer = new QMediaPlayer(this);
    // alarmPlayer->setVolume(50);

    updateTime();

    // 添加秒表面板
    QWidget *stopwatchPanel = new QWidget(this);
    stopwatchPanel->setStyleSheet("background: rgba(255, 255, 255, 20%); border-radius: 10px;");
    QVBoxLayout *stopwatchLayout = new QVBoxLayout(stopwatchPanel);

    // 秒表显示
    stopwatchLabel = new QLabel("00:00:00.000", stopwatchPanel);
    stopwatchLabel->setAlignment(Qt::AlignCenter);
    stopwatchLabel->setStyleSheet("QLabel { color: white; font-size: 24px; }");

    // 秒表按钮
    QWidget *stopwatchButtons = new QWidget(stopwatchPanel);
    QHBoxLayout *buttonLayout = new QHBoxLayout(stopwatchButtons);

    QPushButton *startBtn = new QPushButton("开始", stopwatchButtons);
    QPushButton *pauseBtn = new QPushButton("暂停", stopwatchButtons);
    QPushButton *resetBtn = new QPushButton("重置", stopwatchButtons);
    QPushButton *lapBtn = new QPushButton("计圈", stopwatchButtons);

    connect(startBtn, &QPushButton::clicked, this, &ClockWindow::startStopwatch);
    connect(pauseBtn, &QPushButton::clicked, this, &ClockWindow::pauseStopwatch);
    connect(resetBtn, &QPushButton::clicked, this, &ClockWindow::resetStopwatch);
    connect(lapBtn, &QPushButton::clicked, this, &ClockWindow::lapStopwatch);

    buttonLayout->addWidget(startBtn);
    buttonLayout->addWidget(pauseBtn);
    buttonLayout->addWidget(resetBtn);
    buttonLayout->addWidget(lapBtn);

    // 计圈列表
    lapList = new QListWidget(stopwatchPanel);
    lapList->setStyleSheet("QListWidget { background: white; border-radius: 5px; }");

    stopwatchLayout->addWidget(stopwatchLabel);
    stopwatchLayout->addWidget(stopwatchButtons);
    stopwatchLayout->addWidget(lapList);

    // 添加计时器面板
    QWidget *timerPanel = new QWidget(this);
    timerPanel->setStyleSheet("background: rgba(255, 255, 255, 20%); border-radius: 10px;");
    QVBoxLayout *timerLayout = new QVBoxLayout(timerPanel);

    // 计时器显示和输入
    timerLabel = new QLabel("00:00:00", timerPanel);
    timerLabel->setAlignment(Qt::AlignCenter);
    timerLabel->setStyleSheet("QLabel { color: white; font-size: 24px; }");

    timerInput = new QTimeEdit(QTime(0, 1, 0), timerPanel); // 默认1分钟
    timerInput->setDisplayFormat("hh:mm:ss");

    // 计时器按钮
    QWidget *timerButtons = new QWidget(timerPanel);
    QHBoxLayout *timerBtnLayout = new QHBoxLayout(timerButtons);

    QPushButton *timerStartBtn = new QPushButton("开始", timerButtons);
    QPushButton *timerPauseBtn = new QPushButton("暂停", timerButtons);
    QPushButton *timerResetBtn = new QPushButton("重置", timerButtons);

    connect(timerStartBtn, &QPushButton::clicked, this, &ClockWindow::startTimer);
    connect(timerPauseBtn, &QPushButton::clicked, this, &ClockWindow::pauseTimer);
    connect(timerResetBtn, &QPushButton::clicked, this, &ClockWindow::resetTimer);

    timerBtnLayout->addWidget(timerStartBtn);
    timerBtnLayout->addWidget(timerPauseBtn);
    timerBtnLayout->addWidget(timerResetBtn);

    timerLayout->addWidget(timerLabel);
    timerLayout->addWidget(timerInput);
    timerLayout->addWidget(timerButtons);

    // 将秒表和计时器面板添加到主布局
    QHBoxLayout *toolsLayout = new QHBoxLayout();
    toolsLayout->addWidget(stopwatchPanel);
    toolsLayout->addWidget(timerPanel);
    mainLayout->addLayout(toolsLayout);

    // 初始化定时器
    stopwatchTimer = new QTimer(this);
    connect(stopwatchTimer, &QTimer::timeout, this, &ClockWindow::updateStopwatch);

    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &ClockWindow::updateTimer);

    resetStopwatch();
    resetTimer();
}

void ClockWindow::updateTime()
{
    QDateTime current = QDateTime::currentDateTime();
    timeLabel->setText(current.toString("hh:mm:ss"));
    dateLabel->setText(current.toString("yyyy-MM-dd dddd"));
}

void ClockWindow::checkAlarms()
{
    if (isAlarmPlaying) return; // 如果已经有闹钟在响，不再检查

    QTime current = QTime::currentTime();
    QDate today = QDate::currentDate();
    int dayOfWeek = today.dayOfWeek(); // 1=Monday, ..., 7=Sunday

    for (const Alarm &alarm : alarms) {
        if (alarm.enabled && shouldAlarmTrigger(current, alarm)) {
            currentAlarm = alarm;
            isAlarmPlaying = true;

            // 播放声音
            alarmPlayer->setSource(QUrl::fromLocalFile(alarm.soundFile));  // Qt6使用setSource
            audioOutput->setVolume(0.5);  // 设置音量
            alarmPlayer->play();

            // 振动
            if (alarm.vibrate) vibrate();

            // 显示通知
            showAlarmNotification(alarm.name.isEmpty() ? "闹钟响了！" : alarm.name);
            break;
        }
    }
}

bool ClockWindow::shouldAlarmTrigger(const QTime &currentTime, const Alarm &alarm)
{
    if (alarm.time.toString("hh:mm") != currentTime.toString("hh:mm")) {
        return false;
    }

    QDate today = QDate::currentDate();
    int dayOfWeek = today.dayOfWeek(); // 1=Monday, ..., 7=Sunday

    switch (alarm.repeat) {
    case NoRepeat:
        return true;
    case Daily:
        return true;
    case Weekdays:
        return dayOfWeek >= 1 && dayOfWeek <= 5; // Monday to Friday
    case Weekends:
        return dayOfWeek == 6 || dayOfWeek == 7; // Saturday or Sunday
    case Custom:
        return alarm.customDays.contains(dayOfWeek - 1); // 0-based
    default:
        return false;
    }
}

void ClockWindow::stopAlarm()
{
    if (isAlarmPlaying) {
        alarmPlayer->stop();
        isAlarmPlaying = false;
    }
}

void ClockWindow::snoozeAlarm()
{
    stopAlarm();
    snoozeTimer->start(5 * 60 * 1000); // 5分钟后再次响起
}

void ClockWindow::vibrate()
{
// 在支持振动的平台上实现振动
// 这里只是一个示例，实际实现取决于平台API
#ifdef Q_OS_ANDROID
    // 使用Android振动API
#elif defined(Q_OS_IOS)
    // 使用iOS振动API
#endif
}

// 秒表功能实现
void ClockWindow::startStopwatch()
{
    if (!stopwatchTimer->isActive()) {
        stopwatchTimer->start(10); // 10毫秒更新一次，实现毫秒级精度
    }
}

void ClockWindow::pauseStopwatch()
{
    stopwatchTimer->stop();
}

void ClockWindow::resetStopwatch()
{
    stopwatchTimer->stop();
    stopwatchTime = QTime(0, 0, 0, 0);
    lapTimes.clear();
    lapList->clear();
    stopwatchLabel->setText("00:00:00.000");
}

void ClockWindow::lapStopwatch()
{
    if (stopwatchTimer->isActive()) {
        lapTimes.prepend(stopwatchTime); // 添加到开头，最新记录显示在最上面
        lapList->insertItem(0, stopwatchTime.toString("hh:mm:ss.zzz"));
    }
}

void ClockWindow::updateStopwatch()
{
    stopwatchTime = stopwatchTime.addMSecs(10);
    stopwatchLabel->setText(stopwatchTime.toString("hh:mm:ss.zzz"));
}

// 计时器功能实现
void ClockWindow::startTimer()
{
    if (!countdownTimer->isActive()) {
        if (!isTimerRunning) {
            timerTime = timerInput->time();
            isTimerRunning = true;
        }
        countdownTimer->start(1000); // 1秒更新一次
        updateTimer();
    }
}

void ClockWindow::pauseTimer()
{
    countdownTimer->stop();
    isTimerRunning = false;
}

void ClockWindow::resetTimer()
{
    countdownTimer->stop();
    timerTime = timerInput->time();
    isTimerRunning = false;
    timerLabel->setText(timerTime.toString("hh:mm:ss"));
}

void ClockWindow::updateTimer()
{
    if (timerTime == QTime(0, 0, 0)) {
        countdownTimer->stop();
        isTimerRunning = false;
        // 播放提示音或震动
        QMessageBox::information(this, "提示", "计时结束！");
        return;
    }

    timerTime = timerTime.addSecs(-1);
    timerLabel->setText(timerTime.toString("hh:mm:ss"));
}

//闹钟功能实现
void ClockWindow::showAlarmNotification(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("闹钟");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);

    stopButton = msgBox.addButton("停止", QMessageBox::ActionRole);
    snoozeButton = msgBox.addButton("贪睡(5分钟)", QMessageBox::ActionRole);

    connect(stopButton, &QPushButton::clicked, this, &ClockWindow::stopAlarm);
    connect(snoozeButton, &QPushButton::clicked, this, &ClockWindow::snoozeAlarm);

    msgBox.exec();
}

void ClockWindow::updateAlarmList()
{
    alarmList->clear();
    for (int i = 0; i < alarms.size(); ++i) {
        const Alarm &alarm = alarms[i];
        QListWidgetItem *item = new QListWidgetItem(alarmList);

        QWidget *itemWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(5, 5, 5, 5);

        QCheckBox *enabledCheck = new QCheckBox(itemWidget);
        enabledCheck->setChecked(alarm.enabled);
        enabledCheck->setStyleSheet("QCheckBox { background: transparent; }");

        connect(enabledCheck, &QCheckBox::toggled, this, [this, i](bool checked) {
            alarms[i].enabled = checked;
            saveAlarms();
        });

        QLabel *timeLabel = new QLabel(alarm.time.toString("hh:mm"), itemWidget);
        timeLabel->setStyleSheet("QLabel { color: black; font-weight: bold; }");

        QLabel *nameLabel = new QLabel(alarm.name, itemWidget);
        nameLabel->setStyleSheet("QLabel { color: #555; }");

        QString repeatText;
        switch (alarm.repeat) {
        case NoRepeat: repeatText = "不重复"; break;
        case Daily: repeatText = "每天"; break;
        case Weekdays: repeatText = "工作日"; break;
        case Weekends: repeatText = "周末"; break;
        case Custom: repeatText = "自定义"; break;
        }

        QLabel *repeatLabel = new QLabel(repeatText, itemWidget);
        repeatLabel->setStyleSheet("QLabel { color: #777; font-size: 10px; }");

        layout->addWidget(enabledCheck);
        layout->addWidget(timeLabel);
        layout->addWidget(nameLabel);
        layout->addWidget(repeatLabel);
        layout->addStretch();

        itemWidget->setLayout(layout);
        alarmList->setItemWidget(item, itemWidget);
        item->setSizeHint(itemWidget->sizeHint());
    }
    saveAlarms();
}

void ClockWindow::addAlarm()
{
    Alarm alarm;
    alarm.time = alarmTimeEdit->time();
    alarm.enabled = true;
    alarms.append(alarm);
    updateAlarmList();
}

void ClockWindow::removeAlarm()
{
    QList<QListWidgetItem*> selected = alarmList->selectedItems();
    for (QListWidgetItem *item : selected) {
        int row = alarmList->row(item);
        if (row >= 0 && row < alarms.size()) {
            alarms.removeAt(row);
        }
    }
    updateAlarmList();
}

void ClockWindow::toggleAlarmEnabled()
{
    QList<QListWidgetItem*> selected = alarmList->selectedItems();
    for (QListWidgetItem *item : selected) {
        int row = alarmList->row(item);
        if (row >= 0 && row < alarms.size()) {
            alarms[row].enabled = !alarms[row].enabled;
        }
    }
    updateAlarmList();
}

void ClockWindow::saveAlarms()
{
    QSettings settings("DesktopPet", "Alarms");
    settings.beginWriteArray("alarms");
    for (int i = 0; i < alarms.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("time", alarms[i].time);
        settings.setValue("enabled", alarms[i].enabled);
        settings.setValue("name", alarms[i].name);
        settings.setValue("repeat", static_cast<int>(alarms[i].repeat)); // 如果是枚举类型需要转换
        settings.setValue("soundFile", alarms[i].soundFile);
        settings.setValue("vibrate", alarms[i].vibrate);
        settings.setValue("customDays", QVariant::fromValue(alarms[i].customDays)); // 关键修改
    }
    settings.endArray();
}

void ClockWindow::loadAlarms()
{
    QSettings settings("DesktopPet", "Alarms");
    int size = settings.beginReadArray("alarms");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        Alarm alarm;
        alarm.time = settings.value("time").toTime();
        alarm.enabled = settings.value("enabled").toBool();
        alarm.name = settings.value("name").toString();
        alarm.repeat = static_cast<RepeatType>(settings.value("repeat").toInt());
        alarm.soundFile = settings.value("soundFile").toString();
        alarm.vibrate = settings.value("vibrate").toBool();
        alarm.customDays = settings.value("customDays").value<QList<int>>(); // 关键修改
        alarms.append(alarm);
    }
    settings.endArray();
}

void ClockWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制半透明圆角矩形背景
    painter.setBrush(QColor(0, 0, 0, 180));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 10, 10);
}

void ClockWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void ClockWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}
