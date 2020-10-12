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
	int removeSymbol(std::shared_ptr<Symbol> symbol);
	std::shared_ptr<Symbol> getSymbol(int siteId, int counter, QVector<int> pos);
	QVector<QTcpSocket*> getConnections();
	void addConnection(QTcpSocket* connection);
	void removeConnection(QTcpSocket* connection);
	void pushBackSymbol(std::shared_ptr<Symbol> symbol);

private:
	QString filename;
	QString filePath;
	QVector<std::shared_ptr<Symbol>> symbols;
	QVector<QTcpSocket*> connections;
};

