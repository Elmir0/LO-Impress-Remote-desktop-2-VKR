#include "communicationservice.h"
#include "server.h"
#include "TcpServerConnection.h"
#include "commandstransmitter.h"
#include "messagesreceiver.h"
#include "pairingprovider.h"
#include "protocol.h"
#include <QDebug>
#include <stdexcept>

CommunicationService::ServiceBinder::ServiceBinder(CommunicationService* aCommunicationService)
    : mCommunicationService(aCommunicationService)
{
}

CommunicationService* CommunicationService::ServiceBinder::getService() {
    return mCommunicationService;
}

CommunicationService::CommunicationService(QObject* parent)
    : QObject(parent)
    , mBinder(new ServiceBinder(this))
    , mServer(nullptr)
    , mServerConnection(nullptr)
    , mMessagesReceiver(nullptr)
    , mCommandsTransmitter(nullptr)
    ,mSlideShow(new slideshow(this))
{
}

CommunicationService::~CommunicationService() {
    disconnectServer();
    delete mBinder;
}

void CommunicationService::startServersSearch() {
    // TODO: реализовать позже
}

void CommunicationService::addServer(const QString& address, const QString& name) {
    // TODO: реализовать позже
}

void CommunicationService::removeServer(Server* aServer) {
    // TODO: реализовать позже
}

QList<Server*> CommunicationService::getServers() {
    return QList<Server*>();  // TODO: реализовать позже
}

// ОСНОВНОЙ МЕТОД ПОДКЛЮЧЕНИЯ
void CommunicationService::connectServer(Server* aServer) {
    mServer = aServer;

    try {
        disconnectServer();

        // 1. Создаем соединение
        mServerConnection = new TcpServerConnection(mServer);

        // 2. Открываем соединение
        qDebug() << " Открываю соединение...";
        mServerConnection->open();

        // 3. Создаем отправитель команд
        mCommandsTransmitter = new CommandsTransmitter(mServerConnection);

        // 4. Создаем и настраиваем приемник сообщений
        mMessagesReceiver = new MessagesReceiver(mServerConnection, this);

        //  ПОДКЛЮЧАЕМ СИГНАЛЫ ДЛЯ PIN КОДА
        connect(mMessagesReceiver, &MessagesReceiver::pinValidationRequired,
                this, &CommunicationService::handlePinValidation);
        connect(mMessagesReceiver, &MessagesReceiver::pairingSuccessful,
                this, &CommunicationService::handlePairingSuccessful);
        connect(mMessagesReceiver, &MessagesReceiver::connectionError,
                this, [this](const QString& error) {
                    qDebug() << " Ошибка соединения:" << error;
                    emit connectionFailed();
                });

        // Сигналы слайдов
        connect(mMessagesReceiver, &MessagesReceiver::slideShowStarted,
                this, [this](int slidesCount, int currentSlide) {
                    mSlideShow->reset();
                    mSlideShow->setSlidesCount(slidesCount);
                    mSlideShow->setCurrentSlideIndex(currentSlide);
                    mSlideShow->setRunning(true);
                    emit slideShowStarted(slidesCount, currentSlide);
                });

        connect(mMessagesReceiver, &MessagesReceiver::slideChanged,
                this, [this](int currentSlide) {
                    mSlideShow->setCurrentSlideIndex(currentSlide);
                    emit slideChanged(currentSlide);
                });

        connect(mMessagesReceiver, &MessagesReceiver::slidePreviewReceived,
                this, [this](int slideIndex, const QByteArray& preview) {
                    mSlideShow->setSlidePreviewBytes(slideIndex, preview);
                    emit slidePreviewReceived(slideIndex, preview);
                });

        connect(mMessagesReceiver, &MessagesReceiver::slideNotesReceived,
                this, [this](int slideIndex, const QString& notes) {
                    mSlideShow->setSlideNotes(slideIndex, notes);
                    emit slideNotesReceived(slideIndex, notes);
                });

        connect(mMessagesReceiver, &MessagesReceiver::slideShowFinished,
                this, [this]() {
                    mSlideShow->setRunning(false);
                });

        // 5. Запускаем поток приема сообщений
        mMessagesReceiver->start();
        qDebug() << "📡 MessagesReceiver запущен";

        // 6. Если нужен pairing - отправляем запрос
        if (PairingProvider::isPairingNecessary(mServer)) {
            qDebug() << " Выполняю pairing...";
            pair();
        } else {
            qDebug() << " Подключено без pairing";
        }

        qDebug() << " Успешно подключились к серверу:" << mServer->getName();

    } catch (const std::exception& e) {
        //qDebug() << " Ошибка подключения:" << e.what();
        sendConnectionFailedMessage();
    }
}

void CommunicationService::disconnectServer() {
    if (!isServerConnectionAvailable()) {
        return;
    }

    if (mMessagesReceiver) {
        mMessagesReceiver->stop();
        mMessagesReceiver->wait(3000);
        delete mMessagesReceiver;
        mMessagesReceiver = nullptr;
    }

    mServerConnection->close();

    delete mCommandsTransmitter;
    mCommandsTransmitter = nullptr;

    delete mServerConnection;
    mServerConnection = nullptr;
}

bool CommunicationService::isServerConnectionAvailable() const {
    return mServerConnection != nullptr;
}

//  МЕТОД ДЛЯ PAIRING (ОТПРАВКА PIN КОДА)
void CommunicationService::pair() {
    if (!mServer || !mCommandsTransmitter) {
        qDebug() << "❌Не могу выполнить pairing: сервер или команды не инициализированы";
        return;
    }

    QString deviceName = PairingProvider::getPairingDeviceName();
    QString pin = PairingProvider::getPairingPin(mServer);

    qDebug() << " Отправляю pairing запрос:";
    qDebug() << "  Устройство:" << deviceName;
    qDebug() << "  PIN код:" << pin;

    mCommandsTransmitter->pair(deviceName, pin);
}

//  ОБРАБОТКА PIN КОДА ОТ SERVER
void CommunicationService::handlePinValidation(const QString& pin)
{
    Q_UNUSED(pin);  // Игнорируем пустой PIN от MessagesReceiver

    //  БЕРЕМ ПРАВИЛЬНЫЙ PIN ИЗ PAIRINGPROVIDER
    if (!mServer) {
        qDebug() << " Сервер не инициализирован";
        return;
    }

    QString actualPin = PairingProvider::getPairingPin(mServer);
    qDebug() << " Отправляю PIN в MainWindow:" << actualPin;

    emit pairingValidationRequired(actualPin);
}

//  ОБРАБОТКА УСПЕШНОГО PAIRING
void CommunicationService::handlePairingSuccessful() {
    qDebug() << " Pairing успешен!";
    emit pairingSuccessful();
}

void CommunicationService::sendConnectionFailedMessage() {
    emit connectionFailed();
}

CommandsTransmitter* CommunicationService::getCommandsTransmitter() {
    return mCommandsTransmitter;
}

// КОМАНДЫ ДЛЯ КНОПОК
void CommunicationService::nextSlide() {
    if (mCommandsTransmitter != nullptr) {
        mCommandsTransmitter->performNextTransition();
        qDebug() << " Отправлена команда: следующий слайд";
    }
}

void CommunicationService::previousSlide() {
    if (mCommandsTransmitter != nullptr) {
        mCommandsTransmitter->performPreviousTransition();
        qDebug() << " Отправлена команда: предыдущий слайд";
    }
}
