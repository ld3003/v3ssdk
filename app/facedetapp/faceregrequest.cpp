#include "faceregrequest.h"
#include "Communiction.h"

using namespace std;
using namespace cv;

FaceRegRequest::FaceRegRequest(Mat face)
{
    mFace = face;
    communiction_init(&mCOMMUNICTION);
}
void FaceRegRequest::setFace(cv::Mat face)
{
    mFace = face;
}
void FaceRegRequest::run()
{
    COMMUNICTION_RESULT resu;
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    cv::imwrite("/tmp/outImage.jpg", mFace, compression_params);
    time_consuming_print("jpg compression time");
    time_consuming_start();
    communiction_pushpic(&mCOMMUNICTION,0,&resu);

    QMetaObject::invokeMethod(0, "setValue", Q_ARG(int, 100));
    //判断结果 resu

}
