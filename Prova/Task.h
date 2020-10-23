#pragma once
#include "Symbol.h"
class Task
{
public:
	Task(QVector<std::shared_ptr<Symbol>> symbols, int n_symb, int insert, int siteIdSender); //siteIdSender=siteId di chi fa la cancellazione/inserimento 
	Task(QByteArray bufferSymbols);
	Task();
	QVector<std::shared_ptr<Symbol>> getSymbols();
	int getNSymb();
	int getInsert();
	int getSiteIdSender();
	
private:
	QVector<std::shared_ptr<Symbol>> symbols;
	int n_symb;
	int insert;
	int siteIdSender;
	QByteArray bufferSymbols;
};

