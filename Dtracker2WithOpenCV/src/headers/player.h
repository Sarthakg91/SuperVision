#ifndef PLAYER_H
#define PLAYER_H

#include <QThread>

#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMutex>
using namespace cv;
class Player : public QThread
{    Q_OBJECT


 public:

    Player(QObject *parent = 0);
    ~Player();


    void Play();
    void Stop();

    //check if the player has been stopped
    bool isStopped() const;

 signals:
 //Signal to output frame to be displayed
      void processedImage(const QImage &image);
 protected:
     void run();
     void msleep(int ms);

 private:
    bool stop;
    QMutex mutex;
    QWaitCondition condition;
    Mat frame;
    int frameRate;
    VideoCapture stream;
    Mat RGBframe;
    QImage img;
};


#endif // PLAYER_H
