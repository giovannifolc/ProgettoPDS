#pragma once
#include <QtNetwork>
#include "Symbol.h"
#include "Task.h"
#include "User.h"
class UserConn :
    public User
{
public:
    UserConn(QString username, QString password, QString nickname, int siteId, QTcpSocket* socket, QString filename);

    QString getFilename();
    QTcpSocket* getSocket();
    void setFilename(QString filename);
    bool setBusy(bool busy);
    bool isBusy();
    void pushTask(QVector<std::shared_ptr<Symbol>> symbols, int n_sym, int insert, int siteIdSender);
    void pushTask(QByteArray bufferSymbols);
  //  QByteArray popBufferTask();
    std::shared_ptr<Task> popTask();
    bool havePendingTask();
    void setCounter(int counter);
    int getCounter();

private:
    QTcpSocket* socket;
    QString filename;
    bool busy = false;
    QVector<std::shared_ptr<Task>> tasks;
    int counter = 0;
};

