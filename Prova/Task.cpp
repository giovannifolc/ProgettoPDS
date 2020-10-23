#include "Task.h"

Task::Task(QVector<std::shared_ptr<Symbol>> symbols, int n_symb, int insert, int siteIdSender): 
	symbols(symbols), n_symb(n_symb), insert(insert), siteIdSender(siteIdSender){
}

Task::Task(QByteArray bufferSymbols): bufferSymbols(bufferSymbols) {
}

Task::Task() {
	QVector<std::shared_ptr<Symbol>> s;
	symbols = s;
	n_symb = 0;
	siteIdSender = -1;
}

QVector<std::shared_ptr<Symbol>> Task::getSymbols() {
	return symbols;
}

int Task::getNSymb() {
	return n_symb;
}

int Task::getInsert() {
	return insert;
}

int Task::getSiteIdSender() {
	return siteIdSender;
}