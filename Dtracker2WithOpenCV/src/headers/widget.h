#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<qpoint.h>
#include<qtextedit.h>
#include<QString>
#include<qpushbutton.h>
#include <QCheckBox>
#include<QButtonGroup>
#include<qfile.h>
#include<QTextStream>
#include"element.h"
#include<QPainter>


#include<QtSerialPort/QtSerialPort>
#include <QSerialPort>
#include<qpixmap.h>
#include <QPaintEvent>
#include <QColor>
#include<qpen.h>
#include<QBrush>
#include<QRgb>
#include<QSlider>
#include <qtimer.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include"player.h"
#include<QRegion>
class Widget : public QWidget
{
    Q_OBJECT
public:

    QString name;
    QString myFileName;
    QFile *file;
    QTextStream *in;
    explicit Widget(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);
    QPoint convertPoint(QPoint );

  // integrating the part from superman vision

    QSerialPort *xbeePort;

        void initSerialPort();
        Player player;
        QImage img;
        QImage imgOut;
        QImage cam;
        cv::VideoCapture stream;

signals:
  void nameRegistered();

public slots:
   void updateCenter(QPointF );
   void selectElement(QPointF );
   void receiveVideo(QImage);

  //from superMan vision
   void readData();
   void sliderValueChanged();
   void setPressFlag();


private: QPoint center;
    int paintFlag;
    QTextEdit *textEdit;
    QVector<Element>elements;
   QString prevName;
    QPolygon prevPolygon;
    QImage compositeImage;
    int wall_x_min;
    int wall_x_max;
    int wall_y_min;
    int wall_y_max;

    int elementSelected;
    int longpressed;
    int shortpressed;
    int pressFlag;
    QPen pen;
    QBrush brush;

    int mode;// mode 1 = outside room,mode 0 = inside room.
    int state;//state=0 no elements selected,state=1 lamp selected,state=2 :heater selected


  // taken from superman Vision


    unsigned short valueRead;
    int SerialOpened;
    QByteArray dataArray;
    QTimer *timer;

    int byteCount;
    int j;
    unsigned char threeBytes[3];
    int bitCount;

    //from maskImplementation2
    QImage bricks;
    QImage hole;
    QImage holeTemp;
    QImage kitchen;
    QImage compositeImage2;
    QImage colorWheel;
    QImage wallTexture;
    QImage mask2;

   QPixmap brickPixmap;
   QPixmap holePixmap;
   QPixmap kitchenPixmap;
   QPixmap compositePixmap;
   QPixmap wallTexturePixMap;
   QPixmap mask2PixMap;
   QImage compositeImage3;
   QPixmap compositePixMap2;
   QImage mask2Temp;



    QRgb maskColor;
    QBrush whiteBrush;

    QSlider *slider;
    qreal opacity;

    // to register the states of buttons on the remote control
    int button1State;
    int button2State;
    int button3State;
    int button4State;
    QPoint pointReceived;
   void deSelectElements();
   void interact();
   void handleButtonReleaseLamp();
   void calculateQuadrant();
   void calculateAngle();
   int quadrant;
   float angle;
   void colorSelected();
   QColor colorChosen;
QPoint firstClickPoint;
QPoint releasePoint;
void handleButtonReleaseHeater();

};

#endif // WIDGET_H
