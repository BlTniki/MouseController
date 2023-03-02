#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H

#include <QtCore/QSettings>

class SettingHandler
{
public:
    SettingHandler();

    void saveMouseSense(int val);
    int getSavedMouseSense();
    void saveScrollSense(int val);
    int getSavedScrollSense();
    void setLastHost(QString host);
    QString getLastHost();
    void saveHost(QString hostWithPort);
    void saveHost(QList<QString> hostsWithPort);
    QVariantList getSavedHosts();

private:
    QSettings settings;
    const QString MOUSE_SENSE = "mouseSense";
    const QString SCROLL_SENSE = "scrollSense";
    const QString LAST_USED_HOST = "lastUsedHost";
    const QString SAVED_HOSTS = "savedHost";
};

#endif // SETTINGHANDLER_H
