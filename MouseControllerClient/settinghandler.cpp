#include "settinghandler.h"

SettingHandler::SettingHandler()
{
    //Init default values
    //If there no val set with this key -- init
    settings.setValue(
                MOUSE_SENSE,
                settings.value(
                    MOUSE_SENSE,
                    1
                )
    );
    settings.setValue(
                SCROLL_SENSE,
                settings.value(
                    SCROLL_SENSE,
                    1
                )
    );
    settings.setValue(
                LAST_USED_HOST,
                settings.value(
                    LAST_USED_HOST,
                    "192.168.1.2 2323"
                )
    );
    QVariantList hostList;
    settings.setValue(
                SAVED_HOSTS,
                settings.value(
                    SAVED_HOSTS,
                    hostList
                )
                );
}

void SettingHandler::saveMouseSense(int val)
{
    settings.setValue(MOUSE_SENSE, val);
}

int SettingHandler::getSavedMouseSense()
{
    return settings.value(MOUSE_SENSE).toInt();
}

void SettingHandler::saveScrollSense(int val)
{
    settings.setValue(SCROLL_SENSE, val);
}

int SettingHandler::getSavedScrollSense()
{
    return settings.value(SCROLL_SENSE).toInt();
}

void SettingHandler::setLastHost(QString hostWithPort)
{
    settings.setValue(LAST_USED_HOST, hostWithPort);
}

QString SettingHandler::getLastHost()
{
    return settings.value(LAST_USED_HOST).toString();
}

void SettingHandler::saveHost(QString hostWithPort)
{
    QVariantList hostList = settings.value(SAVED_HOSTS).toList();
    hostList.append(hostWithPort);
    settings.setValue(SAVED_HOSTS, hostList);
}

void SettingHandler::saveHost(QList<QString> hostsWithPort)
{
    QVariantList variantList;
    foreach (const QString& value, hostsWithPort) {
        variantList.append(value);
    }
    settings.setValue(SAVED_HOSTS, variantList);
}

QVariantList SettingHandler::getSavedHosts()
{
    return settings.value(SAVED_HOSTS).toList();
}
