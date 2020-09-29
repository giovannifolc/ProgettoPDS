#pragma once

#include <QtCore>
#include <QtNetwork>
#include <QtGui>
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
#include "StyleSymbol.h"
class Server :
    public QObject
{
    Q_OBJECT
public:
        explicit Server(QObject* parent = 0);
        ~Server();
Q_SIGNALS:/*definizione di segnali*/
	void closed();

private slots:
	void onNewConnection();
	void onDisconnected(QTcpSocket *socket);
	void saveFile(TextFile *f);
	void onReadyRead();
	void changeCredentials(QString username, QString old_password, QString new_password, QString nickname, QTcpSocket* receiver);
	void registration(QString username, QString password, QString nickname, QTcpSocket* sender);
	bool login(QString username, QString password, QTcpSocket* sender);
	void sendFiles(QString username, QTcpSocket* receiver, bool success);
	void insertSymbol(QString filename, QTcpSocket* sender, QDataStream* in);
	void sendSymbol(GenericSymbol* symbol, bool insert, QTcpSocket* socket);
	double generateDecimal(QVector<int> pos);
	void sendFile(QString filename, QTcpSocket* socket, QMap<QTcpSocket*, UserConn*> clients, QMap<QString, TextFile*> files);
	void sendClient(QString nickname, QTcpSocket* socket, bool insert);
	void deleteSymbol(QString filename, int siteId, int counter, QVector<int> pos, QTcpSocket* sender);

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
	void addNewUser();
	void addNewFile(QString filename, QString user);
	

};

