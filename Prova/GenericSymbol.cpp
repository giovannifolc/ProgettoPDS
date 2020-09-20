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

bool GenericSymbol::equals(GenericSymbol* gs)
{
    bool tmp = ((gs->siteId == siteId) && (gs->counter == counter));
    bool equal = true;
    for (int i = 0; i < gs->getPosition().size(); i++) {
        if (!gs->getPosition()[i] == position[i]) {
            equal = false;
        }
    }
    return equal && tmp;
}
