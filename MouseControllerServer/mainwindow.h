#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QApplication>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QStyle>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QRegularExpressionValidator>
#include <windows.h>

#include "myserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool AutoRun = false;

protected:
void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;
    MyServer *ms;
    QSystemTrayIcon *trayIcon;
    QRegularExpressionValidator *ipAdressValidator;
    QRegularExpressionValidator *portValidator;
    QJsonObject SettingsStates{{"HostHistory",2},
                               {"ip0", "0.0.0.0 0000"},
                               {"ip1", "0.0.0.1 0001"}
                              };
    QFile SettingsSaves;
    void CursorMove(QString str);
    void ScrollMove(QString str);
    void FillHostAdress(QString host);

public slots:
    void reciveMes(QString str);
    void reciveStringPast(QString str);
    void reciveVolumeLevelChanges(MyServer::VolumeLevelChangeType msgType);
    void reciveMouseMovement(MyServer::MouseMovementType msgType, QString str);
    void reciveMouseBtnInput(MyServer::MouseInputBtnType msgType);
    void reciveKeyboardBtnInput(MyServer::KeyboardInputBtnType msgType);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void on_pB_Start_clicked();
    void on_pB_Stop_clicked();
    void on_pB_Remeber_clicked();
    void on_pB_Fill_clicked();
    void on_pB_Forget_clicked();
};
#endif // MAINWINDOW_H
