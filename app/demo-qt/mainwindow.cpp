#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    Mat image,image2,image3;

    QImage Img;

    ui->setupUi(this);


    cap =  new VideoCapture(0);
    if(cap->isOpened())
    {
        cout<<"success"<<endl;
    }
    else
    {
        cout << "errr" << endl;
    }

    *cap >> image;

    cv::resize(image, image2, Size(image.cols/4, image.rows/4),0,0);

    cv::cvtColor(image2, image3, CV_BGR2RGB);//颜色空间转换

    Img = QImage((const uchar*)(image3.data), image3.cols, image3.rows, image3.cols * image3.channels(), QImage::Format_RGB888);



    ui->label->setPixmap(QPixmap::fromImage(Img));

}

MainWindow::~MainWindow()
{
    delete ui;
}
