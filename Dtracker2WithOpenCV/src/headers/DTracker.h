/*
* This class serves to connect the ART tracker and process data in a Qt GUI.
* (c) Eric Lecolinet - Telecom ParisTech 2014.
* Derived from "example_with_simple_remote_control" from A.R.T. GmbH.
* see the .cpp file for details.
*/

#ifndef DTRACKER_H
#define DTRACKER_H

#include <string>
#include <QObject>
#include <QThread>
#include<QPointF>
#include<qvector3d.h>
#include<QMatrix4x4>




/**
* This class serves to connect the ART tracker and process data in a Qt GUI.
*
* The start() method (which is an inherited slot) etablishes the connection and
* starts processing data by executing the (private) run() method in a new thread.
*/
class DTracker : public QThread {
    Q_OBJECT
public:
    enum State {Disconnected, Connecting, Running, Disconnecting};

    DTracker(const std::string& hostname, int port, int rport);
    ~DTracker();

//-----------------------------------------------------------------------
    QMatrix4x4 trans;
QVector3D location;
QVector3D transformedLocation;
//------------------------------------------------------


public slots:
    // start() is inherited;

    void stop();

    void setHost(const std::string& hostname, int port, int rport);

    const std::string& getHostName() const {return hostname;}
    int getPort() const {return port;}
    int getRemotePort() const {return rport;}



signals:
     void onMessage(const QString &);
     void onStateChange(int state);
     void bodyTracked(QPointF );
     void flyStickButtonReleased(QPointF );
     void flyStickButtonPressed(QPointF );


private:
     class DTrackSDK* dt;
     std::string hostname;
     int port, rport;
     State state;


     //----------------------------------------------------------
     float directionCosineX;
     float directionCosineY;
     float directionCosineZ;
     float ModulusOfPosition;
     QPointF intersectionPoint;
      float ZPlaneForWall;
      float zLine;

      float xPlaneForLeftWall;
      float xLine;
      int prevButtonState;

     //---------------------------------------------------------------
     /// run() is redefined. It is executed in a new thread and serves to get
     /// the data from the tracker.
     void run();






     void processTrackingData();  ///< current tracking data.
     bool processErrors();        ///< errors, return true if no error occured.
     void processMessages();      ///< ATC messages.

     void sendMessage(const std::string&);
     void sendState(int type);
};

#endif
