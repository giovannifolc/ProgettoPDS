#include "TextFile.h"

TextFile::TextFile(QString filename, QString filePath):filename(filename),filePath(filePath)
{
	
}


TextFile::TextFile(QString filename, QString filePath, QTcpSocket* connection): filename(filename), filePath(filePath)
{
	connections.push_back(connection);

}

TextFile::~TextFile() {}

QString TextFile::getFilename()
{
	return filename;
}

QString TextFile::getFilePath()
{
	return filePath;
}

QVector<std::shared_ptr<Symbol>> TextFile::getSymbols()
{
	return symbols;
}

void TextFile::addSymbol(std::shared_ptr<Symbol> newSymbol) {
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

int TextFile::removeSymbol(std::shared_ptr<Symbol> symbol) {
	int index = -1;
	for (int i = 0; i < symbols.size(); i++) {
		if (symbols[i]->getSiteId() == symbol->getSiteId() && symbols[i]->getCounter() == symbol->getCounter()) {
			index = i;
			break;
		}
	}
	if (index != -1) {
		symbols.erase(symbols.begin() + index);
	}
	return index;
}

std::shared_ptr<Symbol> TextFile::getSymbol(int siteId, int counter) {
	int index = -1;
	for (int i = 0; i < symbols.size(); i++) {
		if (siteId == symbols[i]->getSiteId() && symbols[i]->getCounter() == counter) {
			index = i;
			break;
		}
	}
	if (index != -1) {
		return symbols[index];
	}
	else {
		return nullptr;
	}
}

QVector<QTcpSocket*> TextFile::getConnections()
{
	return connections;
}

void TextFile::addConnection(QTcpSocket* connection)
{
	if(!connections.contains(connection))
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

void TextFile::pushBackSymbol(std::shared_ptr<Symbol> sym) {
	
	symbols.push_back(sym);
} 

