#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QPixmap>
#include <QTextFormat>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    showFullScreen();
    ui->setupUi(this);


    camThread = new CamThread(0);
    detThread = new DetectThread(camThread);

    connect(camThread,SIGNAL(imgReady(QImage)),this,SLOT(imgFlush(QImage)));
    connect(detThread,SIGNAL(tipmsg(QString)),this,SLOT(tipmsg(QString)));

    camThread->start();
    detThread->start();

    tipLable = new QLabel(this);
    tipLable->setGeometry(0,0,this->width(),this->height());
    tipLable->hide();

    QFont ft;
    ft.setPointSize(100);
    tipLable->setFont(ft);

    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    tipLable->setPalette(pa);

    tipLable->setAlignment(Qt::AlignCenter);


}

void MainWindow::imgFlush(QImage img)
{
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::tipmsg(QString str)
{
    qDebug() << "Tip msg : " << str;
    tipLable->show();
    tipLable->setText(str);
    //tipLable->setTextFormat()
    //
}

MainWindow::~MainWindow()
{
    delete ui;
}
