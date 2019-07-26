#include <iostream>
#include <zbar.h>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace zbar;

//zbar接口
string ZbarDecoder(Mat img)
{
    string result;
    ImageScanner scanner;
    const void *raw = (&img)->data;
    // configure the reader
    scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
    // wrap image data
    Image image(img.cols, img.rows, "Y800", raw, img.cols * img.rows);
    // scan the image for barcodes
    int n = scanner.scan(image);
    // extract results
    result = image.symbol_begin()->get_data();
    image.set_data(NULL, 0);
    return result;
}

//对二值图像进行识别，如果失败则开运算进行二次识别
string GetQRInBinImg(Mat binImg)
{
    string result = ZbarDecoder(binImg);
    if(result.empty())
    {
        Mat openImg;
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
        morphologyEx(binImg, openImg, MORPH_OPEN, element);
        result = ZbarDecoder(openImg);
    }
    return result;
}

//main function
string GetQR(Mat img)
{
    Mat binImg;
    //在otsu二值结果的基础上，不断增加阈值，用于识别模糊图像
    int thre = threshold(img, binImg, 0, 255, cv::THRESH_OTSU);
    string result;
    while(result.empty() && thre<255)
    {
        threshold(img, binImg, thre, 255, cv::THRESH_BINARY);
        result = GetQRInBinImg(binImg);
        thre += 20;//阈值步长设为20，步长越大，识别率越低，速度越快
    }
    return result;
}

int main(int c , char **argv)
{
	Mat img;


	string result;
	for(;;)
	{
		img = imread("qr.jpg", CV_LOAD_IMAGE_UNCHANGED);
		result = GetQR(img);
		std::cout << result <<endl;
	}
	return 0;
}
