#include "calendarwindow.h"
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

CalendarWindow::CalendarWindow(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(600, 500);

    initUI();
    loadNotes();
}

CalendarWindow::~CalendarWindow()
{
}

void CalendarWindow::initUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 日期标签
    dateLabel = new QLabel(QDate::currentDate().toString("yyyy年MM月dd日 dddd"), this);
    dateLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setStyleSheet("QLabel { color: white; font-size: 20px; font-weight: bold; }");

    // 日历部件
    setupCalendar();

    // 笔记区域
    setupNoteArea();

    // 保存按钮
    saveButton = new QPushButton("保存笔记", this);
    saveButton->setFixedHeight(40);
    saveButton->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 30%);"
        "   border: 2px solid rgba(255, 255, 255, 70%);"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 50%);"
        "}"
    );

    // 关闭按钮（右上角的×）
    closeButton = new QPushButton("×", this);
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
    closeButton->move(width() - 40, 10);  // 右上角位置
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

    connect(saveButton, &QPushButton::clicked, this, &CalendarWindow::saveNote);

    mainLayout->addWidget(dateLabel);
    mainLayout->addWidget(calendar);
    mainLayout->addWidget(noteEdit);
    mainLayout->addWidget(saveButton);
}

void CalendarWindow::setupCalendar()
{
    calendar = new QCalendarWidget(this);
    calendar->setStyleSheet(
        "QCalendarWidget {"
        "   background: rgba(255, 255, 255, 80%);"
        "   border: 2px solid rgba(255, 255, 255, 50%);"
        "   border-radius: 10px;"
        "}"
        "QCalendarWidget QToolButton {"
        "   height: 30px;"
        "   font-size: 14px;"
        "   color: black;"
        "   background: transparent;"
        "}"
        "QCalendarWidget QMenu {"
        "   background: white;"
        "}"
        "QCalendarWidget QSpinBox {"
        "   width: 80px;"
        "   font-size: 14px;"
        "   color: black;"
        "   background: transparent;"
        "}"
        "QCalendarWidget QWidget { alternate-background-color: rgba(200, 200, 255, 50%); }"
    );

    // 设置日期文本颜色
    QTextCharFormat weekendFormat;
    weekendFormat.setForeground(Qt::red);
    calendar->setWeekdayTextFormat(Qt::Saturday, weekendFormat);
    calendar->setWeekdayTextFormat(Qt::Sunday, weekendFormat);

    connect(calendar, &QCalendarWidget::clicked, this, &CalendarWindow::onDateSelected);
}

void CalendarWindow::setupNoteArea()
{
    noteEdit = new QTextEdit(this);
    noteEdit->setPlaceholderText("在这里记录今天的笔记...");
    noteEdit->setStyleSheet(
        "QTextEdit {"
        "   background: rgba(255, 255, 255, 80%);"
        "   border: 2px solid rgba(255, 255, 255, 50%);"
        "   border-radius: 10px;"
        "   color: black;"
        "   font-size: 14px;"
        "   padding: 5px;"
        "}"
    );
}

void CalendarWindow::onDateSelected(const QDate &date)
{
    dateLabel->setText(date.toString("yyyy年MM月dd日 dddd"));
    noteEdit->setPlainText(notes.value(date, ""));
}

void CalendarWindow::saveNote()
{
    QDate currentDate = calendar->selectedDate();
    notes[currentDate] = noteEdit->toPlainText();

    // 保存到文件
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(appDataPath);
    }

    QString filePath = appDataPath + "/calendar_notes.dat";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << notes;
        file.close();
    }
}

void CalendarWindow::loadNotes()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = appDataPath + "/calendar_notes.dat";

    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> notes;
        file.close();
    }

    // 显示当前日期的笔记
    onDateSelected(QDate::currentDate());
}

void CalendarWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制半透明背景
    painter.fillRect(rect(), QColor(0, 0, 0, 180));

    // 绘制标题栏背景
    painter.fillRect(QRect(0, 0, width(), 50), QColor(0, 0, 0, 120));

    // 绘制窗口标题
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(20, 30, "日历");

    // 绘制圆角边框
    painter.setPen(QPen(QColor(255, 255, 255, 100), 2));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 10, 10);
}
