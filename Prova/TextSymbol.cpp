#include "TextSymbol.h"

TextSymbol::TextSymbol(bool style, QVector<int>& position, int counter, int siteId, QChar value): GenericSymbol(style, position, counter, siteId), value(value) {}

TextSymbol::~TextSymbol() {}

QChar TextSymbol::getValue()
{
	return value;
}
