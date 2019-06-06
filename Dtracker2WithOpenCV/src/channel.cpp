#include "channel.h"
#include<QImage>
#include<QPixmap>

Channel::Channel()
{
    icon=QImage();
    iconPixMap=QPixmap();
    ratioOfImage=1;
    locationOfCenter=QPoint(0,0);
    outRect=QRect();
    name="";

}

void Channel::setOutRectangle()
{
    outRect.setTopLeft(locationOfCenter-QPoint(icon.width()/2,icon.height()/2));
    outRect.setWidth(icon.width());
    outRect.setHeight(icon.height());
}

void Channel::setIconWidth(int w)
{
    QImage iconTemp=icon.scaledToWidth(w);
    icon=iconTemp;


}

void Channel::setIconHeight(int h)
{
    QImage iconTemp=icon.scaledToHeight(h);

    icon=iconTemp;

}
void Channel::setIconImage(QImage image)
{
    icon=image;
    ratioOfImage=icon.height()/icon.width();
}
void Channel::setIconPixMap(QPixmap pic)
{
    iconPixMap=pic;
    ratioOfImage=pic.height()/pic.width();
}

void Channel::clear()
{
    icon=QImage();
    iconPixMap=QPixmap();
    ratioOfImage=1;
    locationOfCenter=QPoint(0,0);
    outRect=QRect();
    name="";
}
