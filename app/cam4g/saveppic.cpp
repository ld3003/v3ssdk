#include <opencv2/opencv.hpp>
#include <iostream>
#include <pthread.h>

static cv::VideoCapture *cap;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int opencam()
{

    cv::Mat pic;

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

    *cap >> pic;

    return 0;
}

int getpic2(void)
{
    cv::Mat pic;

    for (int i = 0; i < 3; i++)
    {
        pthread_mutex_lock(&mutex);
        *cap >> pic;
        pthread_mutex_unlock(&mutex);
    }

    return 1;
}

int getpic(void)
{
    cv::Mat pic;

    pthread_mutex_lock(&mutex);
    *cap >> pic;
    pthread_mutex_unlock(&mutex);

    if (pic.empty())
        return -1;

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(90);
    cv::imwrite("/tmp/tmp.jpg", pic, compression_params);

    return 1;
}