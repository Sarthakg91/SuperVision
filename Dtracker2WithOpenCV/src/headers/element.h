#ifndef ELEMENT_H
#define ELEMENT_H
#include<control.h>
#include<QString>
#include<QPolygonF>
#include<QVector>
#include<QColor>
#include<QPainter>
#include<qvector.h>
#include<qlist.h>
#include<channel.h>
class Element
{
public:
    Element();
   ~Element();
    QString name;
    QPolygon boundingQuad;
    QVector<Control>controls;
    int selected;
    void paint(QPainter *, QString name);


    void addCorner(QPoint );// function to add a corner to the boundingQuad, and its associated color for control
    void addControl(QPoint ,int ,int, QColor,QColor,QString,int,QString );
    void clear();
    bool isSelected(QPoint );
    void setBrush(QColor );
    QBrush getBrush();
    void setOutRect();
    QRect getOutRect();
   void loadInterfaces();
    QImage colorWheel;
    void setTemperature(QPoint );
    void getTemperature();
    QRect sliderOutRect;
    QRect sliderRect;
    int interfaceLoaded;\
    float ratio;
   static QPoint temperaturePoint;
    QList<Channel >channels;
private:
    QBrush brush;
    QRect outRect;


    QPixmap colorWheelPixMap;
    QLinearGradient linear;
    int temperature;
    int maxTemp;
    int leastTemp;


};

#endif // ELEMENT_H
