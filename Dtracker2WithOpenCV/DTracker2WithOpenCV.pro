#-------------------------------------------------
#
# Project created by QtCreator 2014-02-26T19:13:37
#
#-------------------------------------------------

QT       += core gui
QT += serialport
LIBS+= -lws2_32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myproj
TEMPLATE = app


SOURCES += main.cpp\
    DTrackSDK.cpp \
    Lib/DTrackNet.cpp \
    Lib/DTrackParse.cpp \
    Lib/DTrackParser.cpp \
    DTracker.cpp \
    MainWindow.cpp \
    widget.cpp \
    element.cpp \
    control.cpp \
    lamp.cpp \
    heater.cpp \
    channel.cpp \
    opencvtoqt.cpp \
    player.cpp

HEADERS  += \
    DTrackSDK.hpp \
    Lib/DTrackDataTypes.h \
    Lib/DTrackNet.h \
    Lib/DTrackParse.hpp \
    Lib/DTrackParser.hpp \
    DTracker.h \
    MainWindow.h \
    widget.h \
    element.h \
    control.h \
    lamp.h \
    heater.h \
    channel.h \
    opencvtoqt.h \
    player.h

FORMS    += mainwindow.ui \
    widget.ui

RESOURCES += \
    images/icons.qrc \
    images/brickss.qrc \
    images/kitchen.qrc \
    images/maskImage.qrc \
    images/colorLamp.qrc \
    images/channelIcons.qrc

INCLUDEPATH += /usr/local/include/opencv2/
INCLUDEPATH += C:/Programs/opencv24/opencv/build/include
LIBS += -LC:/Programs/opencv24/opencv/build/bin -lopencv_core249 -lopencv_highgui249 -lopencv_imgproc249 -lopencv_video249 -lopencv_calib3d249

