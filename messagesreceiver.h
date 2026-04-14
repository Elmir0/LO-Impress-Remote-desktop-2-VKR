#ifndef MESSAGESRECEIVER_H
#define MESSAGESRECEIVER_H
#include<QObject>
#include <QThread>
#include <QStringList>
#include "TcpServerConnection.h"

class MessagesReceiver:public QThread
{
Q_OBJECT
public:
explicit MessagesReceiver(TcpServerConnection*aServerConnection,QObject*parent=nullptr);
    ~MessagesReceiver();
void stop();

protected:
void run() override;

private:
QIODevice*mMessagesReader;
bool mRunning;
QStringList readMessage();
QString readMessageParameter();
void parseMessage(const QStringList&aMessage);

int parseSlidesCount(const QStringList& aMessage,int aMessageParameterIndex);
int parseSlideIndex(const QStringList&aMessage,int aMessageParameterIndex);
QByteArray parseSlidePreview(const QStringList &aMessage,int aMessageParameterIndex);
QString parseSlideNotes(const QStringList &aMessage,int aMessageParameterIndex);

signals:

void pinValidationRequired(const QString& pin);      // "Введите PIN: 1234"
void pairingSuccessful();                            //  "Подключено!"
void slideShowStarted(int slidesCount, int currentSlide);
void slideShowFinished();
void slideChanged(int currentSlide);
void slidePreviewReceived(int slideIndex, const QByteArray& preview);
void slideNotesReceived(int slideIndex, const QString& notes);
void connectionError(const QString& error);





};

#endif // MESSAGESRECEIVER_H
