#include "myserver.h"

MyServer::MyServer()
{
    if(this->listen(QHostAddress::Any, 2323)){
        qDebug() << "start";
    }
    else{
        qDebug() << "error";
    }

    UDPsocket = new QUdpSocket;
    UDPsocket->bind(QHostAddress::Any, 2323);
    connect(UDPsocket, &QUdpSocket::readyRead, this, &MyServer::slotReadyToReadUdp);
    connect(UDPsocket, &QUdpSocket::disconnected, UDPsocket, &QUdpSocket::deleteLater);
}

/**
 * Block of connection handle
 */
void MyServer::incomingConnection(qintptr socketDescriptor)
{
    TCPsocket = new QTcpSocket;
    TCPsocket->setSocketDescriptor(socketDescriptor);
    connect(TCPsocket, &QTcpSocket::readyRead, this, &MyServer::slotReadyToReadTcp);
    connect(TCPsocket, &QTcpSocket::disconnected, TCPsocket, &QTcpSocket::deleteLater);
    connect(TCPsocket, &QTcpSocket::disconnected, this, &MyServer::disconnectRecived);

    Sockets.push_back(TCPsocket);
    emit sendMes(QString("client connected %1").arg(socketDescriptor));
}

void MyServer::slotReadyToReadTcp()
{
    nextBlockSize = 0;
    TCPsocket = (QTcpSocket*)sender();
    qDebug() << "TCP";
    QByteArray q;
    qDebug() << TCPsocket->size();
    q = TCPsocket->readAll();
    qDebug() << q;

    QDataStream in(q);

    in.setVersion(QDataStream::Qt_6_2);


    if(in.status() == QDataStream::Ok){
        emit sendMes("reading...");

        //this cycle for reading all data that recived
        while(!in.atEnd()){
                if(nextBlockSize == 0){
                    if(q.size() < 2){
                        emit sendMes("Data < 2. error");
                        break;
                    }
                    in >> nextBlockSize;
                    qDebug() << "nextBlockSize ==" << nextBlockSize;
                    if(q.size() < nextBlockSize){
                        emit sendMes("Data not full, break");
                        break;
                    }

                    in >> messageType;
                    nextBlockSize = 0;

                    switch (messageType) {
                    case (Message):{
                        quint16 shift;
                        in >> shift;
                        QString str = "";
                        in >> str;
                        emit sendMes(str);
                        break;
                    }
                    case (MouseInputBtn):{
                        quint16 msgType;
                        in >> msgType;
                        emit sendMouseBtnInput(static_cast<MouseInputBtnType>(msgType));
                        break;
                    }
                    case (Change_Volume_Level):{
                        quint16 shift;
                        in >> shift;
                        QString str = "";
                        in >> str;
                        VolumeLevelChange(str);
                        emit sendMes("Change volume level by"+str);
                        break;
                    }
                    default:
                        //emit sendMes("Read TCP ERROR");
                        break;
                    }
                }

        }
    }
    else{
        emit sendMes("DataStream error");
        return;
    }

}

void MyServer::slotReadyToReadUdp()
{
    int c = 0;
    UDPsocket = (QUdpSocket*)sender();
    qDebug() << "UDP";
    while (UDPsocket->hasPendingDatagrams()) {
        nextBlockSize = 0;
        QByteArray q;
        qDebug() << UDPsocket->size();
        q = UDPsocket->receiveDatagram().data();
        qDebug() << q << c++;

        QDataStream in(q);

        in.setVersion(QDataStream::Qt_6_2);
        QString str;

        if(in.status() == QDataStream::Ok){
            emit sendMes("reading...");
            if(nextBlockSize == 0){
                if(q.size() < 2){
                    emit sendMes("Data < 2. error");
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize ==" << nextBlockSize;
                if(q.size() < nextBlockSize){
                    emit sendMes("Data not full, break");
                    break;
                }

                in >> messageType;
                nextBlockSize = 0;

                switch (messageType) {
                case (MouseMovement):{
                    quint16 movementType;
                    in >> movementType;
                    str = "";
                    in >> str;
                    emit sendMouseMovement(static_cast<MouseMovementType>(movementType), str);
                    break;
                }

                default:
                    emit sendMes("Read UDP ERROR");
                    break;
                }
            }
        }
        else{
            emit sendMes("DataStream error");
            return;
        }
    }
}

void MyServer::disconnectRecived()
{
    emit sendMes("Someone disconnected");

    Sockets.removeOne((QTcpSocket*)sender());
}


/**
 * Block of input controll
 */


void MyServer::VolumeLevelChange(QString event)
{
    if(event == "+"){
        //press and release
        keybd_event( VK_VOLUME_UP, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event( VK_VOLUME_UP, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
    else if(event == "-"){
        keybd_event( VK_VOLUME_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event( VK_VOLUME_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
    else{
        keybd_event( VK_VOLUME_MUTE, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event( VK_VOLUME_MUTE, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
}
