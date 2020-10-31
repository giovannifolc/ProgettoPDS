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

#include <chrono>

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
	void saveFile(std::shared_ptr<TextFile> f);
	void onReadyRead();
	void saveIfLast(QString filename);
	//void changeCredentials(QString username, QString old_password, QString new_password, QString nickname, QTcpSocket* receiver);
	void registration(QString username, QString password, QString nickname, QTcpSocket* sender);
	bool login(QString username, QString password, QTcpSocket* sender);
	void sendFiles(QTcpSocket* receiver);
	std::shared_ptr<Symbol> insertSymbol(QString filepath, QTcpSocket* sender, QDataStream* in, int siteId, int counter, QVector<int> pos);
	void sendSymbol(std::shared_ptr<Symbol> symbol, bool insert, QTcpSocket* socket, QDataStream *out);
	void sendFile(QString filename, QString filePath, QTcpSocket* socket, int siteId);
	void sendClient(int siteId, QString nickname, QTcpSocket* socket, bool insert);
	//std::shared_ptr<Symbol> deleteSymbol(QString filename, int siteId, int counter, QVector<int> pos, QTcpSocket* sender);
	QVector<std::shared_ptr<Symbol>> deleteSymbols(QString filepath, QVector<int> siteIds, QVector<int> counters, QVector<QVector<int>> poses, QTcpSocket* sender);

private:
	QTcpServer* server;
	int siteIdCounter = 0; //devo salvarlo da qualche parte in caso di crash?

	QMap<QTcpSocket*, std::shared_ptr<UserConn>> connections;//client connessi

	QMap<QString, std::shared_ptr<TextFile>> files;//file in archivio

	QMap<QString, std::shared_ptr<User>> subs;//utenti iscritti

	QMap<QString, QVector<QString>> filesForUser; //file associati ad ogni utente 

	QMap<QString, QVector<QString>> fileOwnersMap; //utenti associati ad ogni file

	QMap<QString, QString> fileUri; //stringa finale associata ad un file

	void load_subs();
	void load_files();
	void load_file(std::shared_ptr<TextFile> f);
	void addNewUserToFile(std::shared_ptr<User> user);
	void addNewFile(QString filePath, QString user);
	void rewriteUsersFile();
	bool isAuthenticated(QTcpSocket* socket);
	void shareOwnership(QString uri, QTcpSocket* socket);
	void saveAllFilesStatus(); // salva il file All_files.txt
	void saveURIFileStatus(); // salva il file file_uri.txt
	void requestURI(QString filePath, QTcpSocket* sender);
	void eraseFile(QString filename, QString username, QTcpSocket* sender);
	void changeProfile(QString username, QString nickname, QImage image, QTcpSocket* sender);
	void changeProfile(QString username, QString nickname, QTcpSocket* sender);
	void writeLog(QString filePath, std::shared_ptr<Symbol> s, bool insert);
	bool readFromLog(std::shared_ptr<TextFile> f);
	void deleteLog(std::shared_ptr <TextFile> f);
	void cursorPositionChanged(int index, QString filename, QTcpSocket* sender);
	void sendSymbols(int n_sym, QVector<std::shared_ptr<Symbol>> symbols, bool insert, QTcpSocket* socket, QString filename, int siteIdSender);


	QString genRandom();


};
