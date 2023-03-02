#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTouchEvent>
#include <QRegularExpressionValidator>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>


#include "sensearea.h"
#include "scrollbar.h"
#include "settinghandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString IPadress;
    quint16 Port;
    SenseArea *sa;
    ScrollBar *sb;
    QTcpSocket *TCPsocket;
    QUdpSocket *UDPsocket;
    QByteArray data;
    quint16 nextBlockSize;
    quint16 messageType;
    QRegularExpressionValidator *mouseSenseValidator;
    QRegularExpressionValidator *scrollSenseValidator;
    QRegularExpressionValidator *ipAdressValidator;
    QRegularExpressionValidator *portValidator;
    QFile SettingsSaves;
    SettingHandler* settings;
    int oldMouseX = 0, oldMouseY = 0;
    int oldScrollY;
    int mouseSense=1, scrollSense=1;
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
    void SendToServerUDP(quint16 msgType, quint16 msg, QString str="");
    void SendToServerTCP(quint16 msgType, quint16 msg, QString str="");
    void saveHosts();
    void saveMouseSettings();
    void FillHostAdress(QString host);

protected:

public slots:
    void TouchMove(QTouchEvent *te);
    void ClickDo();
    void ScrollMove(QTouchEvent *te);
    void connectResived();
    void disconnectResived();

private slots:
    /**
     * Buttons of ConnectWindow
     */
    void on_pB_Connect_clicked();
    void on_pB_Remeber_clicked();
    void on_pB_Fill_clicked();
    void on_pB_Forget_clicked();

    /**
     * Buttons of InputWindow
     */
    void on_pB_Send_clicked();
    void on_pB_LeftClick_pressed();
    void on_pB_LeftClick_released();
    void on_pB_MiddleClick_pressed();
    void on_pB_MiddleClick_released();
    void on_pB_RightClick_pressed();
    void on_pB_RightClick_released();

    void on_pB_Settings_clicked();
    void on_pB_Keyboard_clicked();

    /**
     * Buttons of SettingsWindow
     */
    void on_pB_SettingsBack_clicked();
    void on_lE_MouseSense_textChanged(const QString &arg1);
    void on_pB_MouseSensePlus_clicked();
    void on_pB_MouseSenseMinus_clicked();
    void on_lE_ScrollSense_textChanged(const QString &arg1);
    void on_pB_ScrollSense_clicked();
    void on_pB_ScrollSense_2_clicked();

    /**
     * Buttons of KeyboardWindow
     */
    void on_pB_KeyboardBack_clicked();
    void on_pB_VolumePlus_clicked();
    void on_pB_VolumeMinus_clicked();
    void on_pB_VolumeMute_clicked();
    void on_pB_KeyAlt_toggled(bool checked);

    void on_pB_KeyUp_pressed();
    void on_pB_KeyUp_released();

    void on_pB_KeyLeft_pressed();
    void on_pB_KeyLeft_released();

    void on_pB_KeyDown_pressed();
    void on_pB_KeyDown_released();

    void on_pB_KeyRight_pressed();
    void on_pB_KeyRight_released();

    void on_pB_KeyEsc_pressed();
    void on_pB_KeyEsc_released();

    void on_pB_KeyTab_pressed();
    void on_pB_KeyTab_released();

    void on_pB_KeyCaps_pressed();
    void on_pB_KeyCaps_released();

    void on_pB_KeyShift_toggled(bool checked);

    void on_pB_KeyWin_pressed();
    void on_pB_KeyWin_released();

    void on_pB_KeyCtrl_toggled(bool checked);


};
#endif // MAINWINDOW_H
