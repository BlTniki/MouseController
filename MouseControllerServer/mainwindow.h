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
    enum MouseMovementType{
        CursorMovement,
        ScrollMovement
    };
    enum MouseInputBtnType{
        KeyUp = 0x00,
        KeyDown = 0x01,
        MouseLeftClick = 0x02,
        MouseLeftDClick = 0x04,
        MouseMiddleClick = 0x08,
        MouseRightClick = 0x10
    };
    void CursorMove(QString str);
    void ScrollMove(QString str);

public slots:
    void reciveMes(QString str);
    void reciveMouseMovement(MouseMovementType msgType, QString str);
    void reciveMouseBtnInput(MouseInputBtnType msgType);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

};
#endif // MAINWINDOW_H
