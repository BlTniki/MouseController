#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDebug>
#include <QVector>
#include <QIODevice>
#include <windows.h>

class MyServer : public QTcpServer
{
    Q_OBJECT;
public:
    MyServer();
    QTcpSocket *TCPsocket;
    QUdpSocket *UDPsocket;

private:
    QVector <QTcpSocket*> Sockets;
    QByteArray data;
    quint16 nextBlockSize;
    quint16 messageType;
    enum MsgType{
        Mouse_pos = 1,
        Message = 2,
        Mouse_Left_btn = 3,
        Mouse_Middle_btn = 4,
        Mouse_Right_btn = 5,
        Scroll_move = 6
    };
    void MouseMove(QString str);
    void MouseLeftClick(QString event);
    void MouseMiddleClick(QString event);
    void MouseRightClick(QString event);
    void ScrollMove(QString str);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyToReadTcp();
    void slotReadyToReadUdp();
    void disconnectRecived();

signals:
    void sendMes(QString);
};

#endif // MYSERVER_H
