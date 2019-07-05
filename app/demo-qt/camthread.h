#ifndef CAMTHREAD_H
#define CAMTHREAD_H

#include <QThread>
#include <QDebug>
#include <QImage>
#include <QMutexLocker>

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>


class CamThread : public QThread
{
    Q_OBJECT

public:
    CamThread(cv::VideoCapture *cap);
    void run();
    cv::Mat getImage();
    void abARGB2MatBGR(unsigned int *pbuff, int nRows, int nCols, cv::Mat &imag);
    cv::Mat TransBufferToMat(unsigned char* pBuffer, int nWidth, int nHeight, int nBandNum, int nBPB = 1);
    QString name; //添加一个 name 对象
signals:
    void imgReady(QImage img);

private:
    cv::VideoCapture *mCap;

    QMutex mImglocker;
    cv::Mat mImageData;
    cv::Mat mImageData2;



};

#endif // CAMTHREAD_H
