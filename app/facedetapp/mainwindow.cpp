#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QPixmap>
#include <QGraphicsScene>

#include <QGraphicsScene>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    showFullScreen();
    ui->setupUi(this);


    camThread = new CamThread(0);
    detThread = new DetectThread(camThread);

    connect(camThread,SIGNAL(imgReady(QImage)),this,SLOT(imgFlush(QImage)));

    camThread->start();
    detThread->start();

}

void MainWindow::imgFlush(QImage img)
{
    ui->label->setPixmap(QPixmap::fromImage(img));
}

MainWindow::~MainWindow()
{
    delete ui;
}
