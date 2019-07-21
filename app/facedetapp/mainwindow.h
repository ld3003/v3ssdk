#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <iostream>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "camthread.h"
#include "detectthread.h"

using namespace std;
//using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void detectFace();
    cv::Mat QImageToMat(QImage image);
    ~MainWindow();
public slots:
    void imgFlush(QImage img);

private:
    Ui::MainWindow *ui;
    CamThread *camThread;
    DetectThread *detThread;



};

#endif // MAINWINDOW_H
