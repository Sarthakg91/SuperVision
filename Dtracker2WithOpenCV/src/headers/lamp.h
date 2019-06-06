#ifndef LAMP_H
#define LAMP_H
#include "element.h"

class Lamp : public Element
{
public:
    Lamp();
    void paint(QPainter *);
};

#endif // LAMP_H
