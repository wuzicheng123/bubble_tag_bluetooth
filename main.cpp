#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/f2pv3-g40d7-001.png"));
    MainWindow w;
    w.show();

    return a.exec();
}
