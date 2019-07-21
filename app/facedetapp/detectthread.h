#ifndef DETECTTHREAD_H
#define DETECTTHREAD_H

#include <QThread>
#include <QDebug>
#include <QImage>
#include <camthread.h>

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>



class DetectThread : public QThread
{
    Q_OBJECT
public:
    DetectThread(CamThread *ct);
    void run();
    QString name; //添加一个 name 对象
    CamThread *mCt;

signals:
    void detectReady(QImage img);
};

#endif // DETECTTHREAD_H
