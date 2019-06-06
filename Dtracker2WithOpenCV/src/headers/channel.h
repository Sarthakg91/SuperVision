#ifndef CHANNEL_H
#define CHANNEL_H
#include<QRect>
#include<QImage>
#include<QPixmap>

class Channel
{
public:
    Channel();


   QImage icon;
   QPixmap iconPixMap;
   QString name;
   float ratioOfImage;
   QPoint locationOfCenter;
   void setOutRectangle();
   QRect outRect;

    void setIconWidth(int);
    void setIconHeight(int);
    void setIconImage(QImage image);
    void setIconPixMap(QPixmap pic);
    void clear();



};

#endif // CHANNEL_H
