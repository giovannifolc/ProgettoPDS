#pragma once
#include <iostream>
#include <vector>
#include <QtNetwork>
#include <Symbol.h>

class TextFile
{
public:
	TextFile(QString filename, QString filePath);
	TextFile(QString filename, QString filePath, QTcpSocket* connection);
	~TextFile();
	QString getFilename();
	QString getFilePath();
	QVector<std::shared_ptr<Symbol>> getSymbols();
	void addSymbol(std::shared_ptr<Symbol> symbol);
	//void addSymbolsFromBuffer(QDataStream in, int nSymbols);
	int removeSymbol(std::shared_ptr<Symbol> symbol);
	std::shared_ptr<Symbol> getSymbol(int siteId, int counter);
	QVector<QTcpSocket*> getConnections();
	void addConnection(QTcpSocket* connection);
	void removeConnection(QTcpSocket* connection);
	void pushBackSymbol(std::shared_ptr<Symbol> symbol);
	QVector<std::shared_ptr<Symbol>> addSymbols(QByteArray bufferSymbols, int n_symbols, int siteIdSender, std::vector<QTcpSocket*> clientsConnectedonThisFile, QString filepath);
	QVector<std::shared_ptr<Symbol>> removeSymbols(QByteArray bufferSymbols, int n_symbols, int siteIdSender, std::vector<QTcpSocket*> clientsConnectedonThisFile, QString filepath);


private:
	QString filename;
	QString filePath;
	QVector<std::shared_ptr<Symbol>> symbols;
	QVector<QTcpSocket*> connections;
	std::mutex mutex;
	std::vector<std::thread> poolThread;
	void sendSymbols(int n_sym, QVector<std::shared_ptr<Symbol>> symbols, bool insert, QTcpSocket* socket, QString filename, int siteIdSender);
	void writeLog(QString filePath, std::shared_ptr<Symbol> s, bool insert);
};

