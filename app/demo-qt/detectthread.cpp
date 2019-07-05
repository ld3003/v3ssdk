#include "detectthread.h"

#include <zbar.h>

using namespace std;
using namespace cv;
using namespace zbar;


//zbar接口
string ZbarDecoder(Mat img)
{
    string result;
    ImageScanner scanner;
    const void *raw = (&img)->data;
    // configure the reader
    scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 0);
    // wrap image data
    Image image(img.cols, img.rows, "Y800", raw, img.cols * img.rows);
    // scan the image for barcodes
    int n = scanner.scan(image);
    // extract results
    result = image.symbol_begin()->get_data();
    image.set_data(NULL, 0);
    return result;
}

//对二值图像进行识别，如果失败则开运算进行二次识别
string GetQRInBinImg(Mat binImg)
{
    string result = ZbarDecoder(binImg);
    if(result.empty())
    {
        Mat openImg;
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
        morphologyEx(binImg, openImg, MORPH_OPEN, element);
        result = ZbarDecoder(openImg);
    }
    return result;
}

//main function
string GetQR(Mat img)
{
    Mat binImg;
    //在otsu二值结果的基础上，不断增加阈值，用于识别模糊图像
    int thre = threshold(img, binImg, 0, 255, cv::THRESH_OTSU);
    string result;
    while(result.empty() && thre<255)
    {
        threshold(img, binImg, thre, 255, cv::THRESH_BINARY);
        result = GetQRInBinImg(binImg);
        thre += 20;//阈值步长设为20，步长越大，识别率越低，速度越快
    }
    return result;
}


/*
 *                 result = GetQR(img);
                std::cout << result <<endl;
*/

DetectThread::DetectThread(CamThread *ct)
{
    mCt = ct;

    if (!eye_Classifier.load("./haarcascade_eye.xml"))  //需要将xml文档放在自己指定的路径下
    {
        qDebug() << "Load haarcascade_eye.xml failed!" ;
        //return 0;
    }

    if (!face_cascade.load("./haarcascade_frontalface_alt.xml"))
    {
        qDebug() << "Load haarcascade_frontalface_alt failed!";
        //return 0;
    }

}

void DetectThread::run()
{
    int flag = 0;
    string result;
    QImage Img;
    cv::Mat img;
    std::vector<cv::Rect> eyeRect;
    std::vector<cv::Rect> faceRect;
    cv::Mat image_gray,image3;
flag = 0;
    for(;;)
    {


        qDebug() << " get imgage from camThread ";

        //if (flag == 0)
        {
            img = mCt->getImage();

            cv::cvtColor(img, image_gray, CV_BGR2GRAY);//转为灰度图
#if 0
            result = GetQR(image_gray);
            std::cout << "\r\n\r\n\r\n RRRRRRRRRRRRRRRRRRRRRRRRRRR --- :" << result << " \r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n"<<endl;
#endif

#if 1


#if 1
            cv::equalizeHist(image_gray, image_gray);//直方图均衡化，增加对比度方便处理
            flag = 1;

        }
        face_cascade.detectMultiScale(image_gray, faceRect, 1.8, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(80, 80));
        for (size_t i = 0; i < faceRect.size(); i++)
        {
            flag = 1;
            rectangle(img, faceRect[i], cv::Scalar(0, 0, 255));      //用矩形画出检测到的位置
            qDebug() <<"detect !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

        }
#endif
        //img = image_gray;

#endif

        Img = QImage((const uchar*)(img.data), img.cols, img.rows, img.cols * img.channels(), QImage::Format_RGB888);
        emit detectReady(Img);
        //processEvents();
        //'QCoreApplication::processEvents();
    }

}
