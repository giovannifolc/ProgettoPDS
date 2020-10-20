#pragma once
#include <QtNetwork>

#include "User.h"
class UserConn :
    public User
{
public:
    UserConn(QString username, QString password, QString nickname, int siteId, QTcpSocket* socket, QString filename);

    QString getFilename();
    QTcpSocket* getSocket();
    void setFilename(QString filename);
private:
    QTcpSocket* socket;
    QString filename;
    std::thread thread;
};

