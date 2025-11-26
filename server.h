#ifndef SERVER_H
#define SERVER_H
#include <QString>
class Server
{
private:
    QString mAddress;
    QString mName;

    Server(const QString&address,const QString&name);
public:
    static Server* newTcpInstance(const QString&address,const QString&name="");
    QString getAddress()const;
    QString getName()const;
};

#endif // SERVER_H
