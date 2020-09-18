
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
			//caso per il login
		case 0:
		{
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
		case 3:
		{
			//caso per la modifica credenziali
			QString username, old_password, new_password, nickname;
			in >> username >> old_password >> new_password >> nickname;
			//check su identità
			changeCredentials(username, old_password, new_password, nickname, sender);
			
		}
		default:
			break;
		}
	}
	else {
		//visualizzare errore e chiudere connessione?
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
	out << flag; //ritorno 0 se fallita, 1 se riuscita
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
		out << 1 << -1; //operazione riuscita e termine
	}
	else {
		out << 0 << -1; //operazione fallita e termine
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
		out << filesForUser[username].size();
		
		out << -1; //fine trasmissione
	}
	else {
		out << 0 << -1; //operazione fallita e fine trasmissione
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
		for (int i = 0; i < nRows; i++) {
			QVector<int> pos;
		}
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

	std::cout << "UTENTI CONNESSI TOT :\t" << clients.size() << std::endl;
}

