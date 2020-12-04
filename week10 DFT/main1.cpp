#include <opencv2/opencv.hpp>
#include <iostream>
#include "funtions.h"
using namespace cv;
using namespace std;

int main()
{
	
	double start = static_cast<double>(cvGetTickCount());
	cv::Mat src1 = imread("D:\\jiang.jpg", 0);
	cv::Mat src2 = imread("D:\\dai.jpg", 0);
	cv::Mat dst1;
	cv::Mat dst2;
	cv::Mat dst;

	ifftDemo(src1,0.5,dst1);
	imshow("dst1", dst1);

	ifftDemo(src2,0.3, dst2);
	imshow("dst2", dst2);

	add(dst2, dst1, dst);
	imshow("final", dst);
	imshow("dst1",dst1);
	
	waitKey(0);
	return 0;
}
