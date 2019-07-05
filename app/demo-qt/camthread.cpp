#include "camthread.h"
#include "libfacedetection/src/facedetectcnn.h"


#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PIO_BASE_ADDRESS 0x01C20800


//unsigned int 4字节 一个PIO_Struct占36字节,对应十六进制就是0x24，正好是一个offset值。
typedef struct
{
    unsigned int CFG[4];
    unsigned int DAT;
    unsigned int DRV0;
    unsigned int DRV1;
    unsigned int PUL0;
    unsigned int PUL1;
} PIO_Struct;

typedef struct
{
    PIO_Struct Pn[7];
} PIO_Map;

typedef enum {
    PA = 0,
    PB = 1,
    PC = 2,
    PD = 3,
    PE = 4,
    PF = 5,
    PG = 6,
} PORT;

typedef enum {
    IN = 0x00,
    OUT = 0x01,
    AUX = 0x02,
    INT = 0x06,
    DISABLE = 0x07,
} PIN_MODE;

extern PIO_Map *PIO;

void GPIO_Init(void);
void GPIO_ConfigPin(PORT port, unsigned int pin, PIN_MODE mode);
void GPIO_SetPin(PORT port, unsigned int pin, unsigned int level);
unsigned int GPIO_GetPin(PORT port, unsigned int pin);
int GPIO_Free(void);

//#include "gpio.h"

unsigned int fd;
PIO_Map *PIO = NULL;
unsigned int *gpio_map;
unsigned int addr_start, addr_offset;
unsigned int PageSize, PageMask;
void GPIO_Init(void)
{

    if ((fd = open("/dev/mem", O_RDWR)) == -1)
    {
        printf("open error\r\n");
        return;
    }

    PageSize = sysconf(_SC_PAGESIZE); //使用sysconf查询系统页面大小
    PageMask = (~(PageSize - 1));     //页掩码
    printf("PageSize:%d,PageMask:0x%.8X\r\n", PageSize, PageMask);

    addr_start = PIO_BASE_ADDRESS & PageMask;   //0x01C20800 & 0xfffff000 =  0x1C20000
    addr_offset = PIO_BASE_ADDRESS & ~PageMask; //0x01C20800 & 0x00000100 = 0x800
    printf("addr_start:%.8X,addr_offset:0x%.8X\r\n", addr_start, addr_offset);
    //mmap(系统自动分配内存地址，映射区长度“内存页的整数倍”，选择可读可写，MAP_SHARED=与其他所有映射到这个对象的进程共享空间，文件句柄，被映射内容的起点)
    //offest 映射物理内存的话，必须页对其!!!   所以这个起始地址应该是0x1000的整数倍，那么明显0x01C20800需要减去0x800才是整数倍！
    if ((gpio_map = (unsigned int *)mmap(0, PageSize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr_start)) == NULL)
    {
        printf("mmap error\r\n");
        close(fd);
        return;
    }
    printf("gpio_map:%.8X\r\n", gpio_map);
    //这里已经将0x1c20000的地址映射到了内存中，但是我们需要的地址是0x01C20800，所以要再加上地址偏移量～
    PIO = (PIO_Map *)((unsigned int)gpio_map + addr_offset);
    printf("PIO:0x%.8X\r\n", PIO);

    close(fd); //映射好之后就可以关闭文件？
}

void GPIO_ConfigPin(PORT port, unsigned int pin, PIN_MODE mode)
{
    if (gpio_map == NULL)
        return;
    PIO->Pn[port].CFG[pin / 8] &= ~((unsigned int)0x07 << pin % 8 * 4);
    PIO->Pn[port].CFG[pin / 8] |= ((unsigned int)mode << pin % 8 * 4);
    printf("struct PIO_Struct size : %d",sizeof(PIO->Pn[port]));
}

void GPIO_SetPin(PORT port, unsigned int pin, unsigned int level)
{
    if (gpio_map == NULL)
        return;
    if (level)
        PIO->Pn[port].DAT |= (1 << pin);
    else
        PIO->Pn[port].DAT &= ~(1 << pin);
}



void testgpio()
{
    unsigned int tmp = 1<< 16;
    unsigned int tmp2 = ~tmp;
    for(;;)
    {
    PIO->Pn[PE].DAT |= tmp;
    PIO->Pn[PE].DAT &= tmp2;
    }
}

int GPIO_Free(void)
{
    if ((munmap(gpio_map, PageSize * 2)) == 0)//取消映射
    {
        printf("unmap success!\r\n");
    }
    else
    {
        printf("unmap failed!\r\n");
    }
    return 0;
}


#include <sys/time.h>

/*执行耗时计算参数*/
struct timeval gTpstart,gTpend;

void time_consuming_start()
{
    memset(&gTpstart,0,sizeof(struct timeval));
    memset(&gTpend,0,sizeof(struct timeval));
    gettimeofday(&gTpstart,NULL); // 开始时间
}

void time_consuming_print(char *strPuts)
{
    float timeuse;

    gettimeofday(&gTpend,NULL); // 结束时间
    timeuse=1000000*(gTpend.tv_sec-gTpstart.tv_sec)+gTpend.tv_usec-gTpstart.tv_usec;
    timeuse/=1000000;
    printf("@ %s -----> Used Time:%f  S\n",strPuts,timeuse);
}


CamThread::CamThread(cv::VideoCapture *cap)
{

    GPIO_Init();
    //GPIO_SetPin(PB,2,0);




#ifdef USE_V4L2
    camera.fd = -1;

    v4l2_open(&camera, 0);
    v4l2_enum_fmt(&camera);
    v4l2_query_cap(&camera);
    v4l2_s_input(&camera, 0);
    v4l2_s_fmt(&camera, 640, 480, V4L2_PIX_FMT_NV12);
    v4l2_g_fmt(&camera);
    v4l2_reqbufs(&camera, 4);
    v4l2_stream(&camera, 1);


    argb_ptr = (uint8_t*)malloc(camera.fmt.fmt.pix.width * camera.fmt.fmt.pix.height * 4);
#else
    mCap = new cv::VideoCapture(1);
    //mCap->set(CV_CAP_PROP_FRAME_WIDTH,320);
    //mCap->set(CV_CAP_PROP_FRAME_HEIGHT,240);


    if( mCap->isOpened())
    {
        qDebug() <<"opencamera success" ;
    }
    else
    {
        qDebug() << "errr" ;
    }
#endif


}

void CamThread::abARGB2MatBGR(unsigned int *pbuff, int nRows, int nCols, cv::Mat &imag)
{
}


cv::Mat CamThread::TransBufferToMat(unsigned char* pBuffer, int nWidth, int nHeight, int nBandNum, int nBPB)
{
    cv::Mat x;
    return x;
}



void CamThread::run()
{
    int i;
    int benshup;
    QImage Img;
    cv::Mat image,image2,image3,image4;


#ifdef USE_V4L2
    uint8_t *frame_ptr;
    int      frame_len;

    void *pp = (void*)&frame_ptr;




    for(;;)
    {

        v4l2_read_frame(&camera,(void**)pp,&frame_len);

        if (frame_len <= 0) {
            continue;
        }

        printf("malloc address %x \r\n",(unsigned int)frame_ptr);
        qDebug() << "recv img length " << frame_len << v4l2_width(&camera) << v4l2_height(&camera);


        libyuv::NV12ToARGB(frame_ptr, v4l2_width(&camera),
                           frame_ptr + v4l2_width(&camera) * v4l2_height(&camera),
                           v4l2_width(&camera),
                           argb_ptr,
                           v4l2_width(&camera) * 4,
                           v4l2_width(&camera), v4l2_height(&camera));

        free(frame_ptr);

        qDebug() << "create mat img 1";

        //abARGB2MatBGR((unsigned int *)argb_ptr,v4l2_height(&camera),v4l2_width(&camera),image);

#else

#endif





#if 1
#define DETECT_BUFFER_SIZE 0x20000
    unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);

    qDebug() << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx111x";



    for(;;)
    {
        int *pResults;

        time_consuming_start();

        *mCap >> image;

        time_consuming_print("#################1");

#if 0
        Img = QImage((const uchar*)(image.data), image.cols, image.rows, image.cols * image.channels(), QImage::Format_RGB888);
        time_consuming_print("#################5");
        qDebug() << "image size " << image.cols;
        emit imgReady(Img);
        continue ;
#endif

        cv::resize(image, image3, cv::Size(image.cols/4, image.rows/4),0,0);
        cv::resize(image, image2, cv::Size(image.cols/4, image.rows/4),0,0);
        cv::cvtColor(image2, image2, CV_BGR2RGB);//颜色空间转换

        time_consuming_print("#################2");

        //qDebug() << "IMAGE image.cols " << image3.cols << image3.rows << (int)image3.step;
#if 1
        pResults = facedetect_cnn(pBuffer, (unsigned char*)(image3.ptr(0)), image3.cols, image3.rows, (int)image3.step);




        //print the detection results
        for(i = 0; i < (pResults ? *pResults : 0); i++)
        {
            short * p = ((short*)(pResults+1))+142*i;
            int x = p[0];
            int y = p[1];
            int w = p[2];
            int h = p[3];
            int confidence = p[4];
            int angle = p[5];

            rectangle(image2, cv::Rect(x, y, w, h), cv::Scalar(0, 255, 0), 2);
            GPIO_SetPin(PB,2,1);

        }

        time_consuming_print("#################3");


#endif
        time_consuming_print("#################4");

        //人脸检测

        Img = QImage((const uchar*)(image2.data), image2.cols, image2.rows, image2.cols * image2.channels(), QImage::Format_RGB888);
        time_consuming_print("#################5");

        emit imgReady(Img);

    }
#endif

}

cv::Mat CamThread::getImage()
{
    mImglocker.lock();
    mImageData2 = mImageData;
    mImglocker.unlock();

    return mImageData2;
}
