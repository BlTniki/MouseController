#ifndef MYSERVER_H
#define MYSERVER_H

#include <QHostAddress>
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
    MyServer(QString IPadress, QString Port);
    ~MyServer();
    QTcpSocket *TCPsocket = nullptr;
    QUdpSocket *UDPsocket = nullptr;
    enum MsgType{
        MouseMovement,
        MouseInputBtn,
        KeyboardInputBtn,
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
    enum KeyboardInputBtnType{
        KeyboardKeyRelease = 0x00,
        KeyboardKeyPress = 0x01,
        KeyboardKeyUp = 0x02,
        KeyboardKeyDown = 0x04,
        KeyboardKeyLeft = 0x08,
        KeyboardKeyRight = 0x10,
        KeyboardKeyAlt = 0x20,
        KeyboardKeyWin = 0x40,
        KeyboardKeyCtrl = 0x80,
        KeyboardKeyShift = 0x100,
        KeyboardKeyCaps = 0x200,
        KeyboardKeyTab = 0x400,
        KeyboardKeyEsc = 0x800,
    };
    enum VolumeLevelChangeType{
        VolumeLevelUp = 0x02,
        VolumeLevelDown = 0x04,
        VolumeLevelMute = 0x08
    };

private:
    QHostAddress server_IPadress;
    quint16 server_Port;
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
    void sendKeyboardBtnInput(KeyboardInputBtnType);
};

#endif // MYSERVER_H
