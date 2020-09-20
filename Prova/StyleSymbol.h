#pragma once
#include "GenericSymbol.h"
#include <QtGui>
class StyleSymbol :
    public GenericSymbol
{
public:
    StyleSymbol(bool style, QVector<int>& position, int counter, int siteId, bool bold, bool italic, bool underlined, int alignment,
        int textSize, QColor color, QString font);

    ~StyleSymbol();
    bool isBold();
    bool isItalic();
    bool isUnderlined();
    int getAlignment();
    int getTextSize();

    QColor& getColor();

    QString& getFont();

    bool equals(StyleSymbol* e);

private:
    bool bold;
    bool italic;
    bool underlined;
    int alignment;
    int textSize;
    QColor color;
    QString font;
};