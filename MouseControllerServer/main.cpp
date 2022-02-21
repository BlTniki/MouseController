#include "mainwindow.h"
#include "myserver.h"

#include <QApplication>
//#include <windows.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    FreeConsole();
    //w.show();
    return a.exec();
}
