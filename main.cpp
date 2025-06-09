#include "petwidget.h"

#include <QApplication>

//初始化QApplication和主窗口
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PetWidget w;
    w.show();
    return a.exec();
}
