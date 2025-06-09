#include "clockwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QDateTime>
#include <QMouseEvent>
#include <QPushButton>
ClockWindow::ClockWindow(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 200);
    initUI();
}

ClockWindow::~ClockWindow()
{
    if (timer) timer->deleteLater();
    if (timeLabel) timeLabel->deleteLater();
    if (dateLabel) dateLabel->deleteLater();
}

void ClockWindow::initUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 时间标签
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

    // 日期标签
    dateLabel = new QLabel(this);
    dateLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 18px;"
        "   padding: 10px;"
        "}"
        );

    mainLayout->addWidget(timeLabel);
    mainLayout->addWidget(dateLabel);

    // 定时器更新
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ClockWindow::updateTime);
    timer->start(1000); // 每秒更新一次
    updateTime(); // 立即更新时间

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
}

void ClockWindow::updateTime()
{
    QDateTime current = QDateTime::currentDateTime();
    timeLabel->setText(current.toString("hh:mm:ss"));
    dateLabel->setText(current.toString("yyyy-MM-dd dddd"));
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
