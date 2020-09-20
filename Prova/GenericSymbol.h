#pragma once

#include <qvector.h>

class GenericSymbol
{
public:
	GenericSymbol(bool isStyle, QVector<int>& position, int counter, int siteId);
	virtual ~GenericSymbol();
	bool isStyle();
	QVector<int>& getPosition();
	void setPosition(QVector<int> position);
	bool equals(GenericSymbol *gs);
protected:
	bool style;
	QVector<int> position;
	int counter;
	int siteId;
};

