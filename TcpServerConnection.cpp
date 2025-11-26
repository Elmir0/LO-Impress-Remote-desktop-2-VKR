#include "TcpServerConnection.h"
#include "server.h"
#include "protocol.h"
#include <QDebug>
#include <stdexcept>

TcpServerConnection::TcpServerConnection(Server* aServer)
    : mServer(aServer)
    , mServerConnection(buildServerConnection())
{
}

QTcpSocket* TcpServerConnection::buildServerConnection() {
    return new QTcpSocket();
}

void TcpServerConnection::open() {
    try {
        mServerConnection->connectToHost(buildServerAddress(), Protocol::Ports::CLIENT_CONNECTION);

        if (!mServerConnection->waitForConnected(5000)) {
            throw std::runtime_error("Unable to open server connection.");
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Unable to open server connection.");
    }
}

QHostAddress TcpServerConnection::buildServerAddress() {
    QString aServerAddress = mServer->getAddress();
    int aServerPort = Protocol::Ports::CLIENT_CONNECTION;
    return QHostAddress(aServerAddress);
}

void TcpServerConnection::close() {
    try {
        mServerConnection->close();
    } catch (const std::exception& e) {
        throw std::runtime_error("Unable to close server connection.");
    }
}


QIODevice* TcpServerConnection::buildMessagesStream() {
    try {
        return mServerConnection;
    } catch (const std::exception& e) {
        throw std::runtime_error("Unable to open messages stream.");
    }
}

QIODevice* TcpServerConnection::buildCommandsStream() {
    try {
        return mServerConnection;
    } catch (const std::exception& e) {
        throw std::runtime_error("Unable to open commands stream.");
    }
}
