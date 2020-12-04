#include "funtions.h"


std::vector<Point>  mousePoints;
Point points;
/******************************************************************************************************************************
														����Ҷ�任demo����������
*****************************************************************************************************************************/

int dftDemo() {

	cv::Mat srcMat = imread("D:\\lean.jpg", 0);
	cv::Mat magMat;

	if (srcMat.empty()) {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	//��ͼ��ת��Ϊ���ӵĸ���Ҷ�任ͼ��
	calcVisibalMag(srcMat, magMat);

	imshow("Input Image", srcMat);    // Show the result
	imshow("spectrum magnitude", magMat);
	waitKey(0);

	return 0;

}

/*******************************************
1.����һ��ͼƬ����������ӻ��ķ�ֵ��
2.�ٷ�ֵ��ͼ�ϣ�ͨ�����ѡ����Ҫȥ����Ƶ��
3.ȥ����ѡ����źţ�Ȼ��ԭͼ��
**********************************************/
int removeFrequnce()
{
	cv::Mat srcMat = imread("D:\\lean.jpg", 0);
	cv::Mat magMat;
	cv::Mat phMat;
	cv::Mat maskMat;
	double normVal;

	if (srcMat.empty()) {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	//������ӻ���mag���Լ���λ�ף��Լ���һ��ϵ��
	calcVisbalDft(srcMat, magMat, phMat, normVal);

	//�ڷ�ֵ���ϣ�ͨ�����ѡ����Ҫȥ����Ƶ��
	selectPolygon(magMat, maskMat);

	//��任


	return 0;
}


/***************************************�����Ӧ����*******************************************/
void on_mouse(int EVENT, int x, int y, int flags, void* userdata)
{

	Mat hh;
	hh = *(Mat*)userdata;
	Point p(x, y);
	switch (EVENT)
	{
	case EVENT_LBUTTONDOWN:
	{
		points.x = x;
		points.y = y;
		mousePoints.push_back(points);
		circle(hh, points, 4, cvScalar(255, 255, 255), -1);
		imshow("mouseCallback", hh);
	}
	break;
	}

}


int selectPolygon(cv::Mat srcMat, cv::Mat& dstMat)
{

	vector<vector<Point>> contours;
	cv::Mat selectMat;

	cv::Mat m = cv::Mat::zeros(srcMat.size(), CV_32F);

	m = 1;

	if (!srcMat.empty()) {
		srcMat.copyTo(selectMat);
		srcMat.copyTo(dstMat);
	}
	else {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	namedWindow("mouseCallback");
	imshow("mouseCallback", selectMat);
	setMouseCallback("mouseCallback", on_mouse, &selectMat);
	waitKey(0);
	destroyAllWindows();
	//����roi
	contours.push_back(mousePoints);
	if (contours[0].size() < 3) {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	drawContours(m, contours, 0, Scalar(0), -1);

	m.copyTo(dstMat);

	return 0;
}


/***********����һ��ͼƬ������丵��Ҷ�任��Ŀ��ӻ��ķ�ֵ��********************/
int calcVisibalMag(cv::Mat srcMat, cv::Mat& dstMat)
{

	if (srcMat.empty()) {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	Mat padMat;
	//��ͼ��ĳߴ���2��3��5��������ʱ����ɢ����Ҷ�任�ļ����ٶ���졣	
	//�������ͼ�����ѱ任�ߴ�
	int m = getOptimalDFTSize(srcMat.rows);
	int n = getOptimalDFTSize(srcMat.cols);


	//���³ߴ��ͼƬ���б�Ե��Ե���
	/************************************************
	copyMakeBorder���� ����ģ�ͣ�
	copyMakeBorder(InputArray src, OutputArray dst,
								 int top, int bottom, int left, int right,
								 int borderType, const Scalar& value = Scalar() );
	�������ܣ�
	. InputArray src��InputArray���͵�src
	. OutputArray dst�����ͼ��
	. int top, int bottom, int left, int right����ʾ�Ա߽�ÿ���������ӵ����ظ���
	. int borderType: ��ʾ����߽�����ͣ���BORDER_REPLICATE ���ǶԱ߽����ؽ��и��ƣ� BORDER_REFLECT ���䣺�Ը���Ȥ��ͼ���е����������߽��и���
	. const Scalar& value :�߽����ɫֵ
	**************************************************/
	copyMakeBorder(srcMat, padMat, 0, m - srcMat.rows, 0, n - srcMat.cols, BORDER_CONSTANT, Scalar::all(0));




	//����һ������,�洢Ƶ��ת����float���͵Ķ����ٴ洢һ������һ����С�ռ�Ķ������洢��������
	Mat planes[] = { Mat_<float>(padMat), Mat::zeros(padMat.size(), CV_32F) };
	Mat complexMat;

	//��2����ͨ����ͼ��ϳ�һ����ͨ��ͼ��
	/************************************************
	merge���� ����ģ�ͣ�
	merge(const Mat* mv, size_t count, OutputArray dst);
	�������ܣ�
	. const Mat* mv��Mat������
	. size_t count����ϲ��������
	. OutputArray dst���������
	**************************************************/
	merge(planes, 2, complexMat);


	//���и���Ҷ�任,���������ԭMat��,����Ҷ�任���Ϊ����.ͨ��1�����ʵ��,ͨ����������鲿
	/************************************************
	dft���� ����ģ�ͣ�
	dft(InputArray src, OutputArray dst, int flags = 0, int nonzeroRows = 0);
	�������ܣ�
	. InputArray src������ͼ�񣬿�����ʵ��������
	. OutputArray dst�����ͼ�����С������ȡ���ڵ���������flags
	. int flags��ת���ı�ʶ������Ĭ��ֵ0
	. int nonzeroRows: �����������Ϊ0�����������ֻ���������飨û������DFT_INVERSE���ĵ�һ�л��һ��������飨������DFT_INVERSE����������ֵ��
	**************************************************/
	dft(complexMat, complexMat);


	//��˫ͨ����ͼ�����������ͨ����ͼ 
	//ʵ����planes[0] = Re(DFT(I),
	//�鲿��planes[1]=  Im(DFT(I))) 
	split(complexMat, planes);
	//����λ��������planes[0]
	magnitude(planes[0], planes[1], planes[0]);

	//���²����Ϊ����ʾ����
	Mat magMat = planes[0];
	// log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	magMat += Scalar::all(1);
	//ȡ����
	/************************************************
	log���� ����ģ�ͣ�
	log(InputArray src, OutputArray dst);
	�������ܣ�
	. InputArray src������ͼ�񣬿�����ʵ��������
	. OutputArray dst������õ��Ķ���ֵ
	**************************************************/
	log(magMat, magMat);

	//ȷ���Գ�
	magMat = magMat(Rect(0, 0, magMat.cols & -2, magMat.rows & -2));
	int cx = magMat.cols / 2;
	int cy = magMat.rows / 2;
	//��ͼ������
	/*
	0 | 1         3 | 2
	-------  ===> -------
	2 | 3         1 | 0
	*/
	Mat q0(magMat, Rect(0, 0, cx, cy));
	Mat q1(magMat, Rect(cx, 0, cx, cy));
	Mat q2(magMat, Rect(0, cy, cx, cy));
	Mat q3(magMat, Rect(cx, cy, cx, cy));
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	//Ϊ��imshow������ʾ����һ����0��1֮��
	/************************************************
	normalize���� ����ģ�ͣ�
	normalize( InputArray src, InputOutputArray dst, double alpha = 1, double beta = 0,
							 int norm_type = NORM_L2, int dtype = -1, InputArray mask = noArray());
	�������ܣ�
			InputArray src����������
			InputOutputArray dst��������飬֧��ԭ������
			double alpha��range normalizationģʽ����Сֵ
			double beta��range normalizationģʽ�����ֵ��������norm normalization(������һ��)ģʽ��
			normType����һ�������ͣ����������µ�ȡֵ��
					  NORM_MINMAX:�������ֵ��ƽ�ƻ����ŵ�һ��ָ���ķ�Χ�����Թ�һ����һ��ϳ��á�
					  NORM_INF:�����͵Ķ���û�в鵽������OpenCV 1�Ķ�Ӧ������ǹ�һ�������C-����(����ֵ�����ֵ)
					  NORM_L1 : ��һ�������L1-����(����ֵ�ĺ�)
					  NORM_L2: ��һ�������(ŷ�����)L2-����
			dtype��dtypeΪ����ʱ����������type�����������type��ͬ�����������������������ֻ��ͨ������ͬ����tpye=CV_MAT_DEPTH(dtype).
			mask��������Ĥ������ָʾ�����Ƿ������ָ����Ԫ�ؽ��в���
	**************************************************/
	normalize(magMat, magMat, 0, 1, NORM_MINMAX);
	magMat = magMat * 255;
	magMat.copyTo(dstMat);

	return 0;
}


//����һ��ͼƬ������丵��Ҷ�任��Ŀ��ӻ��ķ�ֵ��
//ͬʱ�����λ�ף��ͻ�ԭ��һ��ʱ��ϵ���������ֵ
int calcVisbalDft(cv::Mat srcMat, cv::Mat& magMat, cv::Mat& ph, double& normVal)
{
	cv::Mat dst;
	cv::Mat src = imread("D:\\lean.jpg", 0);

	int m = getOptimalDFTSize(src.rows); //2,3,5�ı����и���Ч�ʵĸ���Ҷ�任
	int n = getOptimalDFTSize(src.cols);
	Mat padded;
	//�ѻҶ�ͼ��������Ͻ�,���ұߺ��±���չͼ��,��չ�������Ϊ0;
	copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));
	//planes[0]Ϊdft�任��ʵ����planes[1]Ϊ�鲿��phΪ��λ�� plane_true=magΪ��ֵ
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat planes_true = Mat_<float>(padded);

	//������λ��Mat_����ȷ�����������ͣ�����Ԫ��ʱ����Ҫ��ָ��Ԫ�����ͣ�
	ph = Mat_<float>(padded);

	Mat complexImg;
	//��ͨ��complexImg����ʵ�������鲿

	merge(planes, 2, complexImg);
	//���ϱߺϳɵ�mat���и���Ҷ�任,***֧��ԭ�ز���***,����Ҷ�任���Ϊ����.ͨ��1�����ʵ��,ͨ����������鲿
	dft(complexImg, complexImg);
	//�ѱ任��Ľ���ָ����mat,һ��ʵ��,һ���鲿,�����������
	split(complexImg, planes);

	//---------------�˲���Ŀ��Ϊ���õ���ʾ��ֵ---�����ָ�ԭͼʱ�����ٴ���һ��-------------------------
	magnitude(planes[0], planes[1], planes_true);//������mag
	phase(planes[0], planes[1], ph);//��λ��ph
	Mat A = planes[0];
	Mat B = planes[1];
	Mat mag = planes_true;

	mag += Scalar::all(1);//�Է�ֵ��1
						  //������ķ�ֵһ��ܴ󣬴ﵽ10^4,ͨ��û�а취��ͼ������ʾ��������Ҫ�������log��⡣
	log(mag, mag);

	//ȡ�����е����ֵ�����ں�����ԭʱȥ��һ��
	minMaxLoc(mag, 0, &normVal, 0, 0);

	//�޼�Ƶ��,���ͼ����л������������Ļ�,����Ƶ���ǲ��ԳƵ�,���Ҫ�޼�
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	ph = ph(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	Mat _magI = mag.clone();
	//�����ȹ�һ��������ʾ��Χ��
	normalize(_magI, _magI, 0, 1, CV_MINMAX);
	//imshow("before rearrange", _magI);

	//��ʾ����Ƶ��ͼ
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	//������������Ϊ��׼����magͼ��ֳ��Ĳ���
	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	normalize(mag, mag, 0, 1, CV_MINMAX);
	mag = mag * 255;

	return 0;
}

int calcDft2Image(cv::Mat magMat, cv::Mat ph, double normVal, cv::Mat& dstMat)
{
	Mat mag = magMat.clone();
	Mat proceMag;
	//planes[0]Ϊdft�任��ʵ����planes[1]Ϊ�鲿��phΪ��λ�� plane_true=magΪ��ֵ
	Mat planes[] = { Mat_<float>(mag), Mat::zeros(mag.size(), CV_32F) };
	Mat planes_true = Mat_<float>(mag);

	Mat complexImg;
	//��ͨ��complexImg����ʵ�������鲿

	mag = magMat / 255;

	proceMag = mag * 255;

	int cx = mag.cols / 2;
	int cy = mag.rows / 2;
	//ǰ�����跴����һ�飬Ŀ����Ϊ����任��ԭͼ
	Mat q00(mag, Rect(0, 0, cx, cy));
	Mat q10(mag, Rect(cx, 0, cx, cy));
	Mat q20(mag, Rect(0, cy, cx, cy));
	Mat q30(mag, Rect(cx, cy, cx, cy));

	Mat tmp;
	//��������
	q00.copyTo(tmp);
	q30.copyTo(q00);
	tmp.copyTo(q30);
	q10.copyTo(tmp);
	q20.copyTo(q10);
	tmp.copyTo(q20);

	mag = mag * normVal;//����һ���ľ���ԭ 
	exp(mag, mag);		//��Ӧ��ǰ��ȥ����
	mag = mag - Scalar::all(1);//��Ӧǰ��+1
	polarToCart(mag, ph, planes[0], planes[1]);//�ɷ�����mag����λ��ph�ָ�ʵ��planes[0]���鲿planes[1]
	merge(planes, 2, complexImg);//��ʵ���鲿�ϲ�


	//-----------------------����Ҷ����任-----------------------------------
	Mat ifft(Size(mag.cols, mag.rows), CV_8UC1);
	//����Ҷ��任
	idft(complexImg, ifft, DFT_REAL_OUTPUT);
	normalize(ifft, ifft, 0, 1, CV_MINMAX);

	Rect rect(0, 0, mag.cols, mag.rows);
	dstMat = ifft(rect);
	dstMat = dstMat * 255;

	/*cv::Mat dspMat;
	dst.convertTo(dspMat, CV_8UC1);*/


	return 0;
}



int mouseROI()
{
	cv::Mat srcMat = imread("D:\\lean.jpg");
	cv::Mat dstMat;

	selectPolygon(srcMat, dstMat);

	imshow("srcMat", srcMat);
	imshow("select Area", dstMat);
	waitKey(0);

	return 0;
}



int ifftDemo()
{
	cv::Mat dst;

	cv::Mat src = imread("D:\\lean.jpg", 0);

	int m = getOptimalDFTSize(src.rows); //2,3,5�ı����и���Ч�ʵĸ���Ҷ�任
	int n = getOptimalDFTSize(src.cols);
	Mat padded;
	//�ѻҶ�ͼ��������Ͻ�,���ұߺ��±���չͼ��,��չ�������Ϊ0;
	copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));
	//planes[0]Ϊdft�任��ʵ����planes[1]Ϊ�鲿��phΪ��λ�� plane_true=magΪ��ֵ
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat planes_true = Mat_<float>(padded);
	Mat ph = Mat_<float>(padded);
	Mat complexImg;
	//��ͨ��complexImg����ʵ�������鲿
	merge(planes, 2, complexImg);
	//���ϱߺϳɵ�mat���и���Ҷ�任,***֧��ԭ�ز���***,����Ҷ�任���Ϊ����.ͨ��1�����ʵ��,ͨ����������鲿
	dft(complexImg, complexImg);
	//�ѱ任��Ľ���ָ����mat,һ��ʵ��,һ���鲿,�����������
	split(complexImg, planes);

	//---------------�˲���Ŀ��Ϊ���õ���ʾ��ֵ---�����ָ�ԭͼʱ�����ٴ���һ��-------------------------
	magnitude(planes[0], planes[1], planes_true);//������mag
	phase(planes[0], planes[1], ph);//��λ��ph
	Mat A = planes[0];
	Mat B = planes[1];
	Mat mag = planes_true;

	mag += Scalar::all(1);//�Է�ֵ��1
	//������ķ�ֵһ��ܴ󣬴ﵽ10^4,ͨ��û�а취��ͼ������ʾ��������Ҫ�������log��⡣
	log(mag, mag);

	//ȡ�����е����ֵ�����ں�����ԭʱȥ��һ��
	double maxVal;
	minMaxLoc(mag, 0, &maxVal, 0, 0);

	//�޼�Ƶ��,���ͼ����л������������Ļ�,����Ƶ���ǲ��ԳƵ�,���Ҫ�޼�
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	ph = ph(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	Mat _magI = mag.clone();
	//�����ȹ�һ��������ʾ��Χ��
	normalize(_magI, _magI, 0, 1, CV_MINMAX);
	//imshow("before rearrange", _magI);

	//��ʾ����Ƶ��ͼ
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	//������������Ϊ��׼����magͼ��ֳ��Ĳ���
	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	normalize(mag, mag, 0, 1, CV_MINMAX);
	//imshow("ԭͼ�Ҷ�ͼ", src);
	//imshow("Ƶ�׷���", mag);
	mag = mag * 255;
	imwrite("ԭƵ��.jpg", mag);
	/*--------------------------------------------------*/

	mag = mag / 255;
	cv::Mat mask;
	Mat proceMag;

	selectPolygon(mag, mask);

	mag = mag.mul(mask);

	proceMag = mag * 255;
	imwrite("������Ƶ��.jpg", proceMag);

	//ǰ�����跴����һ�飬Ŀ����Ϊ����任��ԭͼ
	Mat q00(mag, Rect(0, 0, cx, cy));
	Mat q10(mag, Rect(cx, 0, cx, cy));
	Mat q20(mag, Rect(0, cy, cx, cy));
	Mat q30(mag, Rect(cx, cy, cx, cy));

	//��������
	q00.copyTo(tmp);
	q30.copyTo(q00);
	tmp.copyTo(q30);
	q10.copyTo(tmp);
	q20.copyTo(q10);
	tmp.copyTo(q20);

	mag = mag * maxVal;//����һ���ľ���ԭ 
	exp(mag, mag);//��Ӧ��ǰ��ȥ����
	mag = mag - Scalar::all(1);//��Ӧǰ��+1
	polarToCart(mag, ph, planes[0], planes[1]);//�ɷ�����mag����λ��ph�ָ�ʵ��planes[0]���鲿planes[1]
	merge(planes, 2, complexImg);//��ʵ���鲿�ϲ�


	//-----------------------����Ҷ����任-----------------------------------
	Mat ifft(Size(src.cols, src.rows), CV_8UC1);
	//����Ҷ��任
	idft(complexImg, ifft, DFT_REAL_OUTPUT);
	normalize(ifft, ifft, 0, 1, CV_MINMAX);

	Rect rect(0, 0, src.cols, src.rows);
	dst = ifft(rect);
	dst = dst * 255;

	cv::Mat dspMat;
	dst.convertTo(dspMat, CV_8UC1);
	imshow("dst", dspMat);
	imshow("src", src);
	waitKey(0);

	return 0;

}
int ifftDemo(cv::Mat src,int h, cv::Mat &dspMat)
{
	cv::Mat dst;

	int m = getOptimalDFTSize(src.rows); //2,3,5�ı����и���Ч�ʵĸ���Ҷ�任
	int n = getOptimalDFTSize(src.cols);
	Mat padded;
	//�ѻҶ�ͼ��������Ͻ�,���ұߺ��±���չͼ��,��չ�������Ϊ0;
	copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));
	//planes[0]Ϊdft�任��ʵ����planes[1]Ϊ�鲿��phΪ��λ�� plane_true=magΪ��ֵ
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat planes_true = Mat_<float>(padded);
	Mat ph = Mat_<float>(padded);
	Mat complexImg;
	//��ͨ��complexImg����ʵ�������鲿
	merge(planes, 2, complexImg);
	//���ϱߺϳɵ�mat���и���Ҷ�任,***֧��ԭ�ز���***,����Ҷ�任���Ϊ����.ͨ��1�����ʵ��,ͨ����������鲿
	dft(complexImg, complexImg);
	//�ѱ任��Ľ���ָ����mat,һ��ʵ��,һ���鲿,�����������
	split(complexImg, planes);

	//---------------�˲���Ŀ��Ϊ���õ���ʾ��ֵ---�����ָ�ԭͼʱ�����ٴ���һ��-------------------------
	magnitude(planes[0], planes[1], planes_true);//������mag
	phase(planes[0], planes[1], ph);//��λ��ph
	Mat A = planes[0];
	Mat B = planes[1];
	Mat mag = planes_true;

	mag += Scalar::all(1);//�Է�ֵ��1
	log(mag, mag);//������ķ�ֵһ��ܴ󣬴ﵽ10^4,ͨ��û�а취��ͼ������ʾ��������Ҫ�������log��⡣

				  //ȡ�����е����ֵ�����ں�����ԭʱȥ��һ��
	double maxVal;
	minMaxLoc(mag, 0, &maxVal, 0, 0);

	//�޼�Ƶ��,���ͼ����л������������Ļ�,����Ƶ���ǲ��ԳƵ�,���Ҫ�޼�
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	ph = ph(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	Mat _magI = mag.clone();
	//�����ȹ�һ��������ʾ��Χ��
	normalize(_magI, _magI, 0, 1, CV_MINMAX);
	//imshow("before rearrange", _magI);

	//��ʾ����Ƶ��ͼ
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	//������������Ϊ��׼����magͼ��ֳ��Ĳ���
	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	normalize(mag, mag, 0, 1, CV_MINMAX);
	//imshow("ԭͼ�Ҷ�ͼ", src);
	//imshow("Ƶ�׷���", mag);
	mag = mag * 255;
	imwrite("ԭƵ��.jpg", mag);
	/*--------------------------------------------------*/

	mag = mag / 255;
	cv::Mat mask;
	Mat proceMag;

    selectPolygon(mag, mask);
	if (h) {
		mask = ~mask;
	}
	mag = mag.mul(mask);

	proceMag = mag * 255;
	imwrite("������Ƶ��.jpg", proceMag);

	//ǰ�����跴����һ�飬Ŀ����Ϊ����任��ԭͼ
	Mat q00(mag, Rect(0, 0, cx, cy));
	Mat q10(mag, Rect(cx, 0, cx, cy));
	Mat q20(mag, Rect(0, cy, cx, cy));
	Mat q30(mag, Rect(cx, cy, cx, cy));

	//��������
	q00.copyTo(tmp);
	q30.copyTo(q00);
	tmp.copyTo(q30);
	q10.copyTo(tmp);
	q20.copyTo(q10);
	tmp.copyTo(q20);

	mag = mag * maxVal;//����һ���ľ���ԭ 
	exp(mag, mag);//��Ӧ��ǰ��ȥ����
	mag = mag - Scalar::all(1);//��Ӧǰ��+1
	polarToCart(mag, ph, planes[0], planes[1]);//�ɷ�����mag����λ��ph�ָ�ʵ��planes[0]���鲿planes[1]
	merge(planes, 2, complexImg);//��ʵ���鲿�ϲ�


	//-----------------------����Ҷ����任-----------------------------------
	Mat ifft(Size(src.cols, src.rows), CV_8UC1);
	//����Ҷ��任
	idft(complexImg, ifft, DFT_REAL_OUTPUT);
	normalize(ifft, ifft, 0, 1, CV_MINMAX);

	Rect rect(0, 0, src.cols, src.rows);
	dst = ifft(rect);
	dst = dst * 255;

//	cv::Mat dspMat;
	dst.convertTo(dspMat, CV_8UC1);
	imshow("dst", dspMat);
	imshow("src", src);
	waitKey(0);

	return 0;


}