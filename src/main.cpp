#include <iostream>
#include "process.h"
#include "ui/mainwindow/mainwindow.h"
#include <QApplication>
#include <QDebug>
using std::cout, std::endl;

int main(int argc, char *argv[])
{
    cout << "planing start" << endl;
    Process pro;
    pro.plan_process();
    cout << "planing end" << endl;
    QApplication a(argc, argv);
    qDebug()<<1111;
    MainWindow w;
    w.show();
    return a.exec();
    
    return 0;
}