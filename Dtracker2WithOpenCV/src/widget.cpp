#include "widget.h"
//#include<ui_widget.h>
#include<qslider.h>
#include<qpainter.h>
#include <qpen.h>
#include<qbrush.h>
#include<QDebug>
#include<QCheckBox>
#include<QButtonGroup>
#include<QTextStream>
#include<QStringList>
#include<QPolygon>
#include<QImage>
#include "control.h"
//for superman vision


#include<QtSerialPort/QtSerialPort>
#include<QMessageBox>
#include <QSerialPort>
#include<QIODevice>
#include<Qtimer>
#include<QKeyEvent>


#include<qpainter.h>
#include<qbitmap.h>
#include<qcolor.h>
#include<QPaintDevice>
#include<qpixmap.h>

#include"opencvtoqt.h"
#include"player.h"

using namespace cv;




#define IMAGE_WIDTH 2150
#define IMAGE_HEIGHT 1828
int paintcount=1;
int timerCount=0;
Widget::Widget(QWidget *parent) :
    QWidget(parent)
{

    this->resize(1200,900);


    //-----------For displaying name of device you are pointing at---------------


elementSelected=-1;
 myFileName="C:/Users/Sarthak/Documents/QtProjects/leftWallObjects.txt";
 file=new QFile(myFileName);
 in=new QTextStream(file);
 file->open(QFile::ReadOnly|QFile::Text);

 prevPolygon=QPolygon();

QString line;  // to read a line of text
QStringList list;// to break down the line of text using comma separator
Element element;// temporary element
QPoint controlPos;
QColor penColor;
QColor fillColor;
QString text;
int id;
QPoint imagePoint;

button1State=0;
button2State=0;
button3State=0;
button4State=0;
wall_x_min=50;
wall_x_max=2200;
wall_y_min=280;
wall_y_max=2500;
//qDebug()<<" wall_x_min ="<<wall_x_min;

pressFlag=0;
int c=0;
 do{


     line="";
     line=(*in).readLine();// reads one line of text
     if(line!=NULL)
     {
       //qDebug()<<"line read is :"<<line;
       list=line.split(",");
       c++;
      // qDebug()<<"value of c "<<c;

       for(int i=0;i<list.length();i++)
       {
      // qDebug()<<list[i]<<",";

}
       float x= list[0].toFloat();// the first coordinate
       float y=list[1].toFloat();// the second coordinate


       int x1=(int)x;
       int y1=(int)y;


       imagePoint=convertPoint(QPoint(x1,y1));// converting to image coordinate from wall coordinate

       // store the point as a corner of one element, in image coordinates
       element.addCorner(imagePoint);
       //qDebug()<<"after add point";

       // every corner is associated with a control.
       // add the control here depending on which corner it is

       if(list[2]=="Top Right")
       {
           controlPos.setX(imagePoint.x());
           controlPos.setY(imagePoint.y()-100);
           penColor=QColor(Qt::black);
           fillColor=QColor(Qt::yellow);
           text="yellow";
           id=0;
       }

       else if(list[2]=="Top Left")
       {

           controlPos.setX(imagePoint.x());
           controlPos.setY(imagePoint.y()-100);
           penColor=QColor(Qt::black);
           fillColor=QColor(Qt::green);
           text="green";
           id=1;
       }

       else if(list[2]=="Bottom Left")
       {

           controlPos.setX(imagePoint.x());
           controlPos.setY(imagePoint.y()+100);
           penColor=QColor(Qt::black);
           fillColor=QColor(Qt::red);
           text="red";
           id=2;
       }

       else if(list[2]=="Bottom Right")
       {  // qDebug()<<"color set is blue";
           controlPos.setX(imagePoint.x());
           controlPos.setY(imagePoint.y()+100);
           penColor=QColor(Qt::black);
           fillColor=QColor(Qt::blue);
           text="blue";
           id=3;
       }

       element.addControl(controlPos,70,70,fillColor,penColor,text,id,list[3]);


     if (c==4)// 4 lines for each element
      {
          element.name=list[3];// name of current element
          element.setBrush(Qt::yellow);
          element.setOutRect();
          elements.append(element);// dynamically appending a new element to the vector of elements
          //clear the current temporary element

          //qDebug()<<"the element being pushed back is "<<element.name<<" "<<(element.boundingQuad);
          element.clear();

        //qDebug()<<"the last element appended is :"<< elements[elements.size()-1].name<<" ,"<<(elements[elements.size()-1].boundingQuad);

           // clearing c and polygon
           c=0;


       }// end of if c==4
//qDebug()<<"after if c==4";
     }// end of if(line!=NULL)

 }while(line!=NULL);

// displaying the list of elements as read from the file
for(int i=0;i<elements.size();i++)
{

    qDebug()<<elements[i].name;
    qDebug()<<(elements[i].boundingQuad);
//    for(int j=0;j<elements[i].controls.size();j++)
//    {
//       // qDebug()<<" Controls are at "<<elements[i].controls[j].getPos();
//       // qDebug()<<"Control color is :"<<elements[i].controls[j].getBrush();
//    }




}

 pen=QPen(Qt::blue);
 brush=QBrush(Qt::red);

center.setX(IMAGE_WIDTH/2);
center.setY(IMAGE_HEIGHT/2);

compositeImage= QImage(this->size(), QImage::Format_ARGB32_Premultiplied);
compositeImage=compositeImage.scaled(IMAGE_WIDTH,IMAGE_HEIGHT,Qt::IgnoreAspectRatio);

mode=0;// mode 0 implies we start inside the room .



// from superman Vision------------------------------------------------------------------------------------------
// variables to manage the data coming from the serial port-------------------
  valueRead=0;
  j=0;
  bitCount=0;




 // this->resize(1000,800);
 // change com port number to suit your needs-------------------------------
  xbeePort=new QSerialPort("COM8",this);
  SerialOpened=0;
  initSerialPort();


// initialize the two array of two bytes------------------------------------
// set the array size according to the number of bytes of data coming in from the com port
  threeBytes[0]=0;
  threeBytes[1]=0;
  threeBytes[2]=0;
  byteCount=0;

// part taken from maskImplementation2------------------------------------------
//--------------for managing the kitchen visualization------------------------------

  // setup slider
  slider= new QSlider(this);
  slider->setMaximum(1023);
  slider->setMinimum(0);
  slider->setValue(0);
  slider->show();

  // connect the slider signal with a slot
  connect(slider,SIGNAL(valueChanged(int)),this,SLOT(sliderValueChanged()));


  // load the images
  bricks=QImage(":/images/brickwall.jpg");
  hole=QImage(":/images/maskImage.JPG");
  kitchen=QImage(":/images/kitchen.jpg");
  // scale the images
  QImage brickTemp=bricks.scaled(this->width(),this->height(),Qt::IgnoreAspectRatio);
  bricks=brickTemp;
  hole=hole.scaled(this->width(),this->height(),Qt::IgnoreAspectRatio);


  kitchen=kitchen.scaled(IMAGE_WIDTH,IMAGE_HEIGHT,Qt::IgnoreAspectRatio);
  //qDebug()<<kitchen;
  // masking color
  maskColor=qRgb(0,0,0);
  //hole=hole.createMaskFromColor(maskColor);
  brickPixmap=brickPixmap.fromImage(bricks,Qt::AutoColor);
  holePixmap=holePixmap.fromImage(hole,Qt::AutoColor);

  wallTexture=QImage(":/images/wallTextured.jpg");
  mask2=QImage(":/images/maskPic2.png");
  qDebug()<<"wallTexture :"<<wallTexture;
  qDebug()<<"Mask 2 :"<<mask2;
  wallTexturePixMap=QPixmap(":/images/wallTextured.jpg");
  mask2PixMap=QPixmap(":/images/maskPic2.png");
  mask2PixMap=mask2PixMap.scaled(this->size(),Qt::IgnoreAspectRatio);
  wallTexturePixMap=wallTexturePixMap.scaled(this->size(),Qt::IgnoreAspectRatio);
  mask2=mask2.scaled(this->size(),Qt::IgnoreAspectRatio);
  wallTexture=wallTexture.scaled(this->size(),Qt::IgnoreAspectRatio);

whiteBrush=QBrush(Qt::white);
connect(xbeePort,SIGNAL(readyRead()),this,SLOT(readData()));

// first load the image of the color wheel
colorWheel=QImage(":/images/colorWheel.jpg");
state=0;//state=1 when lamp is selected, state =2 when heater is selected.
timer = new QTimer(this);
connect(timer, SIGNAL(timeout()), this, SLOT(setPressFlag()));
timer->setSingleShot(true);
connect(&player, SIGNAL(processedImage(QImage)), this, SLOT(receiveVideo(QImage)));
player.Play();

}// end of constructor








//---------------------------------------------------------------------------------
//-------------------UPDATE CENTER FUNCTION-----------------------------------------
//Update center is a slot for the signal flyStickTtacked, from dtracker.cpp

/*update center is a function which receives the point you are pointing at
 * and updates the center of the image to be drawn.
 * it receives a point everytime the flystick is tracked.
 */

//---------------------------------------------------------------------------------




void Widget::updateCenter(QPointF c)
{
  pointReceived =QPoint((int)(c.x()),(int)(c.y()));
   QPoint point=convertPoint(pointReceived);
   center.setX(point.x());
   center.setY(point.y());
// update only if the last update is over.
   if(paintFlag==0)
    update();
}// end of update center function




//---------------------------------------------------------------------------------------
// -------------------------------PAINT EVENT FUNCTION-----------------------------------
/* calls the paint function in all elements
 * draws the image in updated center
 * depending on the selected status of an element, it displays its controls.
 */
//---------------------------------------------------------------------------------------



void Widget::paintEvent(QPaintEvent *)
{

    QRegion reg(this->width()/2-this->height()/2,this->height()/2-this->height()/2,this->height(),this->height(),QRegion::Ellipse);
    QPainter painter(this);
    painter.setClipRegion(reg);
    if(mode==0||mode==1)
    {

    opacity=1.0;
    paintFlag=1;

    QPainter painter2(&compositeImage);
    painter2.setCompositionMode(QPainter::CompositionMode_Source);
    painter2.fillRect(compositeImage.rect(), Qt::black);// making a white rectangle

    for(int i=0;i<elements.size();i++)
    {

        elements[i].paint(&painter2,elements[i].name);// every element is drawn.but only the polygons are drawn
    }

    //check the pressFlags
    if(pressFlag==1)//pressFlag==1 implies lamp has been selected and there is a long press
    {
     //keep drawing a line from the center of the image to the current point
        painter2.setPen(QPen(QBrush(Qt::black),15,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        painter2.drawLine(elements[elementSelected].getOutRect().center(),center);

        //center is the new center in the image coordinates so the other point has to be changed to image coordinates



    }
    painter2.end();


   //compositeImage=compositeImage.scaled(this->size(),Qt::IgnoreAspectRatio);

  // drawing the compositeImage on to the widget


    if(mode==1)
    {
       //qDebug()<<"inside mode 1";
        painter.drawImage(0,0,wallTexture);
        opacity=1.0-((float)(slider->value()-15.0)/100.0);
        //qDebug()<<opacity;
    }
    painter.setOpacity(opacity);
    painter.drawImage(this->width()/2-center.x(),this->height()/2-center.y(),compositeImage);
   // painter.drawImage(0,0,compositeImage);
   // drawing an ellipse to mark the center of the screen
    painter.setBrush(Qt::black);
    painter.drawEllipse(this->width()/2,this->height()/2,30,30);




    paintFlag=0;
     }// end of mode=0(inside the same room)

    // ----------------------------paint function from superman Vision..(QtwithXbee)--------------------------------

    if(mode==2)
    {


    brickPixmap=brickPixmap.fromImage(bricks,Qt::AutoColor);
    QPixmap brickPixmapTemp=brickPixmap.scaled(this->size(),Qt::IgnoreAspectRatio);
    brickPixmap=brickPixmapTemp;

    wallTexturePixMap=wallTexturePixMap.fromImage(wallTexture,Qt::AutoColor);
    QPixmap wallTexturePixMapTemp=wallTexturePixMap.scaled(this->size(),Qt::IgnoreAspectRatio);
    wallTexturePixMap=wallTexturePixMapTemp;

   // creating a composite image for the first mask to put on the bricks
       QImage bricksTemp=bricks.scaled(this->size(),Qt::IgnoreAspectRatio);
       bricks=bricksTemp;
       compositeImage2= QImage(bricks.size(), QImage::Format_ARGB32_Premultiplied);
       QPainter painter3(&compositeImage2);

       painter3.setCompositionMode(QPainter::CompositionMode_Source);
       painter3.fillRect(compositeImage.rect(), whiteBrush);// making a white rectangle
       painter3.setCompositionMode(QPainter::CompositionMode_SourceOver);

       painter3.drawImage(this->width()/2-holeTemp.width()/2, this->height()/2-holeTemp.height()/2, holeTemp);// drawing the hole image on the white rectangle
       painter3.end();// image complete
       //compositeImage=compositeImage.scaled(400,400,Qt::IgnoreAspectRatio);
       compositePixmap=compositePixmap.fromImage(compositeImage2,Qt::AutoColor);// change from Qimage to Qpixmap

       compositePixmap=compositePixmap.createMaskFromColor(maskColor);// create a mask from the composite image
       brickPixmap.setMask(compositePixmap);// apply the mask on top of the brickpixmap image

// creating a composite image for the 2nd mask to put on the wall texture
       compositeImage3=QImage(bricks.size(),QImage::Format_ARGB32_Premultiplied);
       QPainter painter4(&compositeImage3);
       painter4.setCompositionMode(QPainter::CompositionMode_Source);
       painter4.fillRect(compositeImage3.rect(), whiteBrush);
       painter4.setCompositionMode(QPainter::CompositionMode_SourceOver);
       painter4.drawImage(bricks.width()/2-mask2Temp.width()/2,bricks.height()/2-mask2Temp.height()/2,mask2Temp);

       painter4.end();
       compositePixMap2=compositePixMap2.fromImage(compositeImage3,Qt::AutoColor);
       compositePixMap2=compositePixMap2.createMaskFromColor(maskColor);
       wallTexturePixMap.setMask(compositePixMap2);



     //  painter.drawImage(this->width()/2-center.x(),this->height()/2-center.y(),cam);
        painter.drawImage(0,0,cam);
      // painter.drawImage(0,0,kitchen);

        painter.drawPixmap(0,0,brickPixmap);// display the masked image
        painter.drawPixmap(0,0,wallTexturePixMap);
}


}// end of paint event




//----------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//----------------------- CONVERT POINT FUNCTION------------------------------------------------------
/*function to convert the point received from wall coordinate
 * to image coordinates
 * returns the image coordinates
 */
//-----------------------------------------------------------------------------------------------------




QPoint Widget::convertPoint(QPoint c)
{
    int x=(IMAGE_WIDTH *(((-1)*c.x())-wall_x_min))/(wall_x_max-wall_x_min);
  //  qDebug()<<" the value of x as calculated is :"<<x;

    int y=IMAGE_HEIGHT+((((-1)*IMAGE_HEIGHT)*((int)c.y()-wall_y_min))/(wall_y_max-wall_y_min));

       QPoint point;
       point=QPoint(x,y);

       return point;
}// end of convertPoint





//---------------------------------------------------------------------------------
//---------------------SELECT ELEMENT FUNCTION-------------------------------------------
/* SLOT for ButtonReleased signal from controller
 * checks if the button has been released while pointing at a known Object
 * sets one object active and all others inactive
 */




void Widget::selectElement(QPointF c)
{
    //qDebug()<<"inside select element";


    QPoint point;
    point=QPoint((int)c.x(),(int)c.y());
    point=convertPoint(point);
    int i;
    int controlId=-1;

//    if(elementSelected!=-1)
//    {// means an item has been selected
//    /* // so have to check for its controls and as well as the other elements.
//        for(int j=0;j<elements[elementSelected].controls.size();j++)
//        { qDebug()<<"inside if elementSelected!=-1";
//            if(elements[elementSelected].controls[j].isSelected(point))
//            {   //controlId=elements[elementSelected].controls[j].getId();
//                controlId=j;

//                elements[elementSelected].setBrush(elements[elementSelected].controls[j].getBrush().color());
//                if(paintFlag==0)
//                qDebug()<<"update is being called from selectElement";
//                update();
//                return;// the rest of the code is not executed if a control of the selected element is chosen.
// if a control is not chosen, we look for other elements that may be chosen.
// if neither, then you are pointing at blank wall.
// so elementSelect has to be set to -1
//            }



//        }



//    }
if(elementSelected==-1)
{
    for( i=0;i<elements.size();i++)
      {
        if(elements[i].isSelected(point))
        {
            qDebug()<<"you have selected "<<elements[i].name;
            //qDebug()<<"value of selected :"<<elements[i].selected;
            elementSelected=i;
            if(elements[i].name.contains("lamp"))
                state=1;
            else if(elements[i].name.contains("heater"))
                state=2;

           break;
        }
      }// end of for i

  // once one element is selected, all others are to be deselected
    int j=0;
  // if i does not break out, then all elements have to be deselected
//    if(i<elements.size())
//    {
//       j=i+1;

//    }

//   for( ;j<elements.size();j++)
//   { elements[j].selected=0;
//                 }

//  // checking if any of the elements is selected. otherwise, set elementSelected=-1
//   int k=0;
//   for(;k<elements.size();k++)
//   {
//       if(elements[k].selected==1)
//           break;
//   }
//   if(k==elements.size())
// {      elementSelected=-1;
//       state=0;
//}

}//if elementSelected==-1
   if(paintFlag==0)
      //qDebug()<<"update is being called from selectElement";
      update();
}// end of selectElement()


//----------------------------------------------------------------------------------------------------

//----------------------------- serial Port initialization-------------------------------------------


void Widget::initSerialPort()
{
    // only if you can open the port, then apply the settings, otherwise display error message
    if(xbeePort->open(QIODevice::ReadWrite))
    {
    xbeePort->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections);
    xbeePort->setDataBits(QSerialPort::Data8);
    xbeePort->setParity(QSerialPort::NoParity);
    xbeePort->setStopBits(QSerialPort::OneStop);
    xbeePort->setFlowControl(QSerialPort::NoFlowControl);
    SerialOpened=1;
    }

    else
    {
        xbeePort->close();
                 QMessageBox::critical(this, tr("Error"),
                 tr("Can't configure the serial port: %1,\n"
                 "error code: %2")
                 .arg("COM9").arg(xbeePort->error()));
                 SerialOpened=0;

    }



}
//------------------------------------------------------------------------

// SLOT called everytime there is data to be read-------------------------------

void Widget::readData()
{
    int arrSize=xbeePort->bytesAvailable();
   //qDebug()<<" array size is :"<<arrSize;

j=0;

  while(arrSize>0)
  {
      dataArray.resize(arrSize);
      dataArray=xbeePort->readAll();


      while(j<arrSize)
      {
          unsigned char a=dataArray[j++];
          threeBytes[bitCount++]=a;
          if(bitCount>2)//we are sending three bytes of data from the controller
          {
              //display the two bytes
              //qDebug()<<"upper byte is :"<<(unsigned char)threeBytes[1];
              //qDebug()<<"lower byte is :"<<(unsigned char)threeBytes[0];
              //qDebug()<<"button Pressed is :"<<(unsigned char)threeBytes[2];
              valueRead=0;
              valueRead |=(threeBytes[0]);
              valueRead |=(threeBytes[1]<<8);
              //qDebug()<<"total value :"<<(int)valueRead;
              slider->setValue((int)valueRead);

              // toggling the state of the buttons as reported by the xbee
              switch(threeBytes[2])
              {
              case 1:if(button1State==1)// button is released
                       {
                         if(state==0)//if none of the elements are selected, then call select Element
                         selectElement(pointReceived);
                         else if(state==1)//lamp has already been selected
                          {   //the button release has to be seen if it is after a long press or a short press
                             handleButtonReleaseLamp();

                           }
                         else if(state==2)//heater has already been selected
                          {
                             handleButtonReleaseHeater();
                         }
                         button1State=0;
                         }
                      else//if there is a button press
                        { button1State=1;
                          if(state!=0)
                          {qDebug()<<"interact is being called ";
                            interact();//any one element has been selected, then the click down has to be registered
                         }
                  }

                              break;
              case 2:if(button2State==1)
                  {button2State=0;deSelectElements();}
                    else
                       button2State=1;
                      break;
              case 3:if(button3State==1)
                     { button3State=0;selectElement(pointReceived);}
                    else
                       button3State=1;
                      break;
              case 4:if(button4State==1)
                  { button4State=0;selectElement(pointReceived);}
                    else
                       button4State=1;
                      break;

              }


              bitCount=0;
              //reset the two bytes
              threeBytes[0]=0;
              threeBytes[1]=0;
              threeBytes[2]=0;
          }


      }
      j=0;
      arrSize=xbeePort->bytesAvailable();
  }





}

//--------------------------------------------------------------------------------------------

//-------------------------slider value changed---------------------------------
void Widget::sliderValueChanged()
{  int x=slider->value();
    if(x<=15)
    {
        mode=0;//still inside the roomready to look outside the room
    }

    else if (x<=115)
    {
        mode=1;// start making the image transparent
    }
    else
        mode=2;// begin looking outside
    //qDebug()<<slider->value();

    if (mode==2&&x<=200)
    {
       mask2Temp=mask2.scaled(x,(4/3)*x,Qt::IgnoreAspectRatio);
    }
else{
    holeTemp=hole.scaled(x-200,(4/3)*(x-200),Qt::IgnoreAspectRatio);
    mask2Temp=mask2.scaled(x,(4/3)*x,Qt::IgnoreAspectRatio);
    }

    if(paintFlag==0)
      update();

}
//-------------------------------------------------------------------------------

void Widget::deSelectElements()
{
    for (int i=0;i<elements.size();i++)
    {
        elements[i].selected=0;
        state=0;
        elementSelected=-1;
    }
}

void Widget::interact()// to decide whether the user is pressing inside the selected element for short or long time

{
    qDebug()<<"Inside interact. That means element has been selected, and button has been pressed";
    //now we have to check if the button press is inside the picture for lamp
    //if it is in the picture for lamp, we see if it is a long press or a short press.
    //if it is a short press, then only the state of the lamp is toggled.
    //if it is a long press, then a line has to be drawn form the center of the picture to the current center.
    //once the button is released then the value of the color has to be noted down


        //then check for the point being inside the lamp
        if(elements[elementSelected].isSelected(center))
        {  timerCount=0;
            //check if the press is long or short.
            if(elements[elementSelected].name.contains("lamp"))
            {
                //set the pressFlag to represent a short click by default
                pressFlag=2;
            }

            if(elements[elementSelected].name.contains("heater"))
            {
                //set the pressFlag to represent a short click by default
                pressFlag=4;
            }
//qDebug()<<"inside element SElected";



            firstClickPoint=center;

            timer->start(1000);
           // if(timer->isActive())
             //   qDebug()<<"timer is active";
           // else
             //   qDebug()<<"timer is active";

         }// this is true for all elements. according to the element selected, the correct pressFlag gets set.





 }




void Widget::setPressFlag()//pressFlag=1;lamp long,pressFlag=2;lamp short;pressFlag=3;heater long;pressFlag=4;heater short

{
    //qDebug()<<"inside setPressFlag function";
   if(timerCount==0)
   {

    if(button1State==1)
    {
        if(elements[elementSelected].name.contains("lamp"))
          pressFlag=1;//indicates the button press is long and for lamp
        else if (elements[elementSelected].name.contains("heater"))
          pressFlag=3;//indicates the button press is long and for heater


    }

    if(button1State==0)
    {
        //it was a short press
        if(elements[elementSelected].name.contains("lamp"))
          pressFlag=2;//indicates the button press is short and for lamp
        else if (elements[elementSelected].name.contains("heater"))
          pressFlag=4;//indicates the button press is short and for heater


    }
   }
timerCount=1;
    update();

}

void Widget::handleButtonReleaseLamp()
{
    qDebug()<<"inside handle Button Release Lamp";

    if(pressFlag==1)
    {
        //the button is being released after a long press
        //calculate the angle and set the RGB value
        calculateQuadrant();
        calculateAngle();
        colorSelected();
        pressFlag=0;
    }

    else if(pressFlag==2)
    {
        //the button is being released after a short press
        //toggle the lamp on or off
        pressFlag=0;

    }
}

void Widget::calculateQuadrant()
{
   if(center.x()>=elements[elementSelected].getOutRect().center().x())
   {
       if(center.y()<=elements[elementSelected].getOutRect().center().y())
       {
           quadrant=1;
       }
       else
           quadrant=2;

   }
   else
   {
       if(center.y()<=elements[elementSelected].getOutRect().center().y())
       {
           quadrant=4;
       }
       else
           quadrant=3;

   }

   qDebug()<<"you have selected quadrant "<<quadrant;

}


void Widget::calculateAngle()

{
    if(quadrant==1||quadrant==2)
    {
        if((center.x()-elements[elementSelected].getOutRect().center().x()<=5)&&quadrant==1)
          angle=90;
        else if (center.x()-elements[elementSelected].getOutRect().center().x()<=5)
            angle=-90;
       else
       {
        float tanangle=((float)center.y()-elements[elementSelected].getOutRect().center().y())/((float)center.x()-elements[elementSelected].getOutRect().center().x());
        angle=-1*atan(tanangle)*180/3.14;
        }
       // selectColor(q);
    }
    else if(quadrant==3||quadrant==4)
    {
       if(((elements[elementSelected].getOutRect().center().x()-center.x())<=5)&&quadrant==4)
           angle=90;
       else if(elements[elementSelected].getOutRect().center().x()-center.x()<=5)
           angle=-90;


       else
           {


        float tanangle=((float)center.y()-elements[elementSelected].getOutRect().center().y())/((elements[elementSelected].getOutRect().center().x()-(float)center.x()));
        angle=-1*atan(tanangle)*180/3.14;
    }
}
}
void Widget::colorSelected()
{
    if(quadrant==1)
    {
        if(angle<=15.2&&angle>=0)
        {
            qDebug()<<"color is yellow";
            colorChosen.setRgb(255,254,52);
            qDebug()<<colorChosen;
        }
         else if(angle<=45.2&&angle>15.2)
        {
            qDebug()<<"color is chrome";
            colorChosen.setRgb(249,188,2);
            qDebug()<<colorChosen;

        }
        else if (angle <=75.2&&angle>45.2)
         {
            qDebug()<<"color is light orange";
            colorChosen.setRgb(251,153,2);
            qDebug()<<colorChosen;

         }
        else if(angle>75.2&&angle<=90)
        {
            qDebug()<<"color is orange";
            colorChosen.setRgb(253,83,8);
            qDebug()<<colorChosen;
        }


    }

    else if (quadrant==2)
    {
        if(angle<=0&&angle>=-15.2)
        {
            colorChosen.setRgb(255,254,52);
            qDebug()<<"color is yellow";
        }
         else if(angle>=-45.2&&angle<-15.2)
        {
            colorChosen.setRgb(209,234,44);
            qDebug()<<"color is light green";
        }
        else if (angle <-45.2&&angle>=-75.2)
         {

             colorChosen.setRgb(101,176,49);
            qDebug()<<"color is dark green";

         }
        else if(angle>=-90&&angle<-75.2)
        {

            colorChosen.setRgb(2,145,205);

            qDebug()<<"color is light blue";
        }
    }

    else if (quadrant==3)
    {
        if(angle<=0&&angle>=-15.2)
        {

             colorChosen.setRgb(135,1,176);
            qDebug()<<"color is purple";
        }
         else if(angle>=-45.2&&angle<-15.2)
        {
            colorChosen.setRgb(61,0,165);
            qDebug()<<"color is ultramarine blue";
        }
        else if (angle <-45.2&&angle>=-75.2)

         {


            colorChosen.setRgb(1,71,254);
            qDebug()<<"color is cobalt blue";

         }
        else if(angle>=-90&&angle<-75.2)
        {
            qDebug()<<"color is light blue";
             colorChosen.setRgb(2,145,205);
        }
    }

    if(quadrant==4)
    {
         if(angle<=15.2&&angle>=0)
        {
           colorChosen.setRgb(135,1,176);
            qDebug()<<"color is purple";
        }
         else if(angle<=45.2&&angle>15.2)
        {
            colorChosen.setRgb(166,26,75);
            qDebug()<<"color is maroon";
        }
        else if (angle <=75.2&&angle>45.2)
         {
            colorChosen.setRgb(254,39,19);
            qDebug()<<"color is red";

         }
       else if(angle>75.2&&angle<=90)
        {
           colorChosen.setRgb(253,83,8);
            qDebug()<<"color is orange";
        }


    }

    update();
}

void Widget::handleButtonReleaseHeater()
{
    qDebug()<<"inside handle Button Release Heater";
    releasePoint=center;

    qDebug()<<"first Click point is :"<<firstClickPoint;
    qDebug()<<"the release point is :"<<releasePoint;
    if(pressFlag==3)//long press for heater
    {
        if(firstClickPoint.x()<elements[elementSelected].sliderRect.bottomLeft().x()&&releasePoint.x()>elements[elementSelected].sliderRect.bottomRight().x())
        {
            if(releasePoint.y()>=elements[elementSelected].sliderRect.topRight().y()&&releasePoint.y()<=elements[elementSelected].sliderRect.bottomRight().y())
             {   QPoint p=QPoint(elements[elementSelected].sliderRect.bottomLeft().x(),releasePoint.y());
                qDebug()<<"set Temperature being called at point :"<<p;
                elements[elementSelected].setTemperature(p);
              }
        }
    }
    else if(pressFlag==4)
    {
        //toggle the heater on or off
    }
    pressFlag=0;
    update();
}

void Widget::receiveVideo(QImage _cam){
   // this->cam = _cam.scaledToWidth(width());
    this->cam=_cam.scaled(this->size(),Qt::IgnoreAspectRatio);
   if(paintFlag==0)
    update();

}
