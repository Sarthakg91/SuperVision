

#include "control.h"
#include<QPainter>
#include<QPoint>
#include<QPen>
#include<QString>
#include<QColor>
#include<QBrush>
#include<QString>
#include<QRect>
#include<QDebug>





Control::Control()
{
    pen=QPen();
    brush=QBrush();
    pos=QPoint();
    text="";
    rx=0;
    ry=0;
    rect=QRect();
    id=-1;
    selected=0;
    parentName="";


}
//---------------------------------------------------------------------------------
// THE SET FUNCTIONS
//----------------------------------------------------------------------------------

void Control::setBrush(QColor color)
{
//qDebug()<<"inside control.setBrush color being set is :"<<color;
//brush.setColor(color);
brush=QBrush(color);
}


void Control::setPen(QColor color)
{
    pen=QPen(color);
}


void  Control::setPos(QPoint position)
{
    pos.setX(position.x());
    pos.setY(position.y());
}


void Control::setText(QString str)
{
    text=str;
}
// to be called only after setPos, so that the rect can be defined
void Control::setRadii(int rx,int ry)
{
    this->rx=rx;
    this->ry=ry;
    rect=QRect(pos.x()-rx,pos.y()-ry,2*rx,2*ry);


}

void Control::setId(int ID )
{
    id= ID;
}

void Control::setParentName(QString name)
{
    parentName=name;
}

// -------------------------------------------------------------------------------------
//THE GET FUNCTIONS
//----------------------------------------------------------------------------



QBrush Control::getBrush()
{
    return brush;
}


QPen Control::getPen()
{
    return pen;
}

QPoint Control::getPos()
{
    return pos;
}

QString Control::getText()
{
    return text;
}


QPoint Control::getRadii()
{
  QPoint radius;
  radius.setX(rx);
  radius.setY(ry);
  return radius;
}
int Control::getId()
{
    return id;
}

QString Control::getParentName()
{
    return parentName;
}

//---------------------------------------------------------------------------------
// IS SELECTED FUNCTION: checks if the recived point lies inside the ellipse
//-------------------------------------------------------------------------------

bool Control::isSelected(QPoint center)
{

    rect=QRect(pos.x()-rx,pos.y()-ry,2*rx,2*ry);
    if(rect.contains(center))
    {   //qDebug()<<" control selected id :"<<id;
        selected=1;

        return true;
    }

    selected=0;
    return false;

}



//---------------------------------------------------------------------------------
//paint function
//----------------------------------------------------------------------------------

void Control::paint(QPainter *painter)
{
    //qDebug()<<"inside paint function of control";

    //qDebug()<<" control position :"<<pos;
    //qDebug()<<"Control color"<<brush;
    //qDebug()<<"text is "<<text;

    painter->setBrush(brush);
    painter->setPen(pen);


    painter->drawEllipse(pos,rx,ry);
    painter->drawText(rect,Qt::AlignCenter,text);



}
