#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    showFullScreen();
    ui->setupUi(this);

    timer = new QTimer(this);
    qDebug() << "new VideoCapture";
    camThread = new CamThread(cap);
    //detThread = new DetectThread(camThread);

    connect(timer,SIGNAL(timeout()),this,SLOT(timerTimeout()));
    connect(camThread,SIGNAL(imgReady(QImage)),this,SLOT(imgFlush(QImage)));

    camThread->start();
    //timer->start(2000);

    this->setGeometry(0,0,240,320);






}

void MainWindow::detectFace()
{
}

void MainWindow::timerTimeout()
{

    timer->stop();
    detThread->start();
    detectFace();

}

void MainWindow::imgFlush(QImage img)
{
    qDebug() << "recv imgFlush";
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::dectFlush(QImage img)
{

}

cv::Mat MainWindow::QImageToMat(QImage image)
{
    cv::Mat mat;
    return mat;
}


MainWindow::~MainWindow()
{
    delete ui;
}
