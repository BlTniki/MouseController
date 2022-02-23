#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTouchEvent>

#include "sensearea.h"
#include "scrollbar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SenseArea *sa;
    ScrollBar *sb;
    QTcpSocket *TCPsocket;
    QUdpSocket *UDPsocket;
    QByteArray data;
    quint16 nextBlockSize;
    quint16 messageType;
    int oldMouseX = 0, oldMouseY = 0;
    int oldScrollY;
    enum MsgType{
        Mouse_pos = 1,
        Message = 2,
        Mouse_Left_btn = 3,
        Mouse_Middle_btn = 4,
        Mouse_Right_btn = 5,
        Scroll_move = 6
    };
    void SendToServer(MsgType type, QString str="");

protected:

public slots:
    void TouchMove(QTouchEvent *te);
    void ClickDo(QString event);
    void ScrollMove(QTouchEvent *te);
    void connectResived();
    void disconnectResived();

private slots:
    void on_pB_Send_clicked();

    void on_pB_Connect_clicked();

    void on_pB_LeftClick_pressed();

    void on_pB_LeftClick_released();

    void on_pB_MiddleClick_pressed();

    void on_pB_MiddleClick_released();

    void on_pB_RightClick_pressed();

    void on_pB_RightClick_released();


    void on_pB_Settings_clicked();
    void on_pB_SettingsBack_clicked();
};
#endif // MAINWINDOW_H
