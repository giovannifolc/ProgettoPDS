#include "TextFile.h"

TextFile::TextFile(QString filename):filename(filename)
{
	
}

TextFile::TextFile(QString filename, QTcpSocket* connection): filename(filename)
{
	connections.push_back(connection);

}

TextFile::~TextFile() {}

QString TextFile::getFilename()
{
	return filename;
}

QVector<GenericSymbol*> TextFile::getSymbols()
{
	return symbols;
}

QVector<QTcpSocket*> TextFile::getConnections()
{
	return connections;
}

void TextFile::addConnection(QTcpSocket* connection)
{
	connections.push_back(connection);
}

void TextFile::removeConnection(QTcpSocket* connection)
{
	connections.erase(
		std::remove_if(connections.begin(), connections.end(),
			[connection](QTcpSocket* c) {
				return c == connection;
			}),
		connections.end());
}

