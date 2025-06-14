#include "petwidget.h"
#include "menuwindow.h"
#include <QMenu>
#include <QPushButton>  // 如果使用QPushButton也需要包含
#include <QPainter>
#include <QFile>
#include <QDebug>
#include <QBitmap>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
//主窗口界面加载
PetWidget::PetWidget(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{/*Qt::FramelessWindowHint	-可创建任意形状的非矩形窗口。*//*Qt::WindowStaysOnTopHint	-强制窗口始终显示在其他窗口之上。*/
    setAttribute(Qt::WA_TranslucentBackground);/*-使窗口背景变为完全透明，仅显示你主动绘制的内容*/

    // 加载原始图像(原始图像-m_originalImage)
    if(!m_originalImage.load(":/cat/catWidget.png")) {/*Qt资源系统路径*/
        qDebug() << "Failed to load image!";

        // 创建占位图像
        m_originalImage = QPixmap(200, 200);/*宽 200px，高 100px*/
        m_originalImage.fill(Qt::transparent);/*填充透明背景*/
        QPainter p(&m_originalImage);
        p.drawText(m_originalImage.rect(), Qt::AlignCenter, "Placeholder Image");
    }

    // 设置初始缩放为50%，原定图片尺寸
    m_scaleFactor = 0.5;  // 新增：初始化缩放系数(当前缩放系数-m_scaleFactor)
    updateScaledImage();    // 应用初始缩放

    // 初始位置设为屏幕右下角，可修改
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.width() - width() - 20,  // X坐标：屏幕宽度-窗口宽度-边距
         screenGeometry.height() - height() - 20); // Y坐标同理
    // move(20, 20);  // 直接使用固定偏移量
    // 左上角定位不需要计算屏幕尺寸，因为坐标(0,0)就是左上角原点

    // 创建菜单按钮
    qDebug() << "正在创建菜单按钮...";
    createMenuButton();
    qDebug() << "菜单按钮创建完成";
}

void PetWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform); // 平滑缩放
    painter.drawPixmap(rect(), m_scaledImage);
}

//设置窗口的自定义边框形状
void PetWidget::setCustomShape()
{
    if(m_scaledImage.isNull()) return;

    QBitmap mask = m_scaledImage.mask();//遮罩
    if(!mask.isNull()) {
        setMask(mask);
    }
}

void PetWidget::createMenuButton() {
    if(menuButton) {
        menuButton->deleteLater();
        menuButton = nullptr;
    }

    // 创建菜单按钮
    menuButton = new QPushButton(this);

    // 设置按钮属性
    const QIcon menuIcon(":/cat/caiDan.png");
    if(menuIcon.isNull()) {
        qDebug() << "Failed to load menu icon!";
        menuButton->setText("Menu");
    } else {
        menuButton->setIcon(menuIcon);
        menuButton->setIconSize(QSize(100, 100));
    }

    // 设置按钮大小和样式
    int buttonSize = qMin(width(), height()) / 4;
    menuButton->setFixedSize(buttonSize, buttonSize);
    menuButton->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   background: rgba(255, 255, 255, 00%);"
        "   border-radius: " + QString::number(buttonSize/2) + "px;"
                                            "}"
                                            "QPushButton:hover {"
                                            "   background: rgba(255, 255, 255, 50%);"
                                            "}"
        );

    // 将按钮放在右下角
    QPoint buttonPos(
        width() - menuButton->width() - width() * 0.20,
        height() - menuButton->height() - height() * 0.20
        );
    menuButton->move(buttonPos);

    // 连接菜单按钮点击事件
    connect(menuButton, &QPushButton::clicked, this, [this]() {
        if (!menuWindow) {
            menuWindow = new MenuWindow();
            connect(menuWindow, &MenuWindow::closed, this, [this]() {
                menuWindow->deleteLater();
                menuWindow = nullptr;
            });
        }
        menuWindow->show();
    });

    menuButton->raise();
}

// 应用初始缩放
void PetWidget::updateScaledImage()
{
    qDebug() << "=== Enter updateScaledImage() ===";

    // 1. 检查原始图像
    if(m_originalImage.isNull()) {
        qDebug() << "Original image is null!";
        return;
    }
    qDebug() << "Original image size:" << m_originalImage.size();

    // 2. 计算新尺寸
    QSize newSize = m_originalImage.size() * m_scaleFactor;
    if(newSize.isEmpty()) {
        qDebug() << "Calculated size is empty!";
        return;
    }
    qDebug() << "New scaled size:" << newSize;

    // 3. 平滑缩放图像
    m_scaledImage = m_originalImage.scaled(
        newSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    qDebug() << "Scaled image created, size:" << m_scaledImage.size();

    // 4. 保存旧尺寸用于位置计算
    QSize oldSize = size();
    if(oldSize.isEmpty()) {
        oldSize = newSize;
        qDebug() << "First call, using newSize as oldSize:" << oldSize;
    }
    qDebug() << "Old widget size:" << oldSize;

    // 5. 更新窗口尺寸和形状
    setFixedSize(newSize);
    qDebug() << "Widget size set to:" << size();

    setCustomShape();
    qDebug() << "Custom shape set";

    // 6. 安全更新菜单按钮位置和大小
    if (menuButton) {
        // 检查按钮是否有效
        if (!menuButton->parent()) {
            qDebug() << "Menu button has no parent! Recreating...";
            menuButton->deleteLater();
            createMenuButton();
        } else {
            try {
                qDebug() << "Current button geometry:" << menuButton->geometry();

                // 计算大小变化比例（防止除零）
                double widthRatio = oldSize.width() > 0 ?
                                        static_cast<double>(menuButton->width()) / oldSize.width() : 0.15;
                double heightRatio = oldSize.height() > 0 ?
                                         static_cast<double>(menuButton->height()) / oldSize.height() : 0.15;

                // 计算新大小（保持最小20x20像素，最大不超过窗口1/3）
                int newButtonWidth = qBound(20,
                                            static_cast<int>(newSize.width() * widthRatio),
                                            newSize.width()/3);
                int newButtonHeight = qBound(20,
                                             static_cast<int>(newSize.height() * heightRatio),
                                             newSize.height()/3);

                // 计算位置比例（保持相对位置）
                double xRatio = oldSize.width() > 0 ?
                                    static_cast<double>(menuButton->x()) / oldSize.width() : 0.8;
                double yRatio = oldSize.height() > 0 ?
                                    static_cast<double>(menuButton->y()) / oldSize.height() : 0.8;

                // 计算新位置（确保在窗口内）
                int newX = qBound(0,
                                  static_cast<int>(newSize.width() * xRatio),
                                  newSize.width() - newButtonWidth);
                int newY = qBound(0,
                                  static_cast<int>(newSize.height() * yRatio),
                                  newSize.height() - newButtonHeight);

                // 应用变化
                menuButton->move(newX, newY);
                menuButton->setFixedSize(newButtonWidth, newButtonHeight);

                // 更新样式
                menuButton->setStyleSheet(QString(
                                              "QPushButton {"
                                              "   border: none;"
                                              "   background: rgba(255, 255, 255, 30%);"
                                              "   border-radius: %1px;"
                                              "}"
                                              "QPushButton:hover {"
                                              "   background: rgba(255, 255, 255, 50%);"
                                              "}").arg(newButtonWidth/2));

                qDebug() << "Button updated - new size:" << newButtonWidth << "x" << newButtonHeight
                         << ", new position:" << newX << "," << newY;
            } catch (...) {
                qDebug() << "Exception during button update! Recreating...";
                menuButton->deleteLater();
                createMenuButton();
            }
        }
    } else {
        qDebug() << "Menu button is null! Creating new one...";
        createMenuButton();
    }
    // 7. 触发重绘
    update();
    qDebug() << "=== Exit updateScaledImage() ===";
}

void PetWidget::wheelEvent(QWheelEvent *event)
{
    // 获取鼠标滚轮滚动角度
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numDegrees.isNull()) {
        // 计算缩放方向
        double zoom = 1.0 + (numDegrees.y() > 0 ? ZOOM_STEP : -ZOOM_STEP);

        // 限制缩放范围 (0.2 ~ 5.0倍)
        double newScale = qBound(0.2, m_scaleFactor * zoom, 5.0);

        if (newScale != m_scaleFactor) {
            m_scaleFactor = newScale;
            updateScaledImage();

            // 保持鼠标位置下的内容稳定
            if (m_scaleFactor > 1.0) {
                QPoint pos = event->position().toPoint();
                QPoint globalPos = mapToGlobal(pos);
                QPoint centerDiff = rect().center() - pos;
                move(globalPos - QPoint(width()/2, height()/2) + centerDiff);
            }
        }
    }
    event->accept();
}

void PetWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    // 窗口大小改变时更新形状
    setCustomShape();
}

//双击恢复原始界面
void PetWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        m_scaleFactor = 0.50; // 恢复原始大小的0.5，原定图片太大了
        updateScaledImage();
    }
}

void PetWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void PetWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}
PetWidget::~PetWidget() {
    if(menuButton) {
        menuButton->deleteLater();
        menuButton = nullptr;
    }
}
