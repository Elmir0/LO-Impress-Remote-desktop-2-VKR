#ifndef COMMANDSTRANSMITTER_H
#define COMMANDSTRANSMITTER_H
#include <QObject>
#include "protocol.h"
#include "TcpServerConnection.h"
class CommandsTransmitter
{
public:
    explicit CommandsTransmitter(TcpServerConnection*aServerConnection)
        void pair(const QString& aDeviceName, const QString& aPin);
};

#endif // COMMANDSTRANSMITTER_H
