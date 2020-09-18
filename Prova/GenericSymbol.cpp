#include "GenericSymbol.h"

GenericSymbol::GenericSymbol(bool style, QVector<int>& position, int counter, int siteId): style(style), position(position), counter(counter), siteId(siteId)
{
}

GenericSymbol::~GenericSymbol()
{
}

bool GenericSymbol::isStyle()
{
    return style;
}

QVector<int>& GenericSymbol::getPosition()
{
    return position;
}

void GenericSymbol::setPosition(QVector<int> position)
{
    this->position = position;
}
