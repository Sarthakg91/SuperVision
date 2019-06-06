#include "mainwindow.h"
#include <QApplication>
#include<widget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
   // Widget wid;
  w.show();
    //wid.show();


    return a.exec();
}
