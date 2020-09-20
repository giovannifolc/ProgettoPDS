#include "StyleSymbol.h"

StyleSymbol::StyleSymbol(bool style, QVector<int>& position, int counter, int siteId, bool bold, bool italic, bool underlined, int alignment, int textSize, QColor color, QString font):
	GenericSymbol(style, position, counter, siteId), bold(bold), italic(italic), underlined(underlined), alignment(alignment), textSize(textSize), color(color), font(font) {}

StyleSymbol::~StyleSymbol()
{
}

bool StyleSymbol::isBold()
{
	return bold;
}

bool StyleSymbol::isItalic()
{
	return italic;
}

bool StyleSymbol::isUnderlined()
{
	return underlined;
}

int StyleSymbol::getAlignment()
{
	return alignment;
}

int StyleSymbol::getTextSize()
{
	return textSize;
}

QColor& StyleSymbol::getColor()
{
	return color;
}

QString& StyleSymbol::getFont()
{
	return font;
}

bool StyleSymbol::equals(StyleSymbol* ss)
{
	return ((ss->bold == bold) && (ss->color == color) && (ss->italic == italic) && (ss->underlined == underlined)
		&& (ss->alignment == alignment) && (ss->textSize == textSize) && (ss->font == font));
}
