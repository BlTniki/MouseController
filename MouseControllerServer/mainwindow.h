#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QApplication>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QStyle>
#include <windows.h>

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

    void CursorMove(QString str);
    void ScrollMove(QString str);

public slots:
    void reciveMes(QString str);
    void reciveStringPast(QString str);
    void reciveVolumeLevelChanges(MyServer::VolumeLevelChangeType msgType);
    void reciveMouseMovement(MyServer::MouseMovementType msgType, QString str);
    void reciveMouseBtnInput(MyServer::MouseInputBtnType msgType);
    void reciveKeyboardBtnInput(MyServer::KeyboardInputBtnType msgType);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

};
#endif // MAINWINDOW_H
