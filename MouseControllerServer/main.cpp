#include "mainwindow.h"
#include "myserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    FreeConsole();
    if(!w.AutoRun)
        w.show();
    return a.exec();
}
