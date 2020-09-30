#pragma once
#include <iostream>
#include <vector>
#include <QtNetwork>
#include <GenericSymbol.h>

class TextFile
{
public:
	TextFile(QString filename);
	TextFile(QString filename, QTcpSocket* connection);
	~TextFile();
	QString getFilename();
	QVector<GenericSymbol*> getSymbols();
	void addSymbol(GenericSymbol* symbol);
	QVector<QTcpSocket*> getConnections();
	void addConnection(QTcpSocket* connection);
	void removeConnection(QTcpSocket* connection);
private:
	QString filename;
	QVector<GenericSymbol*> symbols;
	QVector<QTcpSocket*> connections;
};

