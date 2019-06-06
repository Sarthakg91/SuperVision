#ifndef HEATER_H
#define HEATER_H
#include "element.h"

class Heater : public Element
{
public:
    Heater();
    void paint(QPainter *);
};

#endif // HEATER_H
