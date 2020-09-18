#pragma once

#include <QtCore>
#include <QtNetwork>

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <queue>
#include <memory>
#include <fstream>

#include "TextSymbol.h"
#include "UserConn.h"
#include "TextFile.h"
class Server :
    public QObject
{
    Q_OBJECT
public:
        explicit Server(QObject* parent = 0);
        ~Server();

private slots:
	void onNewConnection();
	void onDisconnected();
	void onReadyRead();
	void changeCredentials(QString username, QString old_password, QString new_password, QString nickname, QTcpSocket* receiver);
	void registration(QString username, QString password, QString nickname, QTcpSocket* sender);
	bool login(QString username, QString password, QTcpSocket* sender);
	void sendFiles(QString username, QTcpSocket* receiver, bool success);

private:
	QTcpServer* server;
	int siteIdCounter = 0; //devo salvarlo da qualche parte in caso di crash?

	QMap<QTcpSocket*, UserConn*> clients;//client connessi
	
	QMap<QString, TextFile*> files;//file in archivio

	QMap<QString, User*> subs;//utenti iscritti
	
	QMap<QString, QVector<QString>> filesForUser;
	
	void load_subs();
	void load_files();
	void load_file(TextFile* f);
	void onNewConnection();
	
	/*
	void check_credentials(QTcpSocket* socket, int login_sign, std::string username, std::string password, std::string nickname);
	void check_file(QTcpSocket* socket, std::string filename);
	*/

};
