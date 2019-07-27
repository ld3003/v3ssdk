#include "camthread.h"
#include "../libfacedetection/src/facedetectcnn.h"

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Common.h"






CamThread::CamThread(QObject *obj)
{

    //GPIO_Init();

    mCap = new cv::VideoCapture(0);

    if( mCap->isOpened())
    {
        qDebug() <<"opencamera success" ;
    }
    else
    {
        qDebug() << "opencamera error" ;
    }


}


void CamThread::run()
{
    QImage Img;
    cv::Mat tmppic0,tmppic;

    for(;;)
    {
        struct timeval gTpstart ,gTpend;
        //time_consuming_start(&gTpstart,&gTpend);

        mImglocker.lock();
        *mCap >> mImageData1;
        //cv::cvtColor(mImageData1, mImageData1, CV_BGR2RGB);
        mImageData2 = mImageData1;

        //time_consuming_print("fetch img time 1",&gTpstart,&gTpend);
        mImglocker.unlock();

        if (mImageData1.empty())
            continue;

        #define RESIZE_VAL 2
        cv::resize(mImageData1, mImageData1, cv::Size(mImageData1.cols/RESIZE_VAL, mImageData1.rows/RESIZE_VAL),0,0);

        //time_consuming_print("fetch img time 2",&gTpstart,&gTpend);

        cv::cvtColor(mImageData1, tmppic, CV_BGR2RGB);
        //tmppic = mImageData1;

        //time_consuming_print("fetch img time 3",&gTpstart,&gTpend);

        mDetlocker.lock();
        rectangle(tmppic, cv::Rect(mDetRect[0]/RESIZE_VAL, mDetRect[1]/RESIZE_VAL, mDetRect[2]/RESIZE_VAL, mDetRect[3]/RESIZE_VAL), cv::Scalar(0, 255, 0), 2);
        mDetlocker.unlock();

        Img = QImage((const uchar*)(tmppic.data), tmppic.cols, tmppic.rows, tmppic.cols * tmppic.channels(), QImage::Format_RGB888);
        //time_consuming_print("fetch img time 4",&gTpstart,&gTpend);
        emit imgReady(Img);

    }

}

cv::Mat CamThread::getImage()
{
    cv::Mat image;
    mImglocker.lock();
    image = mImageData2;
    mImglocker.unlock();
    return image;
}

void CamThread::setDetRect(int x, int y , int w , int h)
{
    mDetlocker.lock();
    mDetRect[0] = x;
    mDetRect[1] = y;
    mDetRect[2] = w;
    mDetRect[3] = h;
    mDetlocker.unlock();
}
