
#include "Server.h"
#include "UserConn.h"
#include "User.h"
Server::~Server() {}

void Server::onDisconnected()
{
}

void Server::onReadyRead()
{
	QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
	auto myClient = clients.find(sender);
	//se esiste nel nostro elenco di client connessi riceviamo, altrimenti no
	if (myClient != clients.end()) {
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
			sendFiles(username, sender, success);
			
		}
		case 1: {
			//caso per la registrazione
			QString username, password, nickname;
			in >> username >> password >> nickname;
			registration(username, password, nickname, sender);
		}
		case 2:
		{
			//caso per la modifica credenziali
			QString username, old_password, new_password, nickname;
			in >> username >> old_password >> new_password >> nickname;
			//check su identità
			changeCredentials(username, old_password, new_password, nickname, sender);
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
			//voglio rispondere con qualcosa? TODO
		}
		default:
			break;
		}
	}
	else {
		//visualizzare errore e chiudere connessione?
	}
}

void Server::insertSymbol( QString filename, QTcpSocket* sender, QDataStream* in) {
	auto tmp = clients.find(sender);
	auto tmpFile = files.find(filename);
	int siteId, counter, style;
	QVector<int> pos;
	*in >> siteId >> counter >> pos >> style;
	//controlli
	if (tmp != clients.end() && tmp.value()->getSiteId() == siteId && tmp.value()->getFilename() == filename && tmpFile != files.end()) {
		QVector<GenericSymbol*> vect = tmpFile.value()->getSymbols();
		int index = 0; //inizializzo posizione in cui inserire
		if (vect.size() == 1) {
			if (generateDecimal(vect[0]->getPosition()) > generateDecimal(pos)) {
				index = 0;
			}
			else {
				index = 1;
			}
		}
		else {
			for (int i = 1; i < vect.size(); i++) {
				if (generateDecimal(vect[i-1]->getPosition()) < generateDecimal(pos) && generateDecimal(pos) > generateDecimal(vect[i]->getPosition())) {
					break;
				}
			}
		}
		if (style == 1) {
			int bold, italic, underlined, alignment, textSize;
			QColor color;
			QString colorName, font;
			color.setNamedColor(colorName);
			*in >> bold >> italic >> underlined >> alignment >> textSize >> colorName >> font;
			StyleSymbol* ss = new StyleSymbol((style == 1), pos, counter, siteId, (bold == 1),
				(italic == 1), (underlined == 1), alignment, textSize, color, font);
			vect.insert(index, ss);
		}
		else {
			QChar value;
			*in >> value;
			TextSymbol* ts = new TextSymbol((style == 1), pos, counter, siteId, value);
			vect.insert(index, ts);
		}
	}

}

double generateDecimal(QVector<int> pos) {
	double start = 0;
	for (int i = 0; i < pos.size(); i++) {
		start += pos[i] * pow(10, -(i + 1));
	}
	return start;
}

void Server::deleteSymbol(QString filename, int siteId, int counter, QVector<int> pos, QTcpSocket* sender) {
	auto tmp = clients.find(sender);
	auto tmpFile = files.find(filename);
	//controlli
	if (tmp != clients.end() && tmp.value()->getSiteId() == siteId && tmp.value()->getFilename() == filename && tmpFile != files.end()) {
		QVector<GenericSymbol*> vect = tmpFile.value()->getSymbols();
		int index = -1; //inizializzo posizione in cui ho trovato un match
		for (int i = 0; i < vect.size(); i++) {
			bool found = true;
			for (int j = 0; j < vect[i]->getPosition().size(); j++){
				if (vect[i]->getPosition()[j] != pos[j]) {
					found = false;
				}
			}
			if (found == true) {
				index = i;
				break;
			}
		}
		if (index != -1) {
			vect.erase(vect.begin() + index);
		}
	}
}

void Server::changeCredentials(QString username, QString old_password, QString new_password, QString nickname, QTcpSocket* receiver) {
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	int flag = 1;
	UserConn* tmp = clients.find(receiver).value();
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
}

void Server::registration(QString username, QString password, QString nickname, QTcpSocket* sender) {
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	if (!subs.contains(username)) {
		User* user = new User(username, password, nickname, siteIdCounter++);
		UserConn* conn = new UserConn(username, password, nickname, user->getSiteId(), sender, QString(""));
		subs.insert(username, user);
		clients.insert(sender, conn);
		out << user->getSiteId() << -1; //operazione riuscita e termine
	}
	else {
		out << -1; //operazione fallita e termine
	}
	sender->write(buf);	
}

void Server::sendFiles(QString username, QTcpSocket* receiver, bool success){
	QVector<QString> tmp = filesForUser[username];
	QByteArray buf;
	QDataStream out(&buf, QIODevice::WriteOnly);
	out << 0;//invio codice operazione
	
	if (success) {
		out << 1; //operazione riuscita
		
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
			out << 0; //mando 0, ovvero la quantità di nomi di file in arrivo
		}
		
		out << -1; //fine trasmissione
	}
	else {
		out << -1; //operazione fallita e fine trasmissione
	}
	receiver->write(buf);
}

bool Server::login(QString username, QString password, QTcpSocket* sender) {
	auto tmp = subs.find(username);
	if (tmp != subs.end()) {
		QString pwd = tmp.value()->getPassword();
		if (pwd == password) {
			UserConn* conn = clients.find(sender).value();
			conn->setUsername(username);
			conn->setPassword(password);
			conn->setNickname(tmp.value()->getNickname());
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void Server::load_subs()
{
	QFile fin("subscribers.txt");
	QString username, pwd, nickname;
	int id;

	std::cout << "Loading subscription...\n";

	if (fin.open(QIODevice::ReadOnly)) {
		QTextStream in(&fin);
		while (!in.atEnd())
		{
			in >> username >> pwd >> nickname >> id;
			User* user = new User(username, pwd, nickname, id);
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

	if (fin.open(QIODevice::ReadOnly)) {
		QTextStream in(&fin);
		while (!in.atEnd())
		{	//stile file: nome_file owner1 owner2 ... per ogni riga
			QString line = in.readLine();
			QStringList words = line.split(" ");
			for (auto str : words) {
				if (str != words[0]) {
					filesForUser[str].append(words[0]);
				}
			}
			TextFile* f = new TextFile(words[0]);
			load_file(f);
			files.insert(filename, f);
		}
		fin.close();
	}
	else std::cout << "File 'all_files.txt' not opened" << std::endl;
}

void Server::load_file(TextFile* f)
{
	int nRows;
	QFile fin(f->getFilename());
	if (fin.open(QIODevice::ReadOnly)) {
		QTextStream in(&fin);
		in >> nRows;
		TextFile* tf;
		for (int i = 0; i < nRows; i++) {
			int siteId, counter, style, pos;
			in >> siteId >> counter >> style >> pos;
			QVector<int> vect;
			vect.push_back(pos);
			if (style == 1) {
				int bold, italic, underlined, alignment, textSize;
				QString colorName;
				QString font;
				in >> bold >> italic >> underlined >> alignment >> textSize >> colorName >> font;
				QColor color; 
				color.setNamedColor(colorName);
				StyleSymbol* ss = new StyleSymbol((style == 1), vect, counter, siteId, (bold==1), (italic==1), (underlined==1), alignment, textSize, color, font);
				tf->getSymbols().push_back(ss);
			}
			else {
				QChar value;
				in >> value;
				QVector<int> vectPos;
				vectPos.push_back(pos);
				TextSymbol* ts = new TextSymbol((style==1), vectPos, counter, siteId, value);
				tf->getSymbols().push_back(ts);
			}
		}
		fin.close();
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
	
	connect(socket, SIGNAL(disconnected()), SLOT(onDisconnected()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

	//addConnection
	UserConn* connection = new UserConn("", "", "", -1, socket, "");//usr,pwd,nickname,siteId,socket,filename
	clients.insert(socket, connection);//mappa client connessi

	std::cout << "# of connected users :\t" << clients.size() << std::endl;
}

