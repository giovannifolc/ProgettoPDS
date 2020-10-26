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
	int size = symbols.size();
	if (size == 0) {
		index = 0;
	}
	
	if (size == 1) {
		if (symbols[0]->getPosition() > newSymbol->getPosition()) {
			index = 0;
		}
		else {
			index = 1;
		}
	}
	if (size > 1) {
		int flag = 0;
		if (newSymbol->getPosition() < symbols[0]->getPosition()) {
			index = 0;
			flag = 1;
		}
		else if(newSymbol->getPosition() > symbols[size-1]->getPosition()){
			index = size;
			flag = 1;
		}
		int i;
		int dx, sx;
		dx = size -1;
		sx = 0;
		
		while (flag == 0)
		{
			i = (dx + sx) / 2;
			if (symbols[i - 1]->getPosition() < newSymbol->getPosition() && newSymbol->getPosition() < symbols[i]->getPosition()) {
				flag = 1;
				index = i;
			}
			else {
				if (symbols[i - 1]->getPosition() > newSymbol->getPosition()) {// il nostro simbolo ha pos minore del simbolo indicizzato -> andare a sinistra;
					dx = i;
				}
				else {
					sx = i;
				}
			}
		}
		/*for (int i = 1; i < symbols.size(); i++) {
			if (symbols[i - 1]->getPosition() < newSymbol->getPosition() && newSymbol->getPosition() < symbols[i]->getPosition()) {
				index = i;
				break;
			}
		}*/
	}
	symbols.insert(index, newSymbol);
}

/*std::shared_ptr<Symbol> TextFile::removeSymbol(int siteId, int counter, QVector<int> pos) {
	int index = -1;
	for (int i = 0; i < symbols.size(); i++) {
		if (symbols[i]->getSiteId() == siteId && symbols[i]->getCounter() == counter) {
			index = i;
			break;
		}
	}
	if (index != -1) {
		std::shared_ptr<Symbol> sym = symbols[index];
		symbols.erase(symbols.begin() + index);
		return sym;
	}
	
}*/
std::shared_ptr<Symbol> TextFile::removeSymbol(int siteId, int counter, QVector<int> position) {
	int index = this->symbols.size();
	int size = this->symbols.size();
	if (size == 0) {
		return nullptr;
	}

	if (size >= 1) {
		int flag = 0;
		if (position == this->symbols[0]->getPosition()) {
			if (symbols[0]->getSiteId() == siteId && symbols[0]->getCounter() == counter) {
				index = 0;
				flag = 1;
			}
			else {
				return nullptr;
			}
		}
		else if (position == this->symbols[size - 1]->getPosition()) {
			if (symbols[size - 1]->getSiteId() == siteId && symbols[size - 1]->getCounter() == counter) {
				index = size - 1;
				flag = 1;
			}
			else {
				return nullptr;
			}
		}
		int i;
		int dx, sx;
		dx = size - 1;
		sx = 0;

		while (flag == 0)
		{
			i = (dx + sx) / 2;
			if (this->symbols[i]->getPosition() == position) {
				if (symbols[i]->getSiteId() == siteId && symbols[i]->getCounter() == counter) {
					flag = 1;
					index = i;
				}
				else {
					return nullptr;
				}
			}
			else {
				if (this->symbols[i]->getPosition() > position) {// il nostro simbolo ha pos minore del simbolo indicizzato -> andare a sinistra;
					dx = i;
				}
				else {
					sx = i;
				}
			}

		}
	}
	std::shared_ptr<Symbol> sym = symbols[index];
	symbols.remove(index);
	return sym;
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

QFile* TextFile::getLogFile() {
	return &this->logFile;
}

void TextFile::openLogFile() {
	
	QDir d = QDir::current();
	this->logFile.setFileName(d.filePath(filePath.split("/")[0] + "/" + filePath.split("/")[1].split(".")[0] + "_log.txt"));

	if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)){}
}

void TextFile::closeLogFile() {
	logFile.close();
	logFile.remove();
}