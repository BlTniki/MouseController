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
        ChangeVolumeLevel
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
    enum VolumeLevelChangeType{
        VolumeLevelUp = 0x02,
        VolumeLevelDown = 0x04,
        VolumeLevelMute = 0x08
    };

private:
    QVector <QTcpSocket*> Sockets;
    QByteArray data;
    quint16 nextBlockSize;
    quint16 messageType;

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyToReadTcp();
    void slotReadyToReadUdp();
    void disconnectRecived();

signals:
    void sendMes(QString);
    void sendStringPast(QString);
    void sendVolumeLevelChanges(VolumeLevelChangeType);
    void sendMouseMovement(MyServer::MouseMovementType, QString);
    void sendMouseBtnInput(MyServer::MouseInputBtnType);
};

#endif // MYSERVER_H
