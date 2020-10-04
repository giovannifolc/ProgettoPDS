#pragma once
#include <iostream>
#include <vector>
#include <QtNetwork>
#include <GenericSymbol.h>
#include <TextSymbol.h>

class TextFile
{
public:
	TextFile(QString filename);
	TextFile(QString filename, QTcpSocket* connection);
	~TextFile();
	QString getFilename();
	QVector<std::shared_ptr<GenericSymbol>> getSymbols();
	void addSymbol(std::shared_ptr<GenericSymbol> symbol);
	int removeSymbol(std::shared_ptr<GenericSymbol> symbol);
	std::shared_ptr<GenericSymbol> getSymbol(int siteId, int counter, QVector<int> pos);
	QVector<QTcpSocket*> getConnections();
	void addConnection(QTcpSocket* connection);
	void removeConnection(QTcpSocket* connection);
	void pushBackSymbol(std::shared_ptr<GenericSymbol> symbol);

private:
	QString filename;
	QVector<std::shared_ptr<GenericSymbol>> symbols;
	QVector<QTcpSocket*> connections;
};

