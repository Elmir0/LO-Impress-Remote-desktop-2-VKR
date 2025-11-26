#ifndef TCPSERVERCONNECTION_H
#define TCPSERVERCONNECTION_H
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QIODevice>

class Server;
class Protocol;

#pragma once
class TcpServerConnection{
private:
    Server*mServer;
    QTcpSocket*mServerConnection;
    QTcpSocket*buildServerConnection();
    QHostAddress buildServerAddress();
public:
    explicit TcpServerConnection(Server*aServer);
    void open();
    void close();
    QIODevice*buildMessagesStream();
    QIODevice*buildCommandsStream();
};
#endif // TCPSERVERCONNECTION_H
