#ifndef COMMANDSTRANSMITTER_H
#define COMMANDSTRANSMITTER_H
#include <QObject>
#include "protocol.h"
#include "TcpServerConnection.h"
#include <QString>
class CommandsTransmitter
{


private:
    QIODevice* mCommandsWriter;
    void writeCommand(const QString& aCommand);
    QIODevice* buildCommandsWriter(TcpServerConnection* aServerConnection);

public:
    explicit CommandsTransmitter(TcpServerConnection*aServerConnection);
    void pair(const QString & aDeviceName,const QString & aPin);
    void performNextTransition();
    void performPreviousTransition();
    void setCurrentSlide(int slideIndex);
    void setUpBlankScreen();
    void resumePresentation();
    void startPresentation();
    void stopPresentation();
    void startPointer(float x,float y);
    void movePointer(float x, float y);
    void stopPointer();


};

#endif // COMMANDSTRANSMITTER_H
