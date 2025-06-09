#ifndef CLOCKWINDOW_H
#define CLOCKWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QTime>

class ClockWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ClockWindow(QWidget *parent = nullptr);
    ~ClockWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void updateTime();

private:
    void initUI();

    QLabel *timeLabel = nullptr;
    QLabel *dateLabel = nullptr;
    QTimer *timer = nullptr;
    QPoint dragPosition;
};

#endif // CLOCKWINDOW_H
