#include "timezoneswicher.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //TimeZoneSwicher w;
    //w.resize(400,300);
    MainWindow w ;

    w.show();
    return a.exec();
}
