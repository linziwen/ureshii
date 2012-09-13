#define DLL_API extern "C" __declspec(dllexport)

#include "Util.h"
#include "highgui.h"

using std::vector;
using std::cout;
using std::endl;
using Pano::Util;

int GenPano(char* aSrcPics[], //2个或多个鱼眼源图像文件名(带路径)
			int nPicNum,    //源图像的个数
			char* sDestPic,  //给生成的全景图像起的名字
			int nDestWidth,  //生成的图像宽度，nHeight = nWidth/2
struct TPatchPntInfo * pPatPtInfo, //匹配点集
	int nPatchPntsNum,  //匹配点对的个数
	int nCenterPixX,int nCenterPixY, //鱼眼图像圆的中心像素坐标. 
	int nRadius,  //鱼眼图像圆的半径-1则是不给定又程序检测，上同
	bool bDisplay){



		return 0;
}


int GenSphereFromCube( char* sCubeTopFilePathName
					  , char* sCubeBottomFilePathName
					  , char* sCubeFrontFilePathName
					  , char* sCubeBackFilePathName
					  , char* sCubeLeftFilePathName
					  , char* sCubeRightFilePathName
					  , char* sSphereFilenPathName
					  ,  int nSpherePicWidth,  bool bDisplay)
{
	std::cout<<"begin generating sphere form cube ..."<<std::endl;
	IplImage *sphereImg = cvCreateImage(cvSize(nSpherePicWidth,nSpherePicWidth/2),IPL_DEPTH_8U,3);
	vector<IplImage *> cubeImg;
	cubeImg.push_back(cvLoadImage(sCubeRightFilePathName));
	cubeImg.push_back(cvLoadImage(sCubeBackFilePathName));
	cubeImg.push_back(cvLoadImage(sCubeLeftFilePathName));
	cubeImg.push_back(cvLoadImage(sCubeFrontFilePathName));
	cubeImg.push_back(cvLoadImage(sCubeTopFilePathName));
	cubeImg.push_back(cvLoadImage(sCubeBottomFilePathName));

	cvFlip(cubeImg[4],cubeImg[4],0);

	int edge = cubeImg[0]->height;

	for (int n = 0; n<6; ++n)
	{
		for (int y = 0; y<edge; ++y)
		{
			uchar *ptr=(uchar *)(cubeImg[n]->imageData + y * cubeImg[n]->widthStep);
			for (int x = 0; x<edge; ++x)
			{
				double xc, yc;
				xc = x - edge/2;
				yc = (-y) + edge/2;

				double theta, phy;
				if (n == 4)
				{
					theta = atan2(yc,xc);
					phy = atan2(double(edge),2 * sqrt(xc*xc+yc*yc));
				}else if(n == 5){
					theta = atan2(yc,xc);
					phy = atan2(double(-edge),2 * sqrt(xc*xc+yc*yc));
				}else{
					theta = n*PI/2 - atan2(xc,edge/2);
					phy = atan2(yc,sqrt(xc*xc+(edge/2)*(edge/2)));
				}

				if (theta<0)
				{
					theta += 2*PI;
				}

				int xp, yp;
				xp = int(theta * sphereImg->width/(2*PI));
				yp = int(sphereImg->height/2 - phy * sphereImg->height/PI);

				uchar* pixelColor = &((uchar*)(sphereImg->imageData + sphereImg->widthStep*yp))[xp*3];
				pixelColor[0] = ptr[x*3];
				pixelColor[1] = ptr[x*3 + 1];
				pixelColor[2] = ptr[x*3 + 2];

			}
		}
	}


	cvSaveImage(sSphereFilenPathName,sphereImg);
	if (bDisplay)
	{
		cvNamedWindow("sphere",CV_WINDOW_AUTOSIZE);
		cvShowImage("sphere",sphereImg);
		cvWaitKey(0);
	}

	cvReleaseImage(&sphereImg);
	cvDestroyWindow("sphere");
	for (int num = 0; num<6; num++)
	{
		cvReleaseImage(&cubeImg[num]);
	}
	std::cout<<"finished generating sphere form cube ..."<<std::endl;
	return 1;

}