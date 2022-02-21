#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Before connection blocking all objects, that communicate with the server
    ui->setupUi(this);
    ui->pB_Send->setEnabled(false);
    ui->senseArea->blockSignals(true);
    ui->pB_LeftClick->setEnabled(true);
    ui->pB_MiddleClick->setEnabled(true);
    ui->pB_RightClick->setEnabled(true);
    ui->pB_LeftClick->setEnabled(false);
    ui->pB_MiddleClick->setEnabled(false);
    ui->pB_RightClick->setEnabled(false);

    sa = ui->senseArea;
    sb = ui->scrollBar;

    connect(sa, &SenseArea::TouchRecived, this, &MainWindow::TouchMove);
    connect(sa, &SenseArea::ClickRecived, this, &MainWindow::ClickDo);
    connect(sb, &ScrollBar::TouchRecived, this, &MainWindow::ScrollMove);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pB_Send_clicked()
{
    QString str = QString("%2").arg(ui->lE_ToSend->text());
    SendToServer(Message, str);
}



/**
 ** Block of control communication to server
 **/
void MainWindow::on_pB_Connect_clicked() //After click on connect button we create new socket, connect signals and connect to host
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::connectResived);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::disconnectResived);

    socket->connectToHost("192.168.31.193", 2323);
}

void MainWindow::connectResived()// Unblocking all objects, that communicate with the server
{
    ui->senseArea->blockSignals(false);
    ui->scrollBar->blockSignals(false);

    ui->pB_Send->setEnabled(true);
    ui->pB_LeftClick->setEnabled(true);
    ui->pB_MiddleClick->setEnabled(true);
    ui->pB_RightClick->setEnabled(true);
}

void MainWindow::disconnectResived()// Blocking all objects, that communicate with the server
{
    ui->senseArea->blockSignals(true);
    ui->scrollBar->blockSignals(true);

    ui->pB_Send->setEnabled(false);
    ui->pB_LeftClick->setEnabled(false);
    ui->pB_MiddleClick->setEnabled(false);
    ui->pB_RightClick->setEnabled(false);
}

void MainWindow::SendToServer(MsgType type, QString str)// Sending to server str and MsgType
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);

    //in this part we write block that contain message size. first we write a block whis 0, after message constructed, we rewrite this block
    out << quint16(0) << quint16(type) << str;
    out.device()->seek(0);
    out << quint16(data.size()-sizeof(quint16));

    socket->write(data);
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

    QString str = QString("%1x%2y").arg(dx).arg(dy);
    if(dx!=0||dy!=0){
        SendToServer(Mouse_pos, str);
    }
    oldMouseX = x; oldMouseY = y;
}

void MainWindow::ClickDo(QString event)//send a click if tap in senseArea recived
{
    SendToServer(Mouse_Left_btn, event);
}

void MainWindow::ScrollMove(QTouchEvent *te)//sending a wish to turn the weel, pretty important thats we send a change touch position not just position
{
    if(te->isBeginEvent()){
        oldScrollY = te->points()[0].position().y();
        return;
    }

    int y = te->points()[0].position().y();
    int dy = y-oldScrollY;

    QString str = QString("%1y").arg(dy);
    qDebug() << dy << str;
    if(dy!=0){
        SendToServer(Scroll_move, str);
    }
    oldScrollY = y;
}

/*
 * block of processing mouse button input
*/
void MainWindow::on_pB_LeftClick_pressed()
{
    SendToServer(Mouse_Left_btn, "d");
}


void MainWindow::on_pB_LeftClick_released()
{
    SendToServer(Mouse_Left_btn, "u");
}

void MainWindow::on_pB_MiddleClick_pressed()
{
    SendToServer(Mouse_Middle_btn, "d");
}


void MainWindow::on_pB_MiddleClick_released()
{
    SendToServer(Mouse_Middle_btn, "u");
}


void MainWindow::on_pB_RightClick_pressed()
{
    SendToServer(Mouse_Right_btn, "d");
}


void MainWindow::on_pB_RightClick_released()
{
    SendToServer(Mouse_Right_btn, "u");
}
