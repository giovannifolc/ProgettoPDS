#pragma once

#include <qvector.h>
#include <QtGui>


class Symbol
{
public:
	Symbol(QVector<int>& position, int counter, int siteId, QChar value, bool bold, bool italic, bool underlined, int alignment,
		int textSize, QColor color, QString font);
	virtual ~Symbol();
	QVector<int>& getPosition();
	void setPosition(QVector<int> position);
	int getCounter();
	int getSiteId();
	QChar getValue();
	bool isBold();
	bool isItalic();
	bool isUnderlined();
	int getAlignment();
	int getTextSize();
	QColor& getColor();
	QString& getFont();
	bool equals(Symbol *gs);

private:
	QVector<int> position;
	int counter;
	int siteId;
	QChar value;
	bool bold;
	bool italic;
	bool underlined;
	int alignment;
	int textSize;
	QColor color;
	QString font;
};

