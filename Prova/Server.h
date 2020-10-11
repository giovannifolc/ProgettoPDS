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
#include <random>
#include "UserConn.h"
#include "TextFile.h"
#include "Symbol.h"

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
	void onDisconnected();
	void saveFile(TextFile* f);
	void onReadyRead();
	void saveIfLast(QString filename);
	void changeCredentials(QString username, QString old_password, QString new_password, QString nickname, QTcpSocket* receiver);
	void registration(QString username, QString password, QString nickname, QTcpSocket* sender);
	bool login(QString username, QString password, QTcpSocket* sender);
	void sendFiles(QTcpSocket* receiver);
	void insertSymbol(QString filename, QTcpSocket* sender, QDataStream* in, int siteId, int counter, QVector<int> pos);
	void sendSymbols(int n_sym, QVector<std::shared_ptr<Symbol>> symbols, bool insert, QTcpSocket* socket, QString filename);
	void sendSymbol(std::shared_ptr<Symbol> symbol, bool insert, QTcpSocket* socket);
	void sendFile(QString filename, QTcpSocket* socket);
	void sendClient(QString nickname, QTcpSocket* socket, bool insert);
	void deleteSymbol(QString filename, int siteId, int counter, QVector<int> pos, QTcpSocket* sender);

private:
	QTcpServer* server;
	int siteIdCounter = 0; //devo salvarlo da qualche parte in caso di crash?

	QMap<QTcpSocket*, UserConn*> connections;//client connessi

	QMap<QString, TextFile*> files;//file in archivio

	QMap<QString, User*> subs;//utenti iscritti

	QMap<QString, QVector<QString>> filesForUser; //file associati ad ogni utente 

	QMap<QString, QVector<QString>> fileOwnersMap; //utenti associati ad ogni file

	QMap<QString, QString> fileUri; //stringa finale associata ad un file

	void load_subs();
	void load_files();
	void load_file(TextFile* f);
	void addNewUserToFile(User* user);
	void addNewFile(QString filename, QString user);
	void rewriteUsersFile();
	bool isAuthenticated(QTcpSocket* socket);
	void shareOwnership(QString uri, QTcpSocket* socket);
	void saveAllFilesStatus(); // salva il file All_files.txt
	void requestURI(QString filename, QTcpSocket* sender);
	void writeLog(QString filename, std::shared_ptr<Symbol> s, bool insert);
	bool readFromLog(TextFile* f);
	void deleteLog(TextFile* f);

	QString genRandom();


};
