#include "mainwindow.h"
#include "luahelper.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LuaHelper *lua = new LuaHelper ;

    MainWindow w;
    w.show();
    return a.exec();
}
