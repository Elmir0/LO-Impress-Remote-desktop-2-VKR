#ifndef PAIRINGPROVIDER_H
#define PAIRINGPROVIDER_H
#include <QString>
#include <QSettings>
#include "server.h"
#include "protocol.h"
class PairingProvider
{
private:
    PairingProvider()=delete;
    static bool isPinSaved(Server*aServer);
    static QString getSavedPin(Server*aServer);
    static void savePin(Server*aServer,const QString&aPin);
    static QSettings&getSettings();
    static QString m_lastPin;
public:
    static bool isPairingNecessary(Server*aServer);
    static QString getPairingPin(Server*aServer);
    static QString getPairingDeviceName();

};

#endif // PAIRINGPROVIDER_H
