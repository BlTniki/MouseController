#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("MouseController");
    ms = new MyServer;

    //connect signals between MainWindow and Server
    connect(ms, &MyServer::sendMes, this, &MainWindow::reciveMes);
    connect(ms, &MyServer::sendMouseMovement , this, &MainWindow::reciveMouseMovement);
    connect(ms, &MyServer::sendMouseBtnInput , this, &MainWindow::reciveMouseBtnInput);

    /* Initialize the tray icon, set the icon from the system icon set,
     * and also set a tooltip
     * */
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icon/icon.ico"));
    trayIcon->setToolTip("MouseController");
    /* Then we create a context menu of two items*/
    QMenu * menu = new QMenu(this);
    QAction * viewWindow = new QAction("Развернуть окно", this);
    QAction * quitAction = new QAction("Выход", this);

    /* connect click signals on menu items to the corresponding slots.
     * The first menu item expands the application from the tray,
     * and the second menu item terminates the application
     * */
    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    menu->addAction(viewWindow);
    menu->addAction(quitAction);

    /* Set the context menu to the icon
     * and show the application tray icon
     * */
    trayIcon->setContextMenu(menu);
    trayIcon->show();

    /* We also connect the icon click signal to the handler
     * given pressing
     * */
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


/* Method that handles the application window close event
 * */
void MainWindow::closeEvent(QCloseEvent * event)
{
    /* If the window is visible, then exit the application
     * is ignored and the window is simply hidden, which is followed by
     * relevant popup message
     */
    if(this->isVisible()){
        event->ignore();
        this->hide();

    }
}

/* Method that handles clicking on the application tray icon
 * */
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
        /* otherwise, if the window is visible, then it is hidden,
         * and vice versa, if hidden, it expands to the screen
         * */
        if(!this->isVisible()){
            this->show();
        } else {
            this->hide();
        }
        break;
    default:
        break;
    }
}

void MainWindow::reciveMes(QString str)
{
    ui->textBrowser->append(str);
}


/**
 * Block of input controll
 */
void MainWindow::reciveMouseMovement(MyServer::MouseMovementType msgType, QString str)
{
    switch (msgType) {
    case (MyServer::CursorMovement):
        CursorMove(str);
        break;
    case (MyServer::ScrollMovement):
        ScrollMove(str);
        break;
    default:
        reciveMes("MouseMovement ERROR");
    }
}

void MainWindow::CursorMove(QString str)
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
    reciveMes(QString("x %1 y %2").arg(dx).arg(dy));

    POINT p;
    GetCursorPos(&p);
    SetCursorPos(p.x+dx, p.y+dy);
}

void MainWindow::ScrollMove(QString str)
{
    int dy=0;
    for(int i=0, pos = 0; i<str.size(); i++){
        if(str[i] == 'y'){
            dy = str.mid(pos,i-pos).toInt();
            pos = i+1;
        }
    }
    reciveMes(QString("y %1").arg(dy));

    mouse_event (MOUSEEVENTF_WHEEL, 0, 0, (-1)*dy*10, 0);
}

void MainWindow::reciveMouseBtnInput(MyServer::MouseInputBtnType msgType)
{
    // ~(~msgType|KeyDown) так как в последем бите хранится указание, что именно делать с кнопкой, на этом этапе мы её игнорируем
    switch (~(~msgType|MyServer::MouseKeyDown)) {
    case (MyServer::MouseLeftClick):{
        if (msgType&MyServer::MouseKeyDown){
            reciveMes("LeftBTN Down");
            mouse_event (MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        }
        else{
            reciveMes("LeftBTN Up");
            mouse_event (MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
        break;
    }
    case (MyServer::MouseLeftTap):{
            reciveMes("LeftBTN Double Tap");
            mouse_event (MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event (MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

            break;
        }
    case (MyServer::MouseMiddleClick):{
        if (msgType&MyServer::MouseKeyDown){
            reciveMes("MiddleBTN Down");
            mouse_event (MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
        }
        else{
            reciveMes("MiddleBTN Up");
            mouse_event (MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
        }
        break;
    }
    case (MyServer::MouseRightClick):{
        if (msgType&MyServer::MouseKeyDown){
            reciveMes("RightBTN Down");
            mouse_event (MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
        }
        else{
            reciveMes("RightBTN Up");
            mouse_event (MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
        }
        break;
    }
    default:
        reciveMes("MouseBtnInput ERROR");
    }
}
