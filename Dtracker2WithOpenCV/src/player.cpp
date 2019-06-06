#include "player.h"
#include "opencvtoqt.h"
#include <QDebug>
#include<time.h>
#include<errno.h>

//-----------------
Player::Player(QObject *parent)
 : QThread(parent)
{
    stop = true;
}

//-----------------
void Player::Play()
{
    if (!isRunning()) {
        if (isStopped()){
            stop = false;
        }
        stream = VideoCapture(1);
        start(LowPriority);
        qDebug()<< "start";
    }
}

void Player::run()
{
    int delay = 100;
    while(!stop){
        if (!stream.read(frame)){
            qDebug()<<"test frame 0";
            //stop = true;
        }

        //qDebug()<<"test frame";
        img = mat_to_qimage_cpy(frame);
        emit processedImage(img);
        this->msleep(delay);
    }
}


Player::~Player()
{
    mutex.lock();
    stop = true;
    stream.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}
void Player::Stop()
{
    stop = true;
}
void Player::msleep(int ms){
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
  //  sleep(&ts);
  nanosleep(&ts,NULL);
}
bool Player::isStopped() const{
    return this->stop;
}
