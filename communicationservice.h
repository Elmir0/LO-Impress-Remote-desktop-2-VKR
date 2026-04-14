#ifndef COMMUNICATIONSERVICE_H
#define COMMUNICATIONSERVICE_H

#include <QObject>
#include "server.h"
#include "TcpServerConnection.h"
#include "commandstransmitter.h"
#include "messagesreceiver.h"
#include "pairingprovider.h"
#include "protocol.h"
#include <QByteArray>
#include "slideshow.h"


class CommunicationService : public QObject
{
    Q_OBJECT

public:
    class ServiceBinder : public QObject {
    public:
        explicit ServiceBinder(CommunicationService* aCommunicationService);
        CommunicationService* getService();
    private:
        CommunicationService* mCommunicationService;
    };

    explicit CommunicationService(QObject* parent = nullptr);
    ~CommunicationService();

    void startServersSearch();
    void addServer(const QString& address, const QString& name);
    void removeServer(Server* aServer);
    QList<Server*> getServers();

    void connectServer(Server* aServer);
    void disconnectServer();

    CommandsTransmitter* getCommandsTransmitter();

    //  ОСНОВНЫЕ КОМАНДЫ ДЛЯ КНОПОК
    void nextSlide();
    void previousSlide();

private:
    ServiceBinder* mBinder;
    Server* mServer;
    TcpServerConnection* mServerConnection;
    MessagesReceiver* mMessagesReceiver;
    CommandsTransmitter* mCommandsTransmitter;
    slideshow* mSlideShow;


    void pair();
    void sendConnectionFailedMessage();
    bool isServerConnectionAvailable() const;

private slots:
    void handlePinValidation(const QString& pin);
    void handlePairingSuccessful();

signals:
    void connectionFailed();
    void pairingValidationRequired(const QString& pin);
    void pairingSuccessful();
    void slideShowStarted(int slidesCount, int currentSlide);
    void slideChanged(int currentSlide);
    void presentationStarted();
    void presentationStopped();
    // слайды
    void slidePreviewReceived(int slideIndex, const QByteArray& preview);
    void slideNotesReceived(int slideIndex, const QString& notes);
};

#endif // COMMUNICATIONSERVICE_H
