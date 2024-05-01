#include <iostream>
#include "ui/mainwindow/mainwindow.h"
#include <QApplication>
#include <QDebug>
using std::cout, std::endl;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug()<<1111;
    MainWindow w;
    w.show();
    return a.exec();
    
    return 0;
}