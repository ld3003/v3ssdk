#include <opencv2/opencv.hpp>
#include <iostream>

static cv::VideoCapture *cap;

int opencam()
{

    cap = new cv::VideoCapture(0);
    if (cap->isOpened())
    {
        printf("opencamera success\n");
    }
    else
    {
        printf("opencamera error\n");
        return -1;
    }

    return 0;

}

int getpic(void)
{
    cv::Mat pic;

    

    *cap >> pic;

    if (pic.empty())
        return -1;

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(50);
    cv::imwrite("/tmp/tmp.jpg", pic, compression_params);

    return 1;
}