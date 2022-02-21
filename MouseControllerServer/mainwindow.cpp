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

    connect(ms, &MyServer::sendMes, this, &MainWindow::reciveMes);

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
