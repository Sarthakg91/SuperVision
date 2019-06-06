#ifndef CONTROL_H
#define CONTROL_H

#include<QPainter>
#include<QPoint>
#include<QPen>
#include<QString>
#include<QColor>
#include<QBrush>
#include<QString>
#include<QRect>

class Control
{
public:
    Control();
    int selected;

    void paint(QPainter *);
    bool isSelected(QPoint );// function to check which control is selected

    void setText(QString );
    void setPen(QColor );
    void setBrush(QColor );
    void setPos(QPoint );
    void setRadii(int, int);
    void setId(int);
    void setParentName(QString name);

    QString getText();
    QPen getPen();
    QBrush getBrush();
    QPoint getPos();
    QPoint getRadii();
    QString getParentName();
    int getId();
 private:
//every control is an ellipse
//so we need the center coordinate, the colors and the text to write inside the ellipse

    QPoint pos;
    QPen pen;
    QBrush brush;
    QString text;

    int id;// a number to identify the control
    int rx;
    int ry;
    QRect rect;// to check if control is selected
    QString parentName;


};

#endif // CONTROL_H
