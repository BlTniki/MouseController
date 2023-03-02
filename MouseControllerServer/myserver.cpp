#include "myserver.h"

MyServer::MyServer(QString IPadress, QString Port)
{
    server_IPadress = QHostAddress(IPadress);
    server_Port = Port.toUInt();
    if(this->listen(server_IPadress, server_Port)){
        qDebug() << "start";
    }
    else{
        qDebug() << "error";
    }
    UDPsocket = new QUdpSocket;
    UDPsocket->bind(server_IPadress, server_Port);
    connect(UDPsocket, &QUdpSocket::readyRead, this, &MyServer::slotReadyToReadUdp);
    connect(UDPsocket, &QUdpSocket::disconnected, UDPsocket, &QUdpSocket::deleteLater);

    nextTcpBlockSize = 0; nextUdpBlockSize = 0;
}

MyServer::~MyServer()
{
    qDebug() << "im closed(";

    if(TCPsocket){
        TCPsocket->disconnect();
        delete TCPsocket;
        TCPsocket = nullptr;
    }

    if(UDPsocket){
        UDPsocket->disconnect();
        delete UDPsocket;
        UDPsocket = nullptr;
    }
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
    qDebug() << "TCP";
    emit sendMes("Has some TCP data");

    TCPsocket = (QTcpSocket*)sender();
    quint64 pendingDataSize = TCPsocket->size();
    QByteArray header;
    const quint64 headerSize = 4; //amount of header bytes

    do{
        if(pendingDataSize<2){
            emit sendMes("Data < 2. return");
            return;
        }

        if(nextTcpBlockSize == 0){
            QDataStream blockSize(TCPsocket->read(2));
            blockSize.setVersion(QDataStream::Qt_6_2);
            if(blockSize.status() == QDataStream::Ok){
                blockSize >> nextTcpBlockSize;
                if(nextTcpBlockSize > pendingDataSize){
                    qDebug() << "Data not full. return";
                    return;
                }
            }else{
                qDebug() << "blockSize: Has problem with reading. Abort";
                TCPsocket->readAll(); //cleaning a buffer
                return;
            }
        }

        header = TCPsocket->read(headerSize);
        QDataStream in(header);
        in.setVersion(QDataStream::Qt_6_2);

        qDebug() << "incoming data size =" << TCPsocket->size() << "incoming block size =" << nextTcpBlockSize;
        qDebug() << header;

        if(in.status() == QDataStream::Ok){
            emit sendMes("reading...");
            in >> message;
            switch (message) {
            case (Message):{
                in >> messageDetail;
                quint64 dataSize = messageDetail;
                QByteArray data = TCPsocket->read(dataSize);
                QString str(data);
                emit sendMes(str);
                emit sendStringPast(str);
                break;
            }
            case (MouseInputBtn):{
                quint16 msgType;
                in >> msgType;
                emit sendMouseBtnInput(static_cast<MouseInputBtnType>(msgType));
                break;
            }
            case (KeyboardInputBtn):{
                quint16 msgType;
                in >> msgType;
                emit sendKeyboardBtnInput(static_cast<KeyboardInputBtnType>(msgType));
                break;
            }
            case (ChangeVolumeLevel):{
                quint16 msgType;
                in >> msgType;
                emit sendVolumeLevelChanges(static_cast<VolumeLevelChangeType>(msgType));
                break;
            }
            default:
                //emit sendMes("Read TCP ERROR");
                break;
            }

        }
        else{
            qDebug() << "in: Has problem with reading. Abort";
            TCPsocket->readAll();
            return;
        }

        nextTcpBlockSize = 0;
        pendingDataSize = TCPsocket->size();
    }while (pendingDataSize != 0);
}

void MyServer::slotReadyToReadUdp()
{
//    int c = 0;
//    UDPsocket = (QUdpSocket*)sender();
//    qDebug() << "UDP";
//    while (UDPsocket->hasPendingDatagrams()) {
//        nextBlockSize = 0;
//        QByteArray q;
//        qDebug() << UDPsocket->size();
//        q = UDPsocket->receiveDatagram().data();
//        qDebug() << q << c++;

//        QDataStream in(q);

//        in.setVersion(QDataStream::Qt_6_2);
//        QString str;

//        if(in.status() == QDataStream::Ok){
//            emit sendMes("reading...");
//            if(nextBlockSize == 0){
//                if(q.size() < 2){
//                    emit sendMes("Data < 2. error");
//                    break;
//                }
//                in >> nextBlockSize;
//                qDebug() << "nextBlockSize ==" << nextBlockSize;
//                if(q.size() < nextBlockSize){
//                    emit sendMes("Data not full, break");
//                    break;
//                }

//                in >> messageType;
//                nextBlockSize = 0;

//                switch (messageType) {
//                case (MouseMovement):{
//                    quint16 movementType;
//                    in >> movementType;
//                    str = "";
//                    in >> str;
//                    emit sendMouseMovement(static_cast<MouseMovementType>(movementType), str);
//                    break;
//                }

//                default:
//                    emit sendMes("Read UDP ERROR");
//                    break;
//                }
//            }
//        }
//        else{
//            emit sendMes("DataStream error");
//            return;
//        }
//    }
}

void MyServer::disconnectRecived()
{
    emit sendMes("Someone disconnected");

    Sockets.removeOne((QTcpSocket*)sender());
}
