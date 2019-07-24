#ifndef FACEREGREQUEST_H
#define FACEREGREQUEST_H

#include <QObject>
#include <QRunnable>
#include <opencv2/opencv.hpp>
#include "Communiction.h"

class FaceRegRequest : public QObject, public QRunnable
{

    Q_OBJECT
public:
    FaceRegRequest(cv::Mat face);
    void setFace(cv::Mat face);
protected:
    void run();

signals:
    void mySignal();

private:
    cv::Mat mFace;
    COMMUNICTION mCOMMUNICTION;

};

#endif // FACEREGREQUEST_H
