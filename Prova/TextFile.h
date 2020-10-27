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
	void addSymbol(std::shared_ptr<Symbol> symbol);	// add singolo carattere
	void addSymbol(QVector<std::shared_ptr<Symbol>> symbol);	// add a blocco
	std::shared_ptr<Symbol> removeSymbol(int siteId, int counter, QVector<int> pos); // remove singolo carattere
	QVector<std::shared_ptr<Symbol>> removeSymbol(QVector<int> siteIds, QVector<int> counters, QVector<QVector<int>> positions); // remove a blocco
	std::shared_ptr<Symbol> getSymbol(int siteId, int counter);
	QVector<QTcpSocket*> getConnections();
	void addConnection(QTcpSocket* connection);
	void removeConnection(QTcpSocket* connection);
	void pushBackSymbol(std::shared_ptr<Symbol> symbol);
	QFile* getLogFile();
	void openLogFile();
	void closeLogFile();

private:
	QString filename;
	QString filePath;
	QVector<std::shared_ptr<Symbol>> symbols;
	QVector<QTcpSocket*> connections;
	QFile logFile;
};

