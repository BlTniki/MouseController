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
    QString str;

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
                    str = "";
                    in >> str;
                    emit sendMes(QString("%1 %2").arg(str).arg(messageType));
                    qDebug() << str << messageType;
                    nextBlockSize = 0;
                    switch (messageType) {
                    case (Mouse_pos):
                        MouseMove(str);
                        break;
                    case (Mouse_Left_btn):
                        MouseLeftClick(str);
                        break;
                    case (Mouse_Middle_btn):
                        MouseMiddleClick(str);
                        break;
                    case (Mouse_Right_btn):
                        MouseRightClick(str);
                        break;
                    case (Scroll_move):
                        ScrollMove(str);
                        break;
                    default:
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
                str = "";
                in >> str;
                emit sendMes(QString("%1 %2").arg(str).arg(messageType));
                qDebug() << str << messageType;
                nextBlockSize = 0;
                switch (messageType) {
                case (Mouse_pos):
                    MouseMove(str);
                    break;
                case (Mouse_Left_btn):
                    MouseLeftClick(str);
                    break;
                case (Mouse_Middle_btn):
                    MouseMiddleClick(str);
                    break;
                case (Mouse_Right_btn):
                    MouseRightClick(str);
                    break;
                case (Scroll_move):
                    ScrollMove(str);
                    break;
                case (Change_Volume_Level):
                    VolumeLevelChange(str);
                    break;
                default:
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
void MyServer::MouseMove(QString str)
{
    int dx=0,dy=0;
    for(int i=0, pos = 0; i<str.size(); i++){
        if(str[i] == 'x'){
            dx = str.mid(pos,i-pos).toInt();
            pos = i+1;
        }
        if(str[i] == 'y'){
            dy = str.mid(pos, i-pos).toInt();
        }
    }
    emit sendMes(QString("x %1 y %2").arg(dx).arg(dy));

    POINT p;
    GetCursorPos(&p);
    SetCursorPos(p.x+dx, p.y+dy);
}

void MyServer::ScrollMove(QString str)
{
    int dy=0;
    for(int i=0, pos = 0; i<str.size(); i++){
        if(str[i] == 'y'){
            dy = str.mid(pos,i-pos).toInt();
            pos = i+1;
        }
    }
    emit sendMes(QString("y %1").arg(dy));

    mouse_event (MOUSEEVENTF_WHEEL, 0, 0, (-1)*dy*10, 0);
}

void MyServer::MouseLeftClick(QString event)
{
    for(int i=0; i<event.size(); i++){
        if (event[i] == 'd'){
            emit sendMes("LeftBTN Down");
            mouse_event (MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        }
        else{
            emit sendMes("LeftBTN Up");
            mouse_event (MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
    }

}

void MyServer::MouseMiddleClick(QString event)
{
    if (event == "d"){
        emit sendMes("MiddleBTN Down");
        mouse_event (MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
    }
    else{
        emit sendMes("MiddleBTN Up");
        mouse_event (MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
    }
}

void MyServer::MouseRightClick(QString event)
{
    if (event == "d"){
        emit sendMes("RightBTN Down");
        mouse_event (MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
    }
    else{
        emit sendMes("RightBTN Up");
        mouse_event (MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
    }
}

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
