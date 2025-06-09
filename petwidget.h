#ifndef PETWIDGET_H
#define PETWIDGET_H
//主窗口界面加载
#include <QWidget>
#include <QPixmap>
#include <QPushButton>

class MenuWindow; // 前向声明

class PetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PetWidget(QWidget *parent = nullptr);
    ~PetWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void setCustomShape();
    void createMenuButton();
    void updateScaledImage();

    QPixmap m_originalImage;
    QPixmap m_scaledImage;
    QPoint m_dragPosition;
    double m_scaleFactor = 1.0;
    const double ZOOM_STEP = 0.1;

    QPushButton *menuButton = nullptr;
    MenuWindow *menuWindow = nullptr; // 添加菜单窗口指针
};

#endif // PETWIDGET_H
