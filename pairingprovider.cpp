#include "pairingprovider.h"
#include <QDebug>
#include <QCoreApplication>
#include "server.h"
#include "protocol.h"


bool PairingProvider::isPairingNecessary(Server* aServer) {
    Q_UNUSED(aServer);
    return true;
}


QString PairingProvider::getPairingPin(Server* aServer) {

    if (isPinSaved(aServer)) {
        QString savedPin = getSavedPin(aServer);
        qDebug() << " Используем сохраненный PIN для" << aServer->getAddress() << ":" << savedPin;
        return savedPin;
    }

    QString aPin = Protocol::Pin::generate();
    qDebug() << " Сгенерирован новый PIN для"<< aServer->getAddress() << ":" << aPin;
    savePin(aServer, aPin);

    return aPin;
}

bool PairingProvider::isPinSaved(Server* aServer) {
    return !getSavedPin(aServer).isEmpty();
}

QString PairingProvider::getSavedPin(Server* aServer) {
    return getSettings().value(aServer->getAddress()).toString();
}

void PairingProvider::savePin(Server* aServer, const QString& aPin) {
    getSettings().setValue(aServer->getAddress(), aPin);
    getSettings().sync();
    qDebug() << "PIN сохранен для" << aServer->getAddress();
}

QString PairingProvider::getPairingDeviceName() {

    return QCoreApplication::applicationName() + " Desktop";
}

QSettings& PairingProvider::getSettings() {
    static QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                              "LibreOffice", "ImpressRemote");
    return settings;
}
