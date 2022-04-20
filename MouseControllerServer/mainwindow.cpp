#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("MouseController");

    //setup validators
    ipAdressValidator = new QRegularExpressionValidator(QRegularExpression("([0-9]{1,3}[\\.]){3}[0-9]{1,3}"));
    ui->lE_IPadress->setValidator(ipAdressValidator);
    portValidator = new QRegularExpressionValidator(QRegularExpression("[1-9]+"));
    ui->lE_Port->setValidator(portValidator);

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

    //setup settings state
    SettingsSaves.setFileName("SettingsSaves");
    if(SettingsSaves.exists()){
        QString buf;
        SettingsSaves.open(QIODevice::ReadOnly | QIODevice::Text);
        buf = SettingsSaves.readAll();
        SettingsSaves.close();
        SettingsStates = QJsonDocument::fromJson(buf.toUtf8()).object();

        //recover host history
        int ipCount = SettingsStates["HostHistory"].toInt();
        for (int i=0; i<ipCount; i++) {
            QString host = SettingsStates[QString("ip%1").arg(i)].toString();
            ui->cB_HostHistory->addItem(QString(host), i);
        }
        FillHostAdress(SettingsStates["lastUsedIp"].toString());
        ui->cB_AutoRun->setChecked(SettingsStates["AutoRun"].toBool());
    }
    if(ui->cB_AutoRun->isChecked()){
        AutoRun = true;
        on_pB_Start_clicked();
    }
}

MainWindow::~MainWindow()
{
    SettingsStates = QJsonObject({{"AutoRun",false},
                                  {"HostHistory",0}
                                 });

    SettingsStates["AutoRun"] = ui->cB_AutoRun->isChecked();

    int comboBoxItemCount = ui->cB_HostHistory->count();
    SettingsStates["HostHistory"] = comboBoxItemCount;
    for (int i=0; i<comboBoxItemCount; i++) {
        SettingsStates[QString("ip%1").arg(i)] = ui->cB_HostHistory->itemText(i);
    }
    SettingsStates["lastUsedIp"] = QString(ui->lE_IPadress->text()+" "+ui->lE_Port->text());

    QJsonDocument doc(SettingsStates);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    SettingsSaves.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream( &SettingsSaves );
    stream << jsonString;
    SettingsSaves.close();
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

void MainWindow::reciveMouseBtnInput(MyServer::MouseInputBtnType msgType)
{

    switch (~(~msgType|MyServer::MouseKeyDown)) { // ~(~msgType|KeyDown) так как в последем бите хранится указание, что именно делать с кнопкой, на этом этапе мы её игнорируем
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

void MainWindow::reciveKeyboardBtnInput(MyServer::KeyboardInputBtnType msgType)
{
    switch (~(~msgType|MyServer::KeyboardKeyPress)) { // ~(~msgType|MyServer::KeyboardKeyPress) так как в последем бите хранится указание, что именно делать с кнопкой, на этом этапе мы её игнорируем
    case (MyServer::KeyboardKeyUp):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyUp Press");
            keybd_event( VK_UP, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyUp Release");
            keybd_event( VK_UP, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyDown):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyDown Press");
            keybd_event( VK_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyDown Release");
            keybd_event( VK_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyLeft):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyLeft Press");
            keybd_event( VK_LEFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyLeft Release");
            keybd_event( VK_LEFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyRight):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyRight Press");
            keybd_event( VK_RIGHT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyRight Release");
            keybd_event( VK_RIGHT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }

    case (MyServer::KeyboardKeyEsc):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyEsc Press");
            keybd_event( VK_ESCAPE, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyEsc Release");
            keybd_event( VK_ESCAPE, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyTab):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyTab Press");
            keybd_event( VK_TAB, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyTab Release");
            keybd_event( VK_TAB, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyCaps):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyCaps Press");
            keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyCaps Release");
            keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyShift):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyShift Press");
            keybd_event( VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyShift Release");
            keybd_event( VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyCtrl):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyCtrl Press");
            keybd_event( VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyCtrl Release");
            keybd_event( VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyWin):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyWin Press");
            keybd_event( VK_LWIN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyWin Release");
            keybd_event( VK_LWIN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    case (MyServer::KeyboardKeyAlt):{
        if (msgType&MyServer::KeyboardKeyPress){
            reciveMes("KeyAlt Press");
                keybd_event( VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);;
        }
        else{
            reciveMes("KeyAlt Release");
            keybd_event( VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;
    }
    default:
        reciveMes("KeyboardBtnInput ERROR");
    }
}

void MainWindow::reciveStringPast(QString str)
{
    reciveMes("Pasting recived message");
    if(!OpenClipboard(0)){
        reciveMes("Cant open Clipboard");
        CloseClipboard();
        return;
    }
    if(!EmptyClipboard()){
        reciveMes("Cant empty Clipboard");
        CloseClipboard();
        return;
    }
    const QChar *unicodeStr = str.unicode();
    const size_t amountOfBytes = (str.size()+1) * sizeof(QChar);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, amountOfBytes);
    if(h == nullptr){
        reciveMes("Cant GlobalAlloc");
        GlobalFree(h);
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(h), unicodeStr, amountOfBytes);
    if(GlobalUnlock(h)){
        reciveMes("Cant GlobalUnlock");
        GlobalFree(h);
        CloseClipboard();
        return;
    }
    SetClipboardData(CF_UNICODETEXT, h);
    if(!CloseClipboard()){
        reciveMes("Cant close Clipboard");
        GlobalFree(h);
        return;
    }
    GlobalFree(h);

    keybd_event( VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event( 0x56, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);

    keybd_event( 0x56, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    keybd_event( VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void MainWindow::reciveVolumeLevelChanges(MyServer::VolumeLevelChangeType msgType)
{
    switch(msgType){
    case (MyServer::VolumeLevelUp):{
        //press and release
        keybd_event( VK_VOLUME_UP, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event( VK_VOLUME_UP, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        break;
    }
    case (MyServer::VolumeLevelDown):{
        keybd_event( VK_VOLUME_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event( VK_VOLUME_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        break;
    }
    case (MyServer::VolumeLevelMute):{
        keybd_event( VK_VOLUME_MUTE, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event( VK_VOLUME_MUTE, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        break;
    }
    default:
        reciveMes("VolumeLevelChanges ERROR");
    }
}

void MainWindow::on_pB_Start_clicked()
{
    ms = new MyServer(ui->lE_IPadress->text(), ui->lE_Port->text());
    if(ms->isListening()){
        ui->textBrowser->append("Start " + ms->serverAddress().toString() + " " + QString::number(ms->serverPort()));
    }
    else{
        ui->textBrowser->append("Error");
    }

    //connect signals between MainWindow and Server
    connect(ms, &MyServer::sendMes, this, &MainWindow::reciveMes);
    connect(ms, &MyServer::sendMouseMovement , this, &MainWindow::reciveMouseMovement);
    connect(ms, &MyServer::sendMouseBtnInput , this, &MainWindow::reciveMouseBtnInput);
    connect(ms, &MyServer::sendKeyboardBtnInput , this, &MainWindow::reciveKeyboardBtnInput);
    connect(ms, &MyServer::sendStringPast , this, &MainWindow::reciveStringPast);
    connect(ms, &MyServer::sendVolumeLevelChanges , this, &MainWindow::reciveVolumeLevelChanges);
}


void MainWindow::on_pB_Stop_clicked()
{
    delete ms;
    ui->textBrowser->append("Server stoped");
}


void MainWindow::on_pB_Remeber_clicked()
{
    int comboBoxItemCount = ui->cB_HostHistory->count();
    QString ip = ui->lE_IPadress->text(), port = ui->lE_Port->text();
    ui->cB_HostHistory->addItem(QString(ip+" "+port), comboBoxItemCount);
}


void MainWindow::on_pB_Fill_clicked()
{
    FillHostAdress(ui->cB_HostHistory->currentText());
}


void MainWindow::on_pB_Forget_clicked()
{
    ui->cB_HostHistory->removeItem(ui->cB_HostHistory->currentIndex());
}

