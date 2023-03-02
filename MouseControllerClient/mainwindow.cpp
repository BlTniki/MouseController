#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //setup UI
    ui->setupUi(this);
    ui->InputWindow->setGeometry(0, 0, ui->InputWindow->geometry().width(), ui->InputWindow->geometry().height());
    ui->InputWindow->hide();
    ui->SettingsWindow->setGeometry(0, 0, ui->SettingsWindow->geometry().width(), ui->SettingsWindow->geometry().height());
    ui->SettingsWindow->hide();
    ui->KeyboardWindow->setGeometry(0, 0, ui->KeyboardWindow->geometry().width(), ui->KeyboardWindow->geometry().height());
    ui->KeyboardWindow->hide();

    //setup validators
    mouseSenseValidator = new QRegularExpressionValidator(QRegularExpression("[1-9]$"));
    ui->lE_MouseSense->setValidator(mouseSenseValidator);
    scrollSenseValidator = new QRegularExpressionValidator(QRegularExpression("[1-9]$"));
    ui->lE_ScrollSense->setValidator(scrollSenseValidator);
    ipAdressValidator = new QRegularExpressionValidator(QRegularExpression("([0-9]{1,3}[\\.]){3}[0-9]{1,3}"));
    ui->lE_IPadress->setValidator(ipAdressValidator);
    portValidator = new QRegularExpressionValidator(QRegularExpression("[1-9]+"));
    ui->lE_Port->setValidator(portValidator);

    sa = ui->senseArea;
    sb = ui->scrollBar;
    connect(sa, &SenseArea::TouchRecived, this, &MainWindow::TouchMove);
    connect(sa, &SenseArea::ClickRecived, this, &MainWindow::ClickDo);
    connect(sb, &ScrollBar::TouchRecived, this, &MainWindow::ScrollMove);

    //setup settings state
    settings = new SettingHandler();
    //recover mouse settings
    ui->lE_MouseSense->setText(
                QString::number(settings->getSavedMouseSense())
    );
    ui->lE_ScrollSense->setText(
                QString::number(settings->getSavedScrollSense())
    );
    //recover ip history
    FillHostAdress(settings->getLastHost());
    QVariantList hostList = settings->getSavedHosts();
    foreach (QVariant hostWithPort, hostList) {
        ui->cB_HostHistory->addItem(hostWithPort.toString());
    }
}

MainWindow::~MainWindow()
{
    //save mouse settings
    saveMouseSettings();

    //save Hosts
    saveHosts();

    delete ui;
}

void MainWindow::saveHosts()
{
    int comboBoxItemCount = ui->cB_HostHistory->count();
    QList<QString> hostList;
    for(int i = 0; i<comboBoxItemCount; i++) {
        QString hostWithPort = ui->cB_HostHistory->itemText(i);
        hostList.append(hostWithPort);
    }
    settings->saveHost(hostList);
    settings->setLastHost(QString(ui->lE_IPadress->text()
                                  +" "
                                  +ui->lE_Port->text()
                                  )
                          );
}

void MainWindow::saveMouseSettings()
{
    settings->saveMouseSense(mouseSense);
    settings->saveScrollSense(scrollSense);
}

void MainWindow::FillHostAdress(QString host)
{
    QString ip = "", port = "";
    for (int j=0; j<host.size(); j++) {
        if(host[j]==' '){
            ip = host.first(j);
            port = host.last(host.size()-j-1);
        }
    }
    ui->lE_IPadress->setText(ip);
    ui->lE_Port->setText(port);
}


/**
 ** Block of control communication to server
 **/


void MainWindow::connectResived()// Unblocking all objects, that communicate with the server
{
    ui->ConnectWindow->hide();
    ui->InputWindow->show();
}

void MainWindow::disconnectResived()// Blocking all objects, that communicate with the server
{
    ui->ConnectWindow->show();
    ui->InputWindow->hide();
    ui->SettingsWindow->hide();
    ui->KeyboardWindow->hide();

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

void MainWindow::SendToServerTCP(quint16 msgType, quint16 msg, QString str)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);

    //in this part we write block that contain message size. first we write a block whis 0, after message constructed, we rewrite this block
    out << quint16(0) << msgType << msg << str;
    out.device()->seek(0);
    out << quint16(data.size()-sizeof(quint16));
    qDebug() << "Send" << data.size()-sizeof(quint16) << msgType << msg << str;
    TCPsocket->write(data);
}

void MainWindow::SendToServerUDP(quint16 msgType, quint16 msg, QString str)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);

    //in this part we write block that contain message size. first we write a block whis 0, after message constructed, we rewrite this block
    out << quint16(0) << msgType << msg << str;
    out.device()->seek(0);
    out << quint16(data.size()-sizeof(quint16));

    UDPsocket->write(data);
}




/**
 ** Block of processing user input
 **/
void MainWindow::TouchMove(QTouchEvent *te)//sending a wish to change mouse position, pretty important thats we send a change touch position not just position
{
    if(te->isBeginEvent()){
        oldMouseX = te->points()[0].position().x();
        oldMouseY = te->points()[0].position().y();
        return;
    }

    int x = te->points()[0].position().x();
    int y = te->points()[0].position().y();
    int dx = x-oldMouseX, dy = y-oldMouseY;

    QString str = QString("%1x%2y").arg(dx*mouseSense).arg(dy*mouseSense);
    if(dx!=0||dy!=0){
        SendToServerUDP(MouseMovement, CursorMovement, str);
    }
    oldMouseX = x; oldMouseY = y;
}

void MainWindow::ClickDo()//send a click if tap in senseArea recived
{
    SendToServerTCP(MouseInputBtn, MouseLeftTap);
}

void MainWindow::ScrollMove(QTouchEvent *te)//sending a wish to turn the weel, pretty important thats we send a change touch position not just position
{
    if(te->isBeginEvent()){
        oldScrollY = te->points()[0].position().y();
        return;
    }

    int y = te->points()[0].position().y();
    int dy = y-oldScrollY;

    QString str = QString("%1y").arg(dy*scrollSense);
    qDebug() << dy << str;
    if(dy!=0){
        SendToServerUDP(MouseMovement, ScrollMovement, str);
    }
    oldScrollY = y;
}

/**
 * Buttons of ConnectWindow
 */
void MainWindow::on_pB_Connect_clicked() //After click on connect button we create new socket, connect signals and connect to host
{
    IPadress = ui->lE_IPadress->text();
    Port = ui->lE_Port->text().toUInt();

    //setup udp socket
    UDPsocket = new QUdpSocket(this);
    UDPsocket->connectToHost(IPadress, Port);
    connect(UDPsocket, &QUdpSocket::disconnected, UDPsocket, &QUdpSocket::deleteLater);

    //setup tcp socket
    TCPsocket = new QTcpSocket(this);
    connect(TCPsocket, &QTcpSocket::connected, this, &MainWindow::connectResived);
    connect(TCPsocket, &QTcpSocket::disconnected, TCPsocket, &QTcpSocket::deleteLater);
    connect(TCPsocket, &QTcpSocket::disconnected, this, &MainWindow::disconnectResived);

    TCPsocket->connectToHost(IPadress, Port);
}

void MainWindow::on_pB_Remeber_clicked()
{
    int comboBoxItemCount = ui->cB_HostHistory->count();
    QString ip = ui->lE_IPadress->text(), port = ui->lE_Port->text();
    ui->cB_HostHistory->addItem(QString(ip+" "+port), comboBoxItemCount);
    saveHosts();
}

void MainWindow::on_pB_Forget_clicked()
{
    ui->cB_HostHistory->removeItem(ui->cB_HostHistory->currentIndex());
}

void MainWindow::on_pB_Fill_clicked()
{
    FillHostAdress(ui->cB_HostHistory->currentText());
}


/**
 * Buttons of InputWindow
 */
void MainWindow::on_pB_Send_clicked()
{
    QString str = QString("%2").arg(ui->lE_ToSend->text());
    SendToServerTCP(Message, quint16(0), str);
}
void MainWindow::on_pB_LeftClick_pressed()
{
    qDebug() << MouseInputBtn << (MouseLeftClick|MouseKeyDown);
    SendToServerTCP(MouseInputBtn, MouseLeftClick|MouseKeyDown);
}


void MainWindow::on_pB_LeftClick_released()
{
    SendToServerTCP(MouseInputBtn, MouseLeftClick);
}

void MainWindow::on_pB_MiddleClick_pressed()
{
    SendToServerTCP(MouseInputBtn, MouseMiddleClick|MouseKeyDown);
}


void MainWindow::on_pB_MiddleClick_released()
{
    SendToServerTCP(MouseInputBtn, MouseMiddleClick);
}


void MainWindow::on_pB_RightClick_pressed()
{
    SendToServerTCP(MouseInputBtn, MouseRightClick|MouseKeyDown);
}


void MainWindow::on_pB_RightClick_released()
{
    SendToServerTCP(MouseInputBtn, MouseRightClick);
}



/**
 * Buttons of SettingsWindow
 */
void MainWindow::on_pB_Settings_clicked()
{
    ui->InputWindow->hide();
    ui->SettingsWindow->show();
}


void MainWindow::on_pB_SettingsBack_clicked()
{
    ui->SettingsWindow->hide();
    ui->InputWindow->show();
}

void MainWindow::on_lE_MouseSense_textChanged(const QString &arg1)
{
    mouseSense = arg1.toInt();
    saveMouseSettings();
}


void MainWindow::on_pB_MouseSensePlus_clicked()
{
    if(ui->lE_MouseSense->text().toInt()+1<=9)
        ui->lE_MouseSense->setText(QString::number(ui->lE_MouseSense->text().toInt()+1));
}


void MainWindow::on_pB_MouseSenseMinus_clicked()
{
    if(ui->lE_MouseSense->text().toInt()-1>=1)
        ui->lE_MouseSense->setText(QString::number(ui->lE_MouseSense->text().toInt()-1));
}


void MainWindow::on_lE_ScrollSense_textChanged(const QString &arg1)
{
    scrollSense = arg1.toInt();
    saveMouseSettings();
}


void MainWindow::on_pB_ScrollSense_clicked()
{
    if(ui->lE_ScrollSense->text().toInt()+1<=9)
        ui->lE_ScrollSense->setText(QString::number(ui->lE_ScrollSense->text().toInt()+1));
}


void MainWindow::on_pB_ScrollSense_2_clicked()
{
    if(ui->lE_ScrollSense->text().toInt()-1>=1)
        ui->lE_ScrollSense->setText(QString::number(ui->lE_ScrollSense->text().toInt()-1));
}

/**
 * Buttons of KeyboardWindow
 */

void MainWindow::on_pB_Keyboard_clicked()
{
    ui->InputWindow->hide();
    ui->KeyboardWindow->show();
}

void MainWindow::on_pB_KeyboardBack_clicked()
{
    ui->InputWindow->show();
    ui->KeyboardWindow->hide();
}

void MainWindow::on_pB_VolumePlus_clicked()
{
    SendToServerTCP(ChangeVolumeLevel, VolumeLevelUp);
}


void MainWindow::on_pB_VolumeMinus_clicked()
{
    SendToServerTCP(ChangeVolumeLevel, VolumeLevelDown);
}


void MainWindow::on_pB_VolumeMute_clicked()
{
    SendToServerTCP(ChangeVolumeLevel, VolumeLevelMute);
}

void MainWindow::on_pB_KeyEsc_pressed()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyEsc|KeyboardKeyPress);
}


void MainWindow::on_pB_KeyEsc_released()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyEsc|KeyboardKeyRelease);
}


void MainWindow::on_pB_KeyTab_pressed()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyTab|KeyboardKeyPress);
}


void MainWindow::on_pB_KeyTab_released()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyTab|KeyboardKeyRelease);
}


void MainWindow::on_pB_KeyCaps_pressed()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyCaps|KeyboardKeyPress);
}


void MainWindow::on_pB_KeyCaps_released()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyCaps|KeyboardKeyRelease);
}

void MainWindow::on_pB_KeyShift_toggled(bool checked)
{
    if(checked){
        SendToServerTCP(KeyboardInputBtn, KeyboardKeyShift|KeyboardKeyPress);
    }
    else{
        SendToServerTCP(KeyboardInputBtn, KeyboardKeyShift|KeyboardKeyRelease);
    }
}


void MainWindow::on_pB_KeyCtrl_toggled(bool checked)
{
    if(checked){
        SendToServerTCP(KeyboardInputBtn, KeyboardKeyCtrl|KeyboardKeyPress);
    }
    else{
        SendToServerTCP(KeyboardInputBtn, KeyboardKeyCtrl|KeyboardKeyRelease);
    }
}

void MainWindow::on_pB_KeyWin_pressed()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyWin|KeyboardKeyPress);
}


void MainWindow::on_pB_KeyWin_released()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyWin|KeyboardKeyRelease);
}

void MainWindow::on_pB_KeyAlt_toggled(bool checked)
{
    if(checked){
        SendToServerTCP(KeyboardInputBtn, KeyboardKeyAlt|KeyboardKeyPress);
    }
    else{
        SendToServerTCP(KeyboardInputBtn, KeyboardKeyAlt|KeyboardKeyRelease);
    }
}

void MainWindow::on_pB_KeyUp_pressed()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyUp|KeyboardKeyPress);
}


void MainWindow::on_pB_KeyUp_released()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyUp|KeyboardKeyRelease);
}


void MainWindow::on_pB_KeyLeft_pressed()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyLeft|KeyboardKeyPress);
}


void MainWindow::on_pB_KeyLeft_released()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyLeft|KeyboardKeyRelease);
}


void MainWindow::on_pB_KeyDown_pressed()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyDown|KeyboardKeyPress);
}


void MainWindow::on_pB_KeyDown_released()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyDown|KeyboardKeyRelease);
}


void MainWindow::on_pB_KeyRight_pressed()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyRight|KeyboardKeyPress);
}


void MainWindow::on_pB_KeyRight_released()
{
    SendToServerTCP(KeyboardInputBtn, KeyboardKeyRight|KeyboardKeyRelease);
}
