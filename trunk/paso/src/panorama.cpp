#include "DllControl.h"

#include "Util.h"
#include <opencv2/highgui/highgui.hpp>
#include "panorama.h"
#include <iostream>
using std::vector;
using std::cout;
using std::endl;
using Pano::Util;

int GenPano(char* aSrcPics[], //2����������Դͼ���ļ���(��·��)
			int nPicNum,    //Դͼ��ĸ���
			char* sDestPic,  //����ɵ�ȫ��ͼ���������
			int nDestWidth,  //��ɵ�ͼ���ȣ�nHeight = nWidth/2
struct TPatchPntInfo * pPatPtInfo, //ƥ��㼯
	int nPatchPntsNum,  //ƥ���Եĸ���
	int nCenterPixX,int nCenterPixY, //����ͼ��Բ�������������. 
	int nRadius,  //����ͼ��Բ�İ뾶-1���ǲ����ֳ����⣬��ͬ
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
