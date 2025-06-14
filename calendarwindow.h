#ifndef CALENDARWINDOW_H
#define CALENDARWINDOW_H

#include <QWidget>
#include <QCalendarWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QDate>
#include <QPushButton>
class CalendarWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarWindow(QWidget *parent = nullptr);
    ~CalendarWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onDateSelected(const QDate &date);
    void saveNote();
    void loadNotes();

private:
    void initUI();
    void setupCalendar();
    void setupNoteArea();

    QCalendarWidget *calendar;
    QTextEdit *noteEdit;
    QLabel *dateLabel;
    QPushButton *saveButton;
    QMap<QDate, QString> notes; // 存储日期和对应的笔记
    QPushButton *closeButton; // 添加退出按钮
};

#endif // CALENDARWINDOW_H
