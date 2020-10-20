#include "TextFile.h"

TextFile::TextFile(QString filename, QString filePath):filename(filename),filePath(filePath)
{
	
}


TextFile::TextFile(QString filename, QString filePath, QTcpSocket* connection): filename(filename), filePath(filePath)
{
	connections.push_back(connection);

}

TextFile::~TextFile() {
	for (int i = 0; i < poolThread.size(); i++) {
		poolThread[i].join();
	}
}

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
	std::lock_guard<std::mutex> ul(mutex);
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

void TextFile::addSymbols(QByteArray bufferSymbols, int n_sym, int siteIdSender, std::vector<QTcpSocket*> clientsConnectedonThisFile, QString filepath) {
	//std::thread t([this, &bufferSymbols, n_sym, siteIdSender, clientsConnectedonThisFile, filepath]() {

		std::lock_guard<std::mutex> ul(mutex);
		//QByteArray ba;
		QVector<std::shared_ptr<Symbol>> symbolsToSend;
		//ba.append(bufferSymbols);
		QDataStream in(bufferSymbols);
		QChar value;
		bool bold, italic, underlined;
		int alignment, textSize;
		QColor color;
		QString font;

		for (int i = 0; i < n_sym; i++)
		{
			int siteId, counter;
			QVector<int> pos;
			in >> siteId >> counter >> pos >> value >> bold >> italic >> underlined >> alignment >> textSize >> color >> font;
			std::shared_ptr<Symbol> newSym;
			 
			Symbol sym(pos, counter, siteId, value, bold, italic, underlined, alignment, textSize, color, font);
			std::shared_ptr<Symbol> symbol = std::make_shared<Symbol>(sym);
			addSymbol(symbol);
			writeLog(filepath, symbol, true);  /// I LOG NON FUNZIONANOOOOOOOOO
			newSym = getSymbol(siteId, counter);

			symbolsToSend.push_back(newSym);
		}

		//mando in out
		for (QTcpSocket* sock : clientsConnectedonThisFile)
		{
			sendSymbols(n_sym, symbolsToSend, 1, sock, filePath, siteIdSender); //false per dire che � una cancellazione
		}
	//});
	//poolThread.push_back(std::move(t));
	return;

}


void TextFile::removeSymbols(QByteArray bufferSymbols, int n_sym, int siteIdSender, std::vector<QTcpSocket*> clientsConnectedonThisFile, QString filepath) {
	//std::thread t([this, bufferSymbols, n_sym, siteIdSender, clientsConnectedonThisFile, filepath, mutex]() {

	std::lock_guard<std::mutex> ul(mutex);
	//QByteArray ba;
	QVector<std::shared_ptr<Symbol>> symbolsToSend;
	//ba.append(bufferSymbols);
	QDataStream in(bufferSymbols);

	for (int i = 0; i < n_sym; i++)
	{
		int siteId, counter;
		QVector<int> pos;
		in >> siteId >> counter >> pos;
		std::shared_ptr<Symbol> newSym;

		newSym = getSymbol(siteId, counter);
		std::shared_ptr<Symbol> sym = getSymbol(siteId, counter);
		removeSymbol(sym);
		writeLog(filepath, sym, false); /// I LOG NON FUNZIONANOOOOOOOOO
		

		symbolsToSend.push_back(newSym);
	}

	//mando in out
	for (QTcpSocket* sock : clientsConnectedonThisFile)
	{
		sendSymbols(n_sym, symbolsToSend, 0, sock, filePath, siteIdSender); //false per dire che � una cancellazione
	}
	//});
	//poolThread.push_back(std::move(t));
	return;

}

void TextFile::sendSymbols(int n_sym, QVector<std::shared_ptr<Symbol>> symbols, bool insert, QTcpSocket* socket, QString filename, int siteIdSender)
{
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	int ins;
	if (socket->state() != QAbstractSocket::ConnectedState)
		return;
	if (insert)
	{
		ins = 1;
	}
	else
	{
		ins = 0;
	}
	out << 3 /*numero operazione (inserimento-cancellazione)*/ << ins;
	if (ins == 0) {
		out << siteIdSender; //nel caso di cancellazione ho bisogno di sapere (per i cursori) chi cancella il carattere, non può essere dedotto dal simbolo
	}
	out << n_sym;
	for (int i = 0; i < n_sym; i++)
	{
		out << symbols[i]->getSiteId() << symbols[i]->getCounter() << symbols[i]->getPosition() << symbols[i]->getValue()
			<< symbols[i]->isBold() << symbols[i]->isItalic() << symbols[i]->isUnderlined() << symbols[i]->getAlignment()
			<< symbols[i]->getTextSize() << symbols[i]->getColor().name() << symbols[i]->getFont();
	}
	socket->write(buf);
	socket->flush();
}

/*void TextFile::deleteSymbol(QString filename, int siteId, int counter, QVector<int> pos, QTcpSocket* sender)
{
	
		
	
}*/

/*void TextFile::insertSymbol(QString filename, QTcpSocket* sender, QDataStream* in, int siteId, int counter, QVector<int> pos)
{
	/*auto tmp = connections.find(sender);
	auto tmpFile = files.find(filename);
	//controlli
	if (tmp != connections.end() && tmp.value()->getSiteId() == siteId && tmp.value()->getFilename() == filename && tmpFile != files.end())
	{
		
		
	}
}*/

void TextFile::writeLog(QString filePath, std::shared_ptr<Symbol> s, bool insert)
{
	QString filename = filePath.split("/")[1];
	QString userFolder = filePath.split("/")[0];

	QString fileLogName = filename + "_log.txt";

	QDir d = QDir::current();

	if (!d.exists(userFolder))
	{
		qWarning() << "Impossibile trovare una cartella associata all'utente!"
			<< "\n"
			<< "Creazione cartella";
		/*
				TODO
				crea cartello o mando messaggio di errore?

		*/
	}

	QFile file(d.filePath(filePath));

	if (file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream stream(&file);

		if (insert)
		{
			stream << 1;
		}
		else
		{
			stream << 0;
		}
		stream << " " << s->getPosition().size() << " ";
		for (int valuePos : s->getPosition())
		{
			stream << valuePos << " ";
		}

		stream << s->getCounter() << " " << s->getSiteId() << " " << s->getValue() << " ";
		if (s->isBold())
		{
			stream << 1 << " ";
		}
		else
		{
			stream << 0 << " ";
		}
		if (s->isItalic())
		{
			stream << 1 << " ";
		}
		else
		{
			stream << 0 << " ";
		}
		if (s->isUnderlined())
		{
			stream << 1 << " ";
		}
		else
		{
			stream << 0 << " ";
		}
		stream << s->getAlignment() << " " << s->getTextSize() << " " << s->getColor().name() << " " << QString::fromStdString(s->getFont().toStdString()) << endl;
	}
	file.close();
}