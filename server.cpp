#include "server.h"
Server::Server(const QString&address,const QString&name):mAddress(address),mName(name){}

Server*Server:: newTcpInstance(const QString&address,const QString&name)
{
    return new Server(address,name);
}

QString Server:: getAddress()const{
    return mAddress;
}

QString Server::getName()const{
    if(mName.isEmpty()){
        return mAddress;
    }
    return mName;
}




