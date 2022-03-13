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
    enum MsgType{
        MouseMovement,
        MouseInputBtn,
        Message,
        Change_Volume_Level
    };
    enum MouseMovementType{
        CursorMovement,
        ScrollMovement
    };
    enum MouseInputBtnType{
        MouseKeyUp = 0x00,
        MouseKeyDown = 0x01,
        MouseLeftClick = 0x02,
        MouseLeftTap = 0x04,
        MouseMiddleClick = 0x08,
        MouseRightClick = 0x10
    };
    enum KeyboardInputBtnType{
        KeyUp = 0x00,
        KeyDown = 0x01,
    };

private:
    QVector <QTcpSocket*> Sockets;
    QByteArray data;
    quint16 nextBlockSize;
    quint16 messageType;


    void VolumeLevelChange(QString event);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyToReadTcp();
    void slotReadyToReadUdp();
    void disconnectRecived();

signals:
    void sendMes(QString);
    void sendMouseMovement(MyServer::MouseMovementType, QString);
    void sendMouseBtnInput(MyServer::MouseInputBtnType);
};

#endif // MYSERVER_H
