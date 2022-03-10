#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QApplication>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QStyle>

#include "myserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;
    MyServer *ms;
    QSystemTrayIcon *trayIcon;

public slots:
    void reciveMes(QString str);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

};
#endif // MAINWINDOW_H
