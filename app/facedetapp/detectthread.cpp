#include "detectthread.h"
#include "../libfacedetection/src/facedetectcnn.h"
//#include <zbar.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <curl/curl.h>
#include "faceregrequest.h"
#include <QThreadPool>

#include "Communiction.h"
#include "Common.h"

using namespace std;
using namespace cv;

#define RESET_VAL 4
#define DETECT_BUFFER_SIZE 0x20000
#define USE_MTCNN


void Jpegcompress(const cv::Mat& src, cv::Mat& dest, int quality)
{
    std::vector<uchar> buff;
    std::vector<int> params;
    /*IMWRITE_JPEG_QUALITY For JPEG, it can be a quality from 0 to 100
    (the higher is the better). Default value is 95 */
    params.push_back(cv::IMWRITE_JPEG_QUALITY);
    params.push_back(quality);
    //将图像压缩编码到缓冲流区域
    cv::imencode(".jpg", src, buff, params);
    //将压缩后的缓冲流内容解码为Mat，进行后续的处理
    dest = cv::imdecode(buff, -1);
    //cv::imshow("src", src);
    //cv::imshow("dst", dest);
}

DetectThread::DetectThread(CamThread *ct)
{
    mtcnn = new MTCNN("./models");
    mCt = ct;
    tp.setMaxThreadCount(5);
}


void DetectThread::run()
{

    char checkflag = 0;





    for(;;)
    {
        int i;
        int *pResults;

        std::vector<Bbox> finalBbox;

        cv::Mat image1 = mCt->getImage();
        cv::Mat image2;
        cv::Mat image3;
        cv::Mat image_roi;

        if (image1.empty())
            continue;

#ifdef USE_MTCNN

        cv::resize(image1, image3, cv::Size(image1.cols/RESET_VAL, image1.rows/RESET_VAL),0,0);
        ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(image3.data, ncnn::Mat::PIXEL_BGR2RGB, image3.cols, image3.rows);

        struct timeval gTpstart ,gTpend;
        time_consuming_start(&gTpstart,&gTpend);
        mtcnn->detectMaxFace(ncnn_img, finalBbox);
        time_consuming_print("detect time",&gTpstart,&gTpend);
        if (finalBbox.size() == 1)
        {

            int x = finalBbox[0].x1;
            int y = finalBbox[0].y1;
            int w = finalBbox[0].x2 - finalBbox[0].x1 + 1;
            int h = finalBbox[0].y2 - finalBbox[0].y1;

            mCt->setDetRect(x*RESET_VAL,y*RESET_VAL,w*RESET_VAL,h*RESET_VAL);

        }

        continue;
#endif


#ifdef USE_LIBDET


        unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);

        cv::resize(image1, image3, cv::Size(image1.cols/RESET_VAL, image1.rows/RESET_VAL),0,0);
        struct timeval gTpstart ,gTpend;
        time_consuming_start(&gTpstart,&gTpend);
        //time_consuming_print("detect time",&gTpstart,&gTpend);
        pResults = facedetect_cnn(pBuffer, (unsigned char*)(image3.ptr(0)), image3.cols, image3.rows, (int)image3.step);

        //print the detection results
        for(i = 0; i < (pResults ? *pResults : 0); i++)
        {
            short * p = ((short*)(pResults+1))+142*i;
            int x = p[0]*RESET_VAL;
            int y = p[1]*RESET_VAL;
            int w = p[2]*RESET_VAL;
            int h = p[3]*RESET_VAL;
            int confidence = p[4];
            int angle = p[5];

            mCt->setDetRect(x,y,w,h);


            printf("ROI X %d Y %d W %d H %d\n",x,y,w,h);
            if (((x+w) > image1.cols) || ((y+h) > image1.rows) || (x<0) || (y<0))
                continue;

            Rect rect(x, y, w, h);
            image_roi = image1(rect);

            if (tp.activeThreadCount() < 3 )
            {
                FaceRegRequest *FR = new FaceRegRequest(image_roi);
                if(!FR->autoDelete()) {
                    qDebug()<<"QRunnable's autoDelete default value is not true";
                    FR->setAutoDelete(true);
                }
                tp.start(FR);
            }

            //FR->run();
            //FR->deleteLater();



        }

        time_consuming_print("detect time",&gTpstart,&gTpend);
#endif
    }

}
