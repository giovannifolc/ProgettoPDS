#include "UserConn.h"

UserConn::UserConn(QString username, QString password, QString nickname, int siteId, QTcpSocket* socket, QString filename):
    User(username, password, nickname, siteId), socket(socket), filename(filename)
{
}

QString UserConn::getFilename()
{
    return filename;
}

QTcpSocket* UserConn::getSocket()
{
    return socket;
}

void UserConn::setFilename(QString filename)
{
    this->filename = filename;
}

bool UserConn::setBusy(bool busy) {
    bool temp = this->busy;
    this->busy = busy;
    return temp;
}

bool UserConn::isBusy() {
    return this->busy;
}

void UserConn::pushTask(QVector<std::shared_ptr<Symbol>> symbols, int n_sym, int insert, int siteIdSender){
    Task task{ symbols, n_sym, insert, siteIdSender };
    tasks.push_back(std::make_shared<Task>(task));
    return;
}
void UserConn::pushTask(QByteArray buffer) {
    Task task{buffer};
    tasks.push_back(std::make_shared<Task>(task));
    return;
}

std::shared_ptr<Task> UserConn::popTask(){
    if (tasks.size() == 0) {
        Task t{};
        std::shared_ptr<Task> task = std::make_shared<Task>(t);
        return task;
    }
    else {
        std::shared_ptr<Task> task = tasks[0];
        tasks.remove(0);
        return task;
    }
}

bool UserConn::havePendingTask() {
    if (tasks.size() == 0) {
        return false;
    }else{
        return true;
    }
}

void UserConn::setCounter(int counter) {
    this->counter = counter;
}

int UserConn::getCounter() {
    return counter;
}

/*QByteArray UserConn::popBufferTask() {

}*/