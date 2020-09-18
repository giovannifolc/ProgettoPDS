#pragma once
#include "GenericSymbol.h"
class TextSymbol :
    public GenericSymbol
{
public:
    TextSymbol(bool style, QVector<int>& position, int counter, int siteId, QChar value);
    ~TextSymbol();

    QChar getValue();
private:
    QChar value;
};

