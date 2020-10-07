
#include "Server.h"
#include "UserConn.h"
#include "User.h"
#include <iostream>
#include <bcrypt.h>



Server::~Server() {
}

void Server::onDisconnected()
{
	QTcpSocket* socket = static_cast<QTcpSocket*>(QObject::sender());
	QString filename = connections.find(socket).value()->getFilename();
	if (filename.compare("") != 0) { //se c'� un file associato a quella connessione
		TextFile *f = files.find(filename).value();
		if (f->getConnections().size() == 1) { //se ultimo connesso posso togliere dalla memoria il file e salvarlo in un file di testo
			saveFile(f);
		}
		f->removeConnection(socket);//rimozione utente dai connessi al file
		std::cout << "UTENTI CONNESSI A " << filename.toStdString() << ":\t" << f->getConnections().size() << std::endl;
		sendClient(connections.find(socket).value()->getNickname(), socket, false);
	}
	connections.remove(socket);
	std::cout << "UTENTI CONNESSI:\t" << connections.size() << std::endl;
}

void Server::saveFile(TextFile *f) {
	QString filename = f->getFilename();
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&file);
		int pos = 1;
		int size = files.find(filename).value()->getSymbols().size();
		stream << size << endl;
		for (auto s : files.find(filename).value()->getSymbols()) {
			/*if (symbol->isStyle()) {
				stream << 1;
				std::shared_ptr<StyleSymbol> ss = std::dynamic_pointer_cast<StyleSymbol>(symbol);
				stream << ss->isStyle() << pos++ << ss->getCounter() << ss->getSiteId(); 
				if (ss->isBold()) {
					stream << 1;
				}else {
					stream << 0;
				}
				if(ss->isItalic()){
					stream << 1;
				}
				else {
					stream << 0;
				} 
				if (ss->isUnderlined()) {
					stream << 1;
				}
				else {
					stream << 0;
				}
				stream << ss->getAlignment() << ss->getTextSize() << ss->getColor().name() << ss->getFont() << endl;
			}
			else {
				std::shared_ptr<TextSymbol> ts = std::dynamic_pointer_cast<TextSymbol>(symbol);
				stream << 0; //non � stile
				stream << " " << pos++ << " " << ts->getCounter() << " " << ts->getSiteId() << " " << ts->getValue() << endl;
			}*/
			//stream << 1;
			qDebug() << pos + 1 << " " << s->getCounter() << " " << s->getSiteId() << " " << s->getValue() << " " <<
				s->isBold() << " " << s->isItalic() << " " << s->isUnderlined() << " " << s->getAlignment()
				<< " " << s->getTextSize() << " " << s->getColor().name() << " " << QString::fromStdString(s->getFont().toStdString()) << endl;
			
			stream << pos++ << " " << s->getCounter() << " " << s->getSiteId() << " " << s->getValue() << " ";
			if (s->isBold()) {
				stream << 1 << " ";
			}
			else {
				stream << 0 << " ";
			}if (s->isItalic()) {
				stream << 1 << " ";
			}
			else {
				stream << 0 << " ";
			}
			if (s->isUnderlined()) {
				stream << 1 << " ";
			}
			else {
				stream << 0 << " ";
			}
			stream << s->getAlignment() << " " << s->getTextSize() << " " << s->getColor().name() << " " << QString::fromStdString(s->getFont().toStdString()) << endl;
		}
	}
	file.close();
	deleteLog(f);
}

void Server::onReadyRead()
{
	QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
	auto myClient = connections.find(sender);
	//se esiste nel nostro elenco di client connessi riceviamo, altrimenti no
	if (myClient != connections.end()) {
		QDataStream in;
		in.setDevice(myClient.key());
		int operation;
		in >> operation;
		switch (operation)
		{
			
		case 0:
		{	//caso per il login
			QString username, password;
			in >> username >> password;
			bool success = login(username, password, sender);
			//se ho successo ritorno l'elenco di file, altrimenti un messaggio di fail
			/*if(success)
				sendFiles(sender);*/
			break;
			
		}
		case 1: {
			//caso per la registrazione
			QString username, password, nickname;
			in >> username >> password >> nickname;
			registration(username, password, nickname, sender);
			break;
		}
		case 2:
		{
			//caso per la modifica credenziali
			QString username, old_password, new_password, nickname;
			in >> username >> old_password >> new_password >> nickname;
			//check su identit�
			changeCredentials(username, old_password, new_password, nickname, sender);
			break;
		}
		case 3:
		{
			//caso per l'inserimento o rimozione di un simbolo
			int insert;
			QString filename;
			in >> insert >> filename;
			if (insert == 1) {
				insertSymbol(filename, sender, &in);
			}
			else {
				int siteId, counter; 
				QVector<int> pos;
				in >> siteId >> counter >> pos;
				deleteSymbol(filename, siteId, counter, pos, sender);
			}
			break;
			//voglio rispondere con qualcosa? TODO
		}
		case 4:
		{	//richiesta di un file da parte di un client
			QString filename;
			in >> filename;
			sendFile(filename, sender);
			break;
		}
		case 5:
		{
			//segnalazione di disconnessione da un file
			QString filename;
			in >> filename;
			connections.find(sender).value()->setFilename("");

			files.find(filename).value()->removeConnection(sender);//rimozione utente dai connessi al file
			for (auto conn : files.find(filename).value()->getConnections()) {
				sendClient(connections.find(sender).value()->getNickname(), conn, false);
			}
			saveIfLast(filename);
			break;
		}
		case 6:
		{
			sendFiles(sender);
			break;
		}
		case 7: {
			/*
			   Implemento la share ownership
			*/
			QString filename;
			QString username;
			int operation;

			in >> operation >> filename >> username ;


			/*
			
			   Manca il controllo sull'utente che mi chiede la share ownership, deve essere l'owner del file
			
			*/

			if (operation == 1) {
				shareOwnership(filename, sender);
			}
			else if (operation == 2) {
				/*
				removeOwnership
				*/
			}
			else {
				/*
				  ERRORE
				
				*/
			}

			break;
		}
		default:
			break;
		}
	}
	else {
		//visualizzare errore e chiudere connessione?
	}
	sender->flush();
}

void Server::saveIfLast(QString filename) {
	bool salva = true;
	for (auto client : connections) {
		if (client->getFilename() == filename) {
			salva = false;
		}
	}
	if (salva) {
		if (files.find(filename) != files.end()) {
			saveFile(files.find(filename).value());
		}
	}
}

void Server::sendFile(QString filename, QTcpSocket* socket) {
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	
	if (files.contains(filename)) {

		TextFile* tf = files.find(filename).value();

		out << 4 /*# operazione*/ << tf->getSymbols().size(); //mando in numero di simboli in arrivo

		socket->write(buf);
		//socket->flush();
		for (auto s : tf->getSymbols()) {
			sendSymbol(s, true, socket);
		}
		//mando a tutti i client con lo stesso file aperto un avviso che c'� un nuovo connesso
		for (auto conn : tf->getConnections()) {
			sendClient(connections.find(socket).value()->getNickname(), conn, true);
		}
		/*for (auto client : clients) {
			if (client->getFilename() == filename) {
				sendClient(clients.find(socket).value()->getNickname(), client->getSocket());
			}
		}*/
	}
	else {
		//creo un nuovo file
		if (connections.contains(socket)) {
			TextFile* tf = new TextFile(filename, socket);
			files.insert(filename, tf);
			filesForUser[connections.find(socket).value()->getUsername()].append(filename);
			addNewFile(filename, connections.find(socket).value()->getUsername());
		}
	}
	//setto il filename dentro la UserConn corrispondente e dentro il campo connection di un file aggiungo la connessione attuale
	if (connections.contains(socket)) {
		files.find(filename).value()->addConnection(socket);
		connections.find(socket).value()->setFilename(filename);
	}
}

void Server::sendClient(QString nickname, QTcpSocket* socket, bool insert) {
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);

	out << 8 << nickname; //8 lo uso come flag per indicare un nuovo connesso
	if (insert) {
		out << 1; //deve aggiungere la persona
	}
	else {
		out << 0; //deve rimuovere la persona
	}
	socket->write(buf);
	//socket->flush();
}

void Server::insertSymbol(QString filename, QTcpSocket* sender, QDataStream* in) {
	auto tmp = connections.find(sender);
	auto tmpFile = files.find(filename);
	int siteId, counter;
	QChar value;
	bool style;
	QVector<int> pos;
	*in >> siteId >> counter >> pos;
	//controlli
	if (tmp != connections.end() && tmp.value()->getSiteId() == siteId && tmp.value()->getFilename() == filename && tmpFile != files.end()) {
		QChar value;
		bool bold, italic, underlined;
		int alignment, textSize;
		QColor color;
		QString font;
		*in >> value >> bold >> italic >> underlined >> alignment >> textSize >> color >> font;
		Symbol sym(pos, counter, siteId, value, bold, italic, underlined, alignment, textSize, color, font);
		std::shared_ptr<Symbol> symbol = std::make_shared<Symbol>(sym);
		tmpFile.value()->addSymbol(symbol);
		writeLog(filename, symbol, true);
		
		//mando agli altri client con il file aperto
		for (auto client : connections) {
			if (client->getFilename() == filename) {
				sendSymbol(symbol, true, client->getSocket());
			}
		}
	}
}

void Server::sendSymbol(std::shared_ptr<Symbol> symbol, bool insert, QTcpSocket* socket) {
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	int ins;
	if (socket->state() != QAbstractSocket::ConnectedState)	
		return;
	if (insert) {
		ins = 1;
	}
	else {
		ins = 0;
	}
	out << 3 /*numero operazione (inserimento-cancellazione)*/ << ins;
	out << symbol->getPosition() << symbol->getCounter() << symbol->getSiteId() << symbol->getValue()
		<< symbol->isBold() << symbol->isItalic() << symbol->isUnderlined() << symbol->getAlignment() 
		<< symbol->getTextSize() << symbol->getColor().name() << symbol->getFont();
	/*if (symbol->isStyle()) {
		std::shared_ptr<StyleSymbol> ss = std::dynamic_pointer_cast<StyleSymbol>(symbol);
		
		out << ss->isStyle() << ss->getPosition() << ss->getCounter() << ss->getSiteId() << ss->isBold() << ss->isItalic() << ss->isUnderlined()
			<< ss->getAlignment() << ss->getTextSize() << ss->getColor().name() << ss->getFont();
	}
	else {
		//TextSymbol* ts = dynamic_cast<TextSymbol*>(symbol);
		//out << ts->isStyle() << ts->getPosition() << ts->getCounter() << ts->getSiteId() << ts->getValue();
		out << symbol->isStyle() << symbol->getPosition() << symbol->getCounter() << symbol->getSiteId();
	}*/
	socket->write(buf);
	//socket->flush();
}


void Server::deleteSymbol(QString filename, int siteId, int counter, QVector<int> pos, QTcpSocket* sender) {
	auto tmp = connections.find(sender);
	auto tmpFile = files.find(filename);
	//controlli
	if (tmp != connections.end() && tmp.value()->getSiteId() == siteId && tmp.value()->getFilename() == filename && tmpFile != files.end()) {
		std::shared_ptr<Symbol> sym = tmpFile.value()->getSymbol(siteId, counter, pos);
		tmpFile.value()->removeSymbol(sym);

		writeLog(filename, sym, false);
		//inoltro la cancellazione agli altri client interessati
		for (auto client : connections) {
			if (client->getFilename() == filename) {
				sendSymbol(sym, false, client->getSocket()); //false per dire che � una cancellazione
			}
		}
	}
}

void Server::changeCredentials(QString username, QString old_password, QString new_password, QString nickname, QTcpSocket* receiver) {
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	int flag = 1;
	UserConn* tmp = connections.find(receiver).value();
	if (tmp->getUsername() == username) {
		if (old_password == tmp->getPassword()) {
			//vuole modificare la password
			if (old_password != new_password) {
				tmp->setPassword(new_password);

			}
			//vuole modificare il nickname
			if (tmp->getNickname() != nickname) {
				tmp->setNickname(nickname);
			}
		}
		else {
			flag = 0; //fallita autenticazione
		}
	}
	else {
		flag = 0; //fallita autenticazione
	}
	out << flag << -1; //ritorno 0 se fallita, 1 se riuscita
	receiver->write(buf);
	//receiver->flush();
}

void Server::registration(QString username, QString password, QString nickname, QTcpSocket* sender) {
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	if (!subs.contains(username)) {
		User* user = new User(username, password, nickname, siteIdCounter++);
		UserConn* conn = new UserConn(username, password, nickname, user->getSiteId(), sender, QString(""));
		subs.insert(username, user);
		addNewUser();
		connections.insert(sender, conn);
		out << 1 /*#operazione*/ << 1 /*successo*/ << user->getSiteId(); //operazione riuscita e termine
	}
	else {
		out << 1 /*#operazione*/ << 0; //operazione fallita e termine
	}
	sender->write(buf);	
	//sender->flush();
}

void Server::sendFiles(QTcpSocket* receiver){
	UserConn* conn = connections.find(receiver).value();
	QString username = conn->getUsername();
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	out << 6;//invio codice operazione
	
	if (isAuthenticated(receiver)) { // controllare se � loggato
		out << 1; //operazione riuscita
		QVector<QString> tmp = filesForUser[conn->getUsername()];
		//mando siteId
		out << subs.find(username).value()->getSiteId();
		
		//gestione se non ho file? Questo da nullpointer exception forse
		if (filesForUser.contains(username)) {
			//mando numero di nomi di file in arrivo
			out << filesForUser[username].size();
			//mando i nomi dei file disponibili
			for (auto filename : filesForUser[username]) {
				out << filename;
			}
		}
		else {
			out << 0; //mando 0, ovvero la quantit� di nomi di file in arrivo
		}
	
	}
	else {
		out << 0; //operazione fallita e fine trasmissione
	}
	receiver->write(buf);
	//receiver->flush();
}

bool Server::login(QString username, QString password, QTcpSocket* sender) {
	auto tmp = subs.find(username);
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	out << 0;//invio codice operazione
	if (tmp != subs.end()) {
		QString pwd = tmp.value()->getPassword();
		if (pwd == password) {
			UserConn* conn = connections.find(sender).value();
			conn->setUsername(username);
			conn->setPassword(password);
			conn->setNickname(tmp.value()->getNickname());
			conn->setSiteId(tmp.value()->getSiteId());
			out << 1; //operazione riuscita
			sender->write(buf);
			//sender->flush();
			return true;
		}
		else {
			out << 0;
			sender->write(buf);
			//sender->flush();
			return false;
		}
	}
	else {
		out << 0;
		sender->write(buf);
		//sender->flush();
		return false;
	}
}

void Server::load_subs()
{
	QFile fin("subscribers.txt");
	QString username, password, nickname;
	int siteId;

	std::cout << "Loading subscription...\n";

	if (fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&fin);
		while (!in.atEnd())
		{   
		
			QString line = in.readLine();
			
			username = line.split(" ")[0]; 
			password = line.split(" ")[1];
			nickname = line.split(" ")[2];
			siteId = line.split(" ")[3].toInt();

			User* user = new User(username, password, nickname, siteId);
			subs.insert(username, user);
		}
		fin.close();
		std::cout << "Loaded subscription!\n";
	}
	else 
		std::cout << "File subscribers.txt non aperto" << std::endl;
}

void Server::load_files()
{
	QString filename;
	QFile fin("all_files.txt");
		
	std::cout << "Loading files..\n";

	if (fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&fin);
		while (!in.atEnd())
		{	//stile file: nome_file owner1 owner2 ... per ogni riga
			QString line = in.readLine();
			QStringList words = line.split(" ");
			QVector<QString> utenti;
			filename = words[0];
			for (auto str : words) {

				
				if (str != filename) {
					utenti.append(str);
					filesForUser[str].append(filename);
				}
			}
			fileOwnersMap.insert(filename, utenti);
			
			TextFile* f = new TextFile(filename);			
			load_file(f);
			files.insert(filename, f);
		}
		fin.close();
	}
	else std::cout << "File 'all_files.txt' not opened" << std::endl;
}



/*
   Funzione per permette l'accettazione di un invito a collaborare
*/
void Server::shareOwnership(QString filename, QTcpSocket* sender) {
	

	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	
	UserConn* tmp = connections.find(sender).value();


	fileOwnersMap[filename].append(tmp->getUsername());
	filesForUser[tmp->getUsername()].append(filename);
	saveAllFilesStatus();

	out << 7 << " " << filename;

	sender->write(buf);
	
    
}

/*void Server::requestURI(QString filename, QTcpSocket* sender) {

	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	
	BCryptHash(BCRYPT_SHA256_ALG_HANDLE,)

	std::hash<QString> hash_fn;
	UserConn* tmp = connections.find(sender).value();

	size_t result = hash_fn(filename + tmp->getUsername());

	out << tmp->getUsername() << "/" << filename << "?" << result;

	// Michele/prova?123456789

	sender->write(buf); 


}*/

void Server::load_file(TextFile* f)
{
	int nRows;
	QFile fin(f->getFilename());
	if (fin.open(QIODevice::ReadOnly)) {
		QTextStream in(&fin);
		in >> nRows;
		for (int i = 0; i < nRows; i++) {
			int siteId, counter, pos;
			int bold, italic, underlined, alignment, textSize;
			QString colorName;
			QString font;
			in >> pos >> counter >> siteId;
			QVector<int> vect;
			vect.push_back(pos);
			QChar value;
			in >> value; //salto lo spazio che separa pos da value
			in >> value;
			in >> bold  >> italic >> underlined >> alignment >> textSize >> colorName >> font;
			QColor color;
			color.setNamedColor(colorName);
			Symbol sym(vect, counter, siteId, value, bold == 1, italic == 1, underlined == 1, alignment, textSize, color, font);

			//StyleSymbol sym((style == 1), vect, counter, siteId, (bold==1), (italic==1), (underlined==1), alignment, textSize, color, font);
			f->pushBackSymbol(std::make_shared<Symbol>(sym));
		}
		fin.close();
	}	
	if (readFromLog(f)) {
		qDebug() << "Il file " << f->getFilename() << " � stato ripristinato partendo dal log";
	}
}

Server::Server(QObject* parent) : QObject(parent) 
{
	server = new QTcpServer(this);

	load_subs();
	
	load_files();

	connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);
	server->listen(QHostAddress::Any, 49002);
	if (server->isListening()) {
		std::cout << "Server is listening on port: " << server->serverPort() << std::endl;
	}

}

void Server::onNewConnection() {
	QTcpSocket* socket  = server->nextPendingConnection();
	
	connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
	connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);

	//addConnection
	UserConn* connection = new UserConn("", "", "", -1, socket, "");//usr,pwd,nickname,siteId,socket,filename
	connections.insert(socket, connection);//mappa client connessi

	std::cout << "# of connected users :\t" << connections.size() << std::endl;
}

void Server::addNewUser() {
	QFile file("subscribers.txt");
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream output(&file);
		for (User* u : subs.values()) {
			output << u->getUsername() << " " << u->getPassword() << " " << u->getNickname() << " " << u->getSiteId() << "\n";
		}
		file.close();
	}
	else
		std::cout << "File subscribers.txt non aperto" << std::endl;
}

void Server::addNewFile(QString filename, QString user) {
	QFile file("all_files.txt");
	if (file.open(QIODevice::ReadOnly | QIODevice::Append)) {

		QTextStream output(&file);
	
		output << filename << " " <<user << "\n";
	}

		file.close();
}

bool Server::isAuthenticated(QTcpSocket* socket)
{
	UserConn* conn = connections.find(socket).value();
	if (conn->getUsername() != "") {
		return true;
	}
	else {
		return false;
	}
}

void Server::saveAllFilesStatus() {

	QFile file("all_files.txt");
	if (file.open(QIODevice::WriteOnly)) {

		QTextStream output(&file);
		for (QString filename : fileOwnersMap.keys()) {
			output << filename;
			for (QString utente : fileOwnersMap[filename]) {

				output << " " << utente;

			}

			output << "\n";

		}


	}

	file.close();
}

void Server::writeLog(QString filename, std::shared_ptr<Symbol> s, bool insert) {
	QString fileLogName = filename + "_log.txt";
	QFile file(fileLogName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream stream(&file);

		if (insert) {
			stream << 1;
		}
		else {
			stream << 0;
		}
		stream << " " << s->getPosition().size() << " ";
		for (int valuePos : s->getPosition()) {
			stream << valuePos << " ";
		}

		stream << s->getCounter() << " " << s->getSiteId() << " " << s->getValue() << " ";
		if (s->isBold()) {
			stream << 1 << " ";
		}
		else {
			stream << 0 << " ";
		}if (s->isItalic()) {
			stream << 1 << " ";
		}
		else {
			stream << 0 << " ";
		}
		if (s->isUnderlined()) {
			stream << 1 << " ";
		}
		else {
			stream << 0 << " ";
		}
		stream << s->getAlignment() << " " << s->getTextSize() << " " << s->getColor().name() << " " << QString::fromStdString(s->getFont().toStdString()) << endl;
	}
	file.close();
}

bool Server::readFromLog(TextFile* f) {
	QString fileLogName = f->getFilename() + "_log.txt";
	QFile fin(fileLogName);
	if (fin.open(QIODevice::ReadOnly)) {
		QTextStream in(&fin);
		while (!in.atEnd())
		{
			int insert, sizeVect;
			int siteId, counter, pos;
			int bold, italic, underlined, alignment, textSize;
			QString colorName;
			QString font;
			QVector<int> vect;
			in >> insert >> sizeVect;
			for (int i = 0; i < sizeVect; i++) {
				in >> pos;
				vect.push_back(pos);
			}
			
			in >> counter >> siteId;
			QChar value;
			in >> value; //salto lo spazio che separa pos da value
			in >> value;
			in >> bold >> italic >> underlined >> alignment >> textSize >> colorName >> font;
			QColor color;
			color.setNamedColor(colorName);
			Symbol sym(vect, counter, siteId, value, bold == 1, italic == 1, underlined == 1, alignment, textSize, color, font);
			
			if(insert==1)
				f->addSymbol(std::make_shared<Symbol>(sym));
			else
				f->removeSymbol(std::make_shared<Symbol>(sym));
		}
		fin.close();
		//saveFile(f); //il log viene rimosso nell saveFile
		return true;
	}
	else {
		return false;
	}
}
void Server::deleteLog(TextFile* f) {
	QString fileLogName = f->getFilename() + "_log.txt";
	remove(fileLogName.toStdString().c_str());
}