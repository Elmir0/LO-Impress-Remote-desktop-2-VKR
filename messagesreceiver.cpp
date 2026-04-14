#include "messagesreceiver.h"
#include "protocol.h"
#include<QDebug>
#include<QTextStream>

MessagesReceiver::MessagesReceiver(TcpServerConnection* aServerConnection, QObject* parent)
    : QThread(parent)
    , mMessagesReader(aServerConnection->buildMessagesStream())
    , mRunning(true)
{
    qDebug() << "MessagesReceiver создан";
}

MessagesReceiver::~MessagesReceiver(){
    stop();
    wait();
}

void MessagesReceiver::stop(){
    mRunning=false;
    requestInterruption();
}

void MessagesReceiver::run(){
    while(mRunning&& !isInterruptionRequested()){
        QStringList aMessage = readMessage();
        if(aMessage.isEmpty()&& !mMessagesReader->isOpen()){
            qDebug()<<"Соединение закрыто,останавливаем поток";
            break;
        }
        if(!aMessage.isEmpty()){
            parseMessage(aMessage);
        }
    }
    qDebug()<<"MessagesReceiver поток остановлен";
}

QStringList MessagesReceiver::readMessage(){
    QStringList aMessage;
    QString aMessageParameter =   readMessageParameter();
    while (true) {
        if(aMessageParameter.isNull()){
           // emit connectionError("соединение разорвано");
            break;
        }
        if(aMessageParameter.isEmpty()){
            break;
        }
        aMessage.append(aMessageParameter);
        aMessageParameter=readMessageParameter();

    }
    if(!aMessage.isEmpty()){
        qDebug()<<"Получено сообщение:"<< aMessage;
    }
    return aMessage;
}

QString MessagesReceiver::readMessageParameter(){
    if(!mMessagesReader || !mMessagesReader->isOpen()){
        return QString(); // null — разрыв соединения
    }

    // Ждём данные
    while(!mMessagesReader->canReadLine()){
        if(!mRunning || isInterruptionRequested()){
            return QString(); // null — остановка
        }
        if(!mMessagesReader->waitForReadyRead(500)){
            return ""; // empty — таймаут, конец сообщения
        }
    }

    // Читаем строку с лимитом размера (10MB достаточно для превью)
    QByteArray data = mMessagesReader->readLine(10 * 1024 * 1024);

    if(data.isEmpty()){
        return ""; // empty
    }

    data = data.trimmed();

    if(data.isEmpty()){
        return ""; // empty — пустая строка-разделитель
    }

    QString result = QString::fromUtf8(data);
    qDebug() << "Прочитана строка длиной:" << result.size() << "первые символы:" << result.left(30);
    return result;
}
void MessagesReceiver::parseMessage(const QStringList&aMessage){
    if(aMessage.isEmpty()){
        return;
    }
    QString aMessageType = aMessage[0];
    qDebug()<<"Парсим сообщение типа:"<<aMessageType;
    if(Protocol::Messages::VALIDATING==aMessageType){
        if(aMessage.size()>1){
            QString pin=aMessage[1];
            qDebug()<<"Требуется валидация PIN:"<<pin;
            emit pinValidationRequired(pin);

        }else{
            qDebug()<<"Ошибка: VALIDATING сообщение без PIN кода";
            emit pinValidationRequired("");
        }
        return;
    }
    if(Protocol::Messages::PAIRED==aMessageType){
        qDebug()<<"Pairing успешен";
        emit pairingSuccessful();
        return;
    }

    if(Protocol::Messages::SLIDE_SHOW_STARTED==aMessageType){
        if(aMessage.size()>=3){
            int aSlidescount = parseSlidesCount(aMessage,1);
            int aCurrentSlideIndex=parseSlidesCount(aMessage,2);
            qDebug()<<"Презентация начата: слайдов"<<aSlidescount<<"Текущий:"<<aCurrentSlideIndex;
            emit slideShowStarted(aSlidescount,aCurrentSlideIndex);
        }
        return;
    }

    if (Protocol::Messages::SLIDE_SHOW_FINISHED==aMessageType){
        qDebug()<<"Презентация завершена";
        emit slideShowFinished();
        return;
    }

    if(Protocol::Messages::SLIDE_UPDATED==aMessageType){
        if(aMessage.size()>=2){
            int aCurrentSlideIndex = parseSlideIndex(aMessage,1);
            qDebug()<<"Слайд изменен на:"<< aCurrentSlideIndex;
            emit slideChanged(aCurrentSlideIndex);
        }
        return;
    }

    if(Protocol::Messages::SLIDE_PREVIEW==aMessageType){
        if(aMessage.size()>=3){
            int aSlideIndex = parseSlideIndex(aMessage,1);
            QByteArray aSlidePreview=parseSlidePreview(aMessage,2);
            qDebug()<<"Получено превью слайда"<<aSlideIndex<<"размер:"<<aSlidePreview.size()<<"байт";
            emit slidePreviewReceived(aSlideIndex,aSlidePreview);
        }
        return;
    }

    if(Protocol::Messages::SLIDE_NOTES==aMessageType){
        if(aMessage.size()>=2){
            int aSlideIndex = parseSlideIndex(aMessage,1);
            QString aSlideNotes = parseSlideNotes(aMessage,2);
            qDebug()<<"Получен заметки для слайда"<<aSlideIndex;
            emit slideNotesReceived(aSlideIndex, aSlideNotes);

        }
    }

}

int MessagesReceiver::parseSlidesCount(const QStringList&aMessage,int aMessageParameterIndex){
    if(aMessageParameterIndex<aMessage.size()){
        bool ok;
        int result = aMessage[aMessageParameterIndex].toInt(&ok);
        return ok?result:0;
    }
    return 0;
}

int MessagesReceiver::parseSlideIndex(const QStringList& aMessage,int aMessageParameterIndex){
    if(aMessageParameterIndex<aMessage.size()){
        bool ok;
        int result = aMessage[aMessageParameterIndex].toInt(&ok);
        return ok?result:0;
    }
    return 0;
}
QByteArray MessagesReceiver::parseSlidePreview(const QStringList& aMessage, int aMessageParameterIndex) {
    if (aMessageParameterIndex < aMessage.size()) {
        QString aPreviewAsString = aMessage[aMessageParameterIndex];
        return QByteArray::fromBase64(aPreviewAsString.toUtf8());
    }
    return QByteArray();
}

QString MessagesReceiver::parseSlideNotes(const QStringList& aMessage, int aMessageParameterIndex) {
    QString aNotesBuilder;
    for (int aNoteIndex = aMessageParameterIndex; aNoteIndex < aMessage.size(); ++aNoteIndex) {
        aNotesBuilder.append(aMessage[aNoteIndex]);
        if (aNoteIndex < aMessage.size() - 1) {
            aNotesBuilder.append("\n");
        }
    }
    return aNotesBuilder;

}





