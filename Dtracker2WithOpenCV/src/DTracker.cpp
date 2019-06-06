/*
* This class serves to connect the ART tracker and process data in a Qt GUI.
* (c) 
* Derived from "example_with_simple_remote_control" from A.R.T. GmbH.
*
* DTrackSDK: C++ example, A.R.T. GmbH 16.12.05-17.6.13
* C++ example using DTrackSDK with simple DTrack/DTrack2 remote control
* Copyright (C) 2005-2013, Advanced Realtime Tracking GmbH
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Purpose:
*  - example with DTrack/DTrack2 remote commands:
*    starts measurement, collects frames and stops measurement again
*  - using DTrackSDK v2.4.0
*  - tested under Linux (gcc) and MS Windows 2000/XP (MS Visual C++)
*
*/

#include "DTracker.h"
#include "MainWindow.h"
#include "DTrackSDK.hpp"
#include <iostream>
#include <sstream>
#include <QObject>
#include <QBasicTimer>
#include <QTextStream>
using namespace std;

void DTracker::processTrackingData() {
    ostringstream ss;
    ss.precision(3);
    ss.setf(ios::fixed, ios::floatfield);


//	ss << endl << "frame " << dt->getFrameCounter() << " ts " << dt->getTimeStamp()
//		<< " nbod " << dt->getNumBody() << " nfly " << dt->getNumFlyStick()
//		<< " nmea " << dt->getNumMeaTool() << " nmearef " << dt->getNumMeaRef()
//		<< " nhand " << dt->getNumHand() << " nmar " << dt->getNumMarker()
//		<< " nhuman " << dt->getNumHuman()
//		<< endl;

    // bodies:
    DTrack_Body_Type_d body;
    for (int i=0; i<dt->getNumBody(); i++){
        body = *dt->getBody(i);

        if (body.quality < 0) {
            ss << "bod " << body.id << " not tracked" << endl;
        }
        else {
            ss << "bod " << body.id << " qu " << body.quality
            << " loc " << body.loc[0] << " " << body.loc[1] << " " << body.loc[2]
            << " rot " << body.rot[0] << " " << body.rot[1] << " " << body.rot[2]
            << " " << body.rot[3] << " " << body.rot[4] << " " << body.rot[5]
            << " " << body.rot[6] << " " << body.rot[7] << " " << body.rot[8] << endl;
        }




        trans= QMatrix4x4( float(body.rot[0]),float(body.rot[3]),float(body.rot[6]),float (0.0),
                                           float (body.rot[1]) , float(body.rot[4]) ,float(body.rot[7]),float (0.0),
                                           float (body.rot[2]) , float(body.rot[5]),float(body.rot[8]),float (0.0),
                float (0.0),float (0.0),float (0.0),float (1.0));

//    location.setX(float(flystick.loc[0]));
//     location.setY(float(flystick.loc[1]));
//     location.setZ(float(flystick.loc[2]));
       // transformedLocation=trans*location;
transformedLocation=QVector3D(body.rot[6],body.rot[7],body.rot[8]);
        ModulusOfPosition=transformedLocation.length();
        ss<<"The modulus of pointting vector is :"<<ModulusOfPosition;

        directionCosineX=transformedLocation.x()/ModulusOfPosition;
        directionCosineY=transformedLocation.y()/ModulusOfPosition;
        directionCosineZ=transformedLocation.z()/ModulusOfPosition;
        /*
        //for intersection with Z plane

        zLine=(ZPlaneForWall-flystick.loc[2])/directionCosineZ;
        intersectionPoint.setX( qreal((directionCosineX*zLine)+flystick.loc[0]));
        intersectionPoint.setY( qreal((directionCosineY*zLine)+flystick.loc[1]));

        ss<<" the pointing vector is : "<<transformedLocation.x()<<", "<<transformedLocation.y()<<", "<<transformedLocation.z();

        ss<<" The intersection Point for Z='-1500' is "<<intersectionPoint.x()<<", "<<intersectionPoint.y();

        */


        // for intersection with left wall.
        xLine=(xPlaneForLeftWall-body.loc[0])/directionCosineX;
        intersectionPoint.setX( qreal((directionCosineZ*xLine)+body.loc[2]));
        intersectionPoint.setY( qreal((directionCosineY*xLine)+body.loc[1]));
        ss<<" the pointing vector is : "<<transformedLocation.x()<<", "<<transformedLocation.y()<<", "<<transformedLocation.z();

        ss<<" The intersection Point for X='-2000' is "<<intersectionPoint.x()<<", "<<intersectionPoint.y();



 //---------------------------------------------------------------
  //------------------------------------------------------------------------------------------------






//if(flystick.quality>0&&buttonPress==1)
//{  qDebug()<<"flystick button pressed signal emitted";
//    emit(flyStickButtonPressed(intersectionPoint));
//    prevButtonState=1;
//}

// the image is always updated in widget.cpp

// emit signal if the body is tracked. the intersection point is always emitted
if(body.quality>0)
 emit(bodyTracked(intersectionPoint));// handled by updateCenter in widget.cpp


// emit signal if a flystick is tracked and if button is released
// this means element is to be selected.


//if(flystick.quality>0&&prevButtonState==1&&buttonPress==0)
//{
//    qDebug()<<"buttonRelease signal emitted";

//    emit(flyStickButtonReleased(intersectionPoint));// Slot : selectElement in widget.cpp
//    prevButtonState=0;buttonPress=0;
//}



        ss << endl;
    }



    sendMessage(ss.str());
}


bool DTracker::processErrors()
{
    if (dt->getLastDataError() == DTrackSDK::ERR_TIMEOUT) {
        sendMessage("! Timeout while waiting for tracking data");
        return false;
    }

    if (dt->getLastDataError() == DTrackSDK::ERR_NET) {
        sendMessage("! Error while receiving tracking data");
        return false;
    }

    if (dt->getLastDataError() == DTrackSDK::ERR_PARSE){
        sendMessage("! Error while parsing tracking data");
        return false;
    }

    return true;
}


void DTracker::processMessages()
{
    while (dt->getMessage()) {
        string msg = "ATC message: '";
        msg += dt->getMessageStatus() + "' '" + dt->getMessageMsg() + "'";
        sendMessage(msg);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DTracker::sendMessage(const string& txt) {
    //cout << "Msg: " << txt <<endl;
    emit onMessage(QString(txt.c_str()));
}

void DTracker::sendState(int state) {
    //cout << "State: " << state <<endl;
    emit onStateChange(state);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DTracker::DTracker(const string& hostname, int port, int rport) :
    dt(NULL),
    hostname(hostname),
    port(port),
    rport(rport),
    state(Disconnected)
{

    qDebug()<<"Inside DTracker constructor";
      location=  QVector3D(0.0,0.0,1.0);
        trans=   QMatrix4x4(1.0, 0.0, 0.0, 0.0,
                                 0.0, 1.0, 0.0, 0.0,
                                 0.0, 0.0, 1.0, 0.0,
                                 0.0, 0.0, 0.0, 1.0);

        qDebug()<<trans*location;
         directionCosineX=1.0; directionCosineY=1.0;directionCosineZ=1.0;
    ZPlaneForWall=-1500;
    xPlaneForLeftWall=-2000;
    xLine=0.0;
    zLine=0.0;

    //-------------------------------------------------------

}


DTracker::~DTracker() {
    stop();
    wait(100);
}

void DTracker::setHost(const string& _hostname, int _port, int _rport) {
    hostname = _hostname;
    port = _port;
    rport = _rport;
}


void DTracker::stop() {
    //if (state == Connecting) terminate();  // run is blocked
    state = Disconnecting;
}


void DTracker::run() {
    sendState(state = Connecting);

    stringstream ss;
    ss << "\n*** Starting connection with "<< hostname
    << " on ports " << port << " " << rport;
    sendMessage(ss.str());
    sendMessage("Please wait...");

    dt = new DTrackSDK(hostname, rport, port);

    if (!dt->isLocalDataPortValid()) {
        sendMessage("! Could not connect\n");
        delete dt;
        dt = NULL;
        sendState(state = Disconnected);
        return;
    }

    if (!dt->startMeasurement()) {
        sendMessage("! Start measurement failed\n");
        processMessages();
        delete dt;
        dt = NULL;
        sendState(state = Disconnected);
        return;
    }

    sendMessage("Running...");
    sendState(state = Running);

    while (state == Running) {
        if (dt->receive()) processTrackingData();
        else processErrors();
        processMessages();
    }

    sendMessage("Disconnecting\n");
    sendState(state = Disconnecting);

    // stop measurement & clean up:
    dt->stopMeasurement();
    processMessages();

    delete dt;
    dt = NULL;
    sendState(state = Disconnected);
}


