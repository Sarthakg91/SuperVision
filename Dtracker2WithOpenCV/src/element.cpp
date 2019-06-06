#include "element.h"
#include"control.h"
#include<QPainter>
#include<QDebug>
#include<QRgb>
#include<QPixmap>
#include<qbitmap.h>
#include<qcolor.h>
#include<QPaintDevice>
#include<qpixmap.h>
#include<QTextStream>
#include<QFile>

QPoint Element::temperaturePoint = QPoint();

Element::Element()
{
    name ="";
    boundingQuad=QPolygon();
    selected=0;
    outRect=QRect();
    colorWheelPixMap=QPixmap(":/images/colorWheel.jpg");
    colorWheel=QImage(":/images/colorWheel2.jpg");
    ratio=0;

leastTemp=15;
maxTemp=40;
temperature=leastTemp;

}

Element::~Element()
{

}

void Element::addCorner(QPoint corner)
{
    boundingQuad<<corner;

}
//--------------------------------------------------------------------------
//---------------------ADDCONTROL FUNCTION--------------------------------
/*every element has a vector of controls.
 * this function adds one such control to the vector
 */
//----------------------------------------------------------------------------



void Element::addControl(QPoint position,int rx,int ry,QColor fillColor,QColor penColor,QString text, int Id,QString parentName)
{
    Control control;
    control.setPos(position);
    control.setRadii(rx,ry);
    control.setId(Id);
    control.setPen(penColor);
    //qDebug()<<"in front of setBrush fillcolor is "<<fillColor;
    control.setBrush(fillColor);
    control.setText(text);
    control.setParentName(parentName);
   controls.push_back(control);
interfaceLoaded=0;
}
//-----------------------------------------------------------------------
void Element::clear()
{
    controls.clear();
    boundingQuad.clear();
    name="";
}

//------------------------------------------------------------------------
//ISSelected function-------------------------------------------------------
// checks if the center of spotlight lies inside an element
bool Element::isSelected(QPoint c)
{

    if(boundingQuad.containsPoint(c,Qt::OddEvenFill))
       { selected=1;
        return true;
    }
     selected=0;
     return false;

}

//----------------------------------------------------------------------
//---------------------------PAINT Function---------------------------------
//draws the representing ppolygon using the brush which is already set
//--------------------------------------------------------------------------
void Element::paint(QPainter *painter,QString name)
{
    // paint the bounding polygons if the elements are not selected.
    painter->setBrush(brush);
    painter->drawPolygon(this->boundingQuad);
    QImage temp;
    QColor white=Qt::white;

int incx=0;



    if(selected==1)
    {

    if(name.contains("lamp"))
    {
        //scale the color wheel according to the lamp's outer rectangle
        if(this->outRect.width()>this->outRect.height())
        temp=colorWheel.scaled(this->outRect.width(),this->outRect.width(),Qt::IgnoreAspectRatio);
        else
        {
         temp=colorWheel.scaled(this->outRect.height(),this->outRect.height(),Qt::IgnoreAspectRatio);
        }
        // create a mask
      //  QPixmap maskedPixmap=temp.createMaskFromColor(white.rgb(),Qt::MaskInColor);
        //
        //temp.setMask(maskedPixmap);
        //draw the image at the center of the outer polygon
        painter->drawImage( this->outRect.center().x()-temp.width()/2,this->outRect.center().y()-temp.height()/2,temp);
        //qDebug()<<"lamp is there";
    }

    else if(name.contains("heater"))
    {

            //display the stuff for the heater
            //qDebug()<<"name contains heater";
            this->temperaturePoint=this->sliderRect.bottomLeft();
            painter->setPen(Qt::black);
            painter->setFont(QFont("Helvetica",20,5,false));
            painter->drawText(this->outRect,"    Temperature = 23 deg",QTextOption(Qt::AlignVCenter));

            painter->drawText(outRect,"Temperature = 23 deg");

            QPoint p=this->getOutRect().topLeft()+QPoint((2*this->getOutRect().width())/3,0);
            sliderOutRect.setTopLeft(p);
            sliderOutRect.setWidth(this->getOutRect().width()/3);
            sliderOutRect.setHeight(this->getOutRect().height());

            sliderRect.setTopLeft(sliderOutRect.topLeft()+QPoint((3*sliderOutRect.width())/8,this->getOutRect().height()/10));
            sliderRect.setWidth(this->sliderOutRect.width()/10);
            sliderRect.setHeight((this->sliderOutRect.height()*8)/10);
            painter->setBrush(Qt::white);
            painter->drawRect(sliderOutRect);
            painter->setBrush(Qt::blue);
            painter->drawRect(sliderRect);
            //qDebug()<<"temperature - leastTemp"<<(temperature-leastTemp);
           p=this->sliderRect.bottomLeft()-((this->sliderRect.topLeft()-this->sliderRect.bottomLeft())*(temperature-leastTemp))/(maxTemp-leastTemp);
          // p=QPoint(this->sliderRect.bottomLeft());

            QPen pen=QPen(Qt::green);
            pen.setWidth(10);
            QPoint newPoint= this->sliderRect.bottomLeft();
            newPoint-=ratio*(this->sliderRect.bottomLeft()-this->sliderRect.topLeft());

            painter->setPen(pen);
            painter->drawLine(newPoint,newPoint+QPoint(this->sliderRect.width(),0));


        }

    else   if(name.contains("tv")&&interfaceLoaded==1)
    {

        int height=this->getOutRect().height();
        int inc=0;
        int incY;
        int centerX;int centerY;
        for(int i=0;i<this->channels.size();i++)
        {
            if(i<5)
            {
                //display the channels on the sides

                QPoint bottomLeft=this->getOutRect().bottomLeft();
                int width=this->getOutRect().width()/10;
                centerY=bottomLeft.y()-(int)inc;
                inc+=50;
                centerX=bottomLeft.x()-width/2;
                this->channels[i].locationOfCenter=QPoint(centerX,centerY);
                //this->channels[i].setIconHeight(height);
                this->channels[i].setOutRectangle();


            }
            int width=this->getOutRect().width();

             if(i<13&&i>=5)
            {
                QPoint topLeft=this->getOutRect().topLeft();
                centerX=topLeft.x()+(int)incx;
                incx+=80;
                centerY=topLeft.y()-10;
                this->channels[i].locationOfCenter=QPoint(centerX,centerY);
              //  this->channels[i].setIconWidth(width);
                this->channels[i].setOutRectangle();


            }
           //qDebug()<<"Image being drawin is :"<<this->channels[i].name;
             //qDebug()<<"Image being drawin is :"<<this->channels[i].icon.size()<<" the location of center is :"<<this->channels[i].locationOfCenter;

         if(i>=13)
         {
             QPoint topRight=this->getOutRect().topRight();
             centerY=topRight.y()+(int)incY;
             incY+=80;
             centerX=topRight.x()+50;
             this->channels[i].locationOfCenter=QPoint(centerX,centerY);
           //  this->channels[i].setIconWidth(width);
             this->channels[i].setOutRectangle();
         }

            painter->drawImage(this->channels[i].outRect.topLeft(),this->channels[i].icon);

        }

}


    }


    }





//    // if an element is selected, then display its controls
//    if(selected==1)
//    {
//     for(int i=0;i<controls.size();i++)
//        {
//          controls[i].paint(painter);

//        }

//    }

//default case





void Element::setBrush(QColor color)
{
    brush=QBrush(color);

}

QBrush Element::getBrush()
{
    return brush;
}

void Element::setOutRect()
{ int minX=10000;
    int maxX=-10000;
    int minY=10000;
    int maxY=-10000;
    // find the bigges rectangle enclosing the element
    for(int i=0;i<4;i++)
    {
        if(boundingQuad[i].x()<minX)
            minX=boundingQuad[i].x();
        if(boundingQuad[i].x()>maxX)
            maxX=boundingQuad[i].x();
        if(boundingQuad[i].y()<minY)
            minY=boundingQuad[i].y();
        if(boundingQuad[i].y()>maxY)
            maxY=boundingQuad[i].y();

    }
    outRect=QRect(QPoint(minX,minY),QPoint(maxX,maxY));
    //qDebug()<<"outRect is :"<<outRect;

}

QRect Element::getOutRect()
{
    return outRect;
}

void Element::loadInterfaces()
{
   //qDebug()<<"Inside Load Interfaces";
    // first load the image of the color wheel

   // colorWheelPixMap=colorWheel;


    //try to make the color wheel transparent in the white areas

            //QImage masked=colorWheel.createMaskFromColor(white.rgb(),Qt::MaskOutColor);
             //brickPixmap=brickPixmap.fromImage(bricks,Qt::AutoColor);

           //  colorWheelPixMap=colorWheelPixMap.fromImage(colorWheel,Qt::AutoColor);

//    // render the colorWheel image transparent in the white parts


   QString myFileName="C:/Users/Sarthak/Documents/QtProjects/frenchChannels.txt";
    QFile *file=new QFile(myFileName);
    QTextStream *in=new QTextStream(file);
    file->open(QFile::ReadOnly|QFile::Text);
    Channel channelTemp;
    QString line;
    do{line="";
       line=(*in).readLine();
       //qDebug()<<line;
       if(line!=NULL)
       {

           QString filePath=":/images/"+line;
           int index=line.indexOf('.');
           line.truncate(index);
           //qDebug()<<line;
           channelTemp.name=line;
           QImage image=QImage(filePath);
           QImage image2=image.scaled(50,50,Qt::IgnoreAspectRatio);
           QPixmap pix=QPixmap(filePath);
           channelTemp.setIconImage(image2);
          // channelTemp.setIconPixMap(pix);
           channelTemp.setOutRectangle();

           channels.append(channelTemp);
channelTemp.clear();

       }
    }while(line!=NULL);

for(int i=0;i<channels.size();i++)
{
    if(channels[i].icon.size()!=QSize(0,0))
       { //qDebug()<<"image loaded";qDebug()<<channels[i].icon;
    }
}
//qDebug()<<channels.size();
file->close();

interfaceLoaded=1;
}

void Element::setTemperature(QPoint point)
{   temperaturePoint=point;

    qDebug()<<"numerator :"<<this->sliderRect.bottomLeft().y()-point.y();
    qDebug()<<"denominator :"<<this->sliderRect.bottomLeft().y()-this->sliderRect.topLeft().y();
     ratio=(float)(this->sliderRect.bottomLeft().y()-point.y())/(float)(this->sliderRect.bottomLeft().y()-this->sliderRect.topLeft().y());
qDebug()<<"ratio is : "<<ratio;
   this->temperature=leastTemp+(int)(ratio*(maxTemp-leastTemp));
   qDebug()<<"the new temperature is "<<this->temperature;


}
