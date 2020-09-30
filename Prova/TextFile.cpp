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

void TextFile::addSymbol(GenericSymbol* newSymbol) {
	int index = symbols.size();  
	if (symbols.size() == 0) {
		index = 0;
	}
	if (symbols.size() == 1) {
		if (symbols[0]->getPosition() > newSymbol->getPosition()) {
			index = 0;
		}
		else {
			index = 1;
		}
	}
	if (symbols.size() > 1) {
		if (newSymbol->getPosition() < symbols[0]->getPosition()) {
			index = 0;
		}
		for (int i = 1; i < symbols.size(); i++) {
			if (symbols[i - 1]->getPosition() < newSymbol->getPosition() && newSymbol->getPosition() < symbols[i]->getPosition()) {
				index = i;
				break;
			}
		}
	}
	symbols.insert(index, newSymbol);
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

