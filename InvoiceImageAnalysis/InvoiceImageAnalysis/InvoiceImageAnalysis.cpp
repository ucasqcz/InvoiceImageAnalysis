// InvoiceImageAnalysis.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <io.h>
#include <direct.h>

#include <cv.h>
#include <opencv2/opencv.hpp>
#include <highgui.h>



#include "AntigerResize.h"
#include "AntigerColor.h"
#include "AntigerBin.h"
#include "AntigerEdge.h"
#include "AntigerEllipse.h"
#include "AntigerBarcode.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	//if (argc != 2)
	//{
	//	printf("\n正确用法: %s <image directory name>\n", argv[0]);
	//	return (-1);
	//}

	string imgFold = "F:\\qcz_pro\\实验室项目\\格式文档\\飞机票\\data\\Original";

	//计时变量
	long long t1, t2;
	double duration, TickFrequency;
	TickFrequency = 1.0 / (getTickFrequency()*1e-3);

	//文件名相关
	char sFileNameStem[32],sInputFileName[MAX_PATH], sOutputFileName[MAX_PATH];
	CreateDirectory(TEXT("Img_Out"), NULL);

	//图像变量
	Mat InputImg, NormImg, RfImg, EdgeImg, RedImg, GrayImg, BgImg, EnhancedImg;

	// random forest init
	
	//RandomForest edgeRF;

	//printf("读取随机森林------\n");
	//t1 = getTickCount();
	//CreateRandomForest(edgeRF, "model.yml");
	//t2 = getTickCount();
	//duration = ((double)(t2 - t1)) * TickFrequency;
	//printf("花费 %.4f ms!\n\n", duration);
	//
	vector<Barcode> barcodes;
	CBarcodeReader barcodeReader;
	HANDLE hBarcode;
	InitBarcodeReader(barcodeReader, hBarcode);

	int i;

	ofstream outfile("failure.txt");

	string barcodeFold = "Img_Out\\Barcode";
	if (_access(barcodeFold.c_str(), 0)){ _mkdir(barcodeFold.c_str()); }
	string sealFold = "Img_Out\\Seal";
	if (_access(sealFold.c_str(), 0)){ _mkdir(sealFold.c_str()); }


	for (i = 1; i <= 200; i++)
	{
		sprintf(sFileNameStem, "img%05d", i);
		sprintf(sInputFileName, "%s\\%s.jpg", imgFold.c_str(), sFileNameStem);

		////////////////////////////////////////////////////////////        
		//1, 读取图像
		////////////////////////////////////////////////////////////
		//printf("读取图像( %s )------\n", sFileNameStem);
		t1 = getTickCount();
		InputImg = cv::imread(sInputFileName);
		t2 = getTickCount();
		duration = ((double)(t2 - t1)) * TickFrequency;
		//printf("花费 %.4f ms!\n\n", duration);

		////////////////////////////////////////////////////////////        
		//2, 读取条码
		////////////////////////////////////////////////////////////
		//printf("读取条码------\n");

		t1 = getTickCount();
		BarcodeRead(InputImg, barcodeReader, hBarcode, barcodes);
		
		t2 = getTickCount();
		duration = ((double)(t2 - t1)) * TickFrequency;
		//printf("花费 %.4f ms!\n\n", duration);
		if (barcodes.size() == 0){
			printf("读取图像( %s )中的条码失败------花费 %.4f ms!\n", sFileNameStem, duration);
			outfile << "read barcode failed --- image : " << sFileNameStem << endl;
		}
		else
			printf("读取图像( %s )中的条码成功%s------花费 %.4f ms!\n", sFileNameStem, barcodes[0].sBarCodeData, duration);
		Mat saveInputImg;
		InputImg.copyTo(saveInputImg);
		for (int ind = 0; ind < barcodes.size(); ind++){ rectangle(saveInputImg, barcodes[ind].rcBarcodeRegion, Scalar(0, 0, 255), 3); }
		for (int j = 0; j < barcodes.size(); j++){
			string barcodeStr(barcodes[j].sBarCodeData);
			putText(saveInputImg, barcodeStr, Point(barcodes[j].rcBarcodeRegion.x - 300, barcodes[j].rcBarcodeRegion.y + barcodes[j].rcBarcodeRegion.height), FONT_HERSHEY_COMPLEX, 2, Scalar(0, 0, 255), 3);
		}

		char saveBarcodeImgPath[MAX_PATH];
		sprintf(saveBarcodeImgPath, "%s\\%s_barcode.jpg", barcodeFold.c_str(),sFileNameStem);
		imwrite(saveBarcodeImgPath, saveInputImg);


		////////////////////////////////////////////////////////////        
		////2, 图像大小归一化
		////////////////////////////////////////////////////////////

		printf("保持长宽比归一化图像到长边640------\n");
		t1 = getTickCount();
		//如果出租车票横着拍摄，则先逆时针旋转90度
		int w, h;
		w = InputImg.size().width;
		h = InputImg.size().height;
		if ( w > h)
		{
			transpose(InputImg, InputImg);
			flip(InputImg, InputImg, 0);
		}
		double ratio;
		ratio = ImgResizeWithFixedLongside(InputImg, NormImg, 640);
		t2 = getTickCount();
		duration = ((double)(t2 - t1)) * TickFrequency;
		printf("倍数：%f, 花费 %.4f ms!\n\n", ratio, duration);

		//sprintf(sOutputFileName, "Img_Out\\%s_norm.jpg", sFileNameStem);
		//cv::imwrite(sOutputFileName, NormImg);

		//////////////////////////////////////////////////////////////        
		//////3, 随机森林边缘检测
		//////////////////////////////////////////////////////////////
		{
			////printf("随机森林边缘检测------\n");
			////t1 = getTickCount();
			////RandomForestEdges(NormImg, RfImg, edgeRF);
			////CannyEdges(NormImg, RfImg, EdgeImg);
			////t2 = getTickCount();
			////duration = ((double)(t2 - t1)) * TickFrequency;
			////printf("花费 %.4f ms!\n\n", duration);

			////sprintf(sOutputFileName, "Img_Out\\%s_rf.jpg", sFileNameStem);
			////cv::imwrite(sOutputFileName, RfImg);
			////sprintf(sOutputFileName, "Img_Out\\%s_edge.jpg", sFileNameStem);
			////cv::imwrite(sOutputFileName, EdgeImg);
		}
		////////////////////////////////////////////////////////////        
		////4, 红色通道提取
		////////////////////////////////////////////////////////////

		printf("红色通道提取------\n");
		t1 = getTickCount();
		//Mat YUVImg;
		//cv::cvtColor(NormImg, YUVImg, CV_BGR2YUV);
		//std::vector<cv::Mat> YUVChannels(3);
		//cv::split(YUVImg, YUVChannels);
		//GrayImg = YUVChannels[0];
		//FastBgEstimation(GrayImg, BgImg,16);
		//BgCompensation(GrayImg, BgImg, EnhancedImg);
		//YUVChannels[0] = EnhancedImg;
		//cv::merge(YUVChannels, YUVImg);
		//cv::cvtColor(YUVImg, NormImg, CV_YUV2BGR);

		//FastDecolorization(NormImg, RedImg);
		GetRednessMap(NormImg, RedImg);
		t2 = getTickCount();
		duration = ((double)(t2 - t1)) * TickFrequency;
		printf("花费 %.4f ms!\n\n", duration);

		//sprintf(sOutputFileName, "Img_Out\\%s_red.jpg", sFileNameStem);
		//cv::imwrite(sOutputFileName, RedImg);

		////////////////////////////////////////////////////////////        
		////4, 椭圆红章检测
		////////////////////////////////////////////////////////////
		printf("椭圆红章检测------\n");
		t1 = getTickCount();
		// Parameters Settings (Sect. 4.2)
		Size sz = RedImg.size();
		int		iThLength = 16; //16
		float	fThObb = 3.0f;
		float	fThPos = 1.0f;
		float	fTaoCenters = 0.05f;
		int 	iNs = 16;
		float	fMaxCenterDistance = sqrt(float(sz.width*sz.width + sz.height*sz.height)) * fTaoCenters;

		float	fThScoreScore = 0.5f;//0.5f

		// Other constant parameters settings. 

		// Gaussian filter parameters, in pre-processing
		Size	szPreProcessingGaussKernelSize = Size(5, 5);
		double	dPreProcessingGaussSigma = 1.0;

		float	fDistanceToEllipseContour = 0.1f;	// (Sect. 3.3.1 - Validation)
		float	fMinReliability = 0.7f;	// Const parameters to discard bad ellipses 0.54， 0.7


		// Initialize Detector with selected parameters
		CEllipseDetectorYaed yaed;
		yaed.SetParameters(szPreProcessingGaussKernelSize,
			dPreProcessingGaussSigma,
			fThPos,
			fMaxCenterDistance,
			iThLength,
			fThObb,
			fDistanceToEllipseContour,
			fThScoreScore,
			fMinReliability,
			iNs
			);
		// Detect
		vector<struct Ellipse> ellsYaed;
		Mat1b gray;
		gray = RedImg;
		yaed.Detect(gray, ellsYaed);
		t2 = getTickCount();
		duration = ((double)(t2 - t1)) * TickFrequency;
		printf("花费 %.4f ms!\n\n", duration);
			
		Mat3b resultImage = NormImg.clone();
		yaed.DrawDetectedEllipses(resultImage, ellsYaed,0,1);
			

		sprintf(sOutputFileName, "%s\\%s_seal.jpg", sealFold.c_str(),sFileNameStem);
		cv::imwrite(sOutputFileName, resultImage);

		////////////////////////////////////////////////////////////        
		////3, 图像灰度化
		////////////////////////////////////////////////////////////
		//printf("图像灰度化------\n");
		//t1 = getTickCount();
		//FastDecolorization(NormImg, GrayImg);
		//t2 = getTickCount();
		//duration = ((double)(t2 - t1)) * TickFrequency;
		//printf("花费 %.4f ms!\n\n", duration);

		//sprintf(sOutputFileName, "Img_Out\\%s_gray.bmp", sFileNameStem);
		//cv::imwrite(sOutputFileName, GrayImg);

		////////////////////////////////////////////////////////////        
		////4, 背景估计与补偿
		////////////////////////////////////////////////////////////
		//printf("背景估计与补偿------\n");
		//t1 = getTickCount();
		//FastBgEstimation(GrayImg, BgImg,8);
		//BgCompensation(GrayImg, BgImg, EnhancedImg);
		//t2 = getTickCount();
		//duration = ((double)(t2 - t1)) * TickFrequency;
		//printf("花费 %.4f ms!\n\n", duration);

		//sprintf(sOutputFileName, "Img_Out\\%s_en.bmp", sFileNameStem);
		//cv::imwrite(sOutputFileName, EnhancedImg);
	}

	//DeleteRandomForest(edgeRF);
    return 0;
}

