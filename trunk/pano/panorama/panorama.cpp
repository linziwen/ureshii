#include "DllControl.h"

#include "Util.h"
#include "opencv2\highgui\highgui.hpp"
#include "Logger.h"
#include "PanoImage.h"
#include <iostream>
#include <map>
#include "PanoAsso.h"
using std::vector;
using std::cout;
using std::endl;
using Pano::Util;
using Pano::PanoImage;
using Pano::Logger;
using std::map;
using Pano::PanoAsso;
using std::stringstream;

int GenPano(char* aSrcPics[], //2个或多个鱼眼源图像文件名(带路径)
			int nPicNum,    //源图像的个数
			char* sDestPic,  //给生成的全景图像起的名字
			int nDestWidth,  //生成的图像宽度，nHeight = nWidth/2
struct TPatchPntInfo * pPatPtInfo, //匹配点集
	int nPatchPntsNum,  //匹配点对的个数
	int nCenterPixX,int nCenterPixY, //鱼眼图像圆的中心像素坐标. 
	int nRadius,  //鱼眼图像圆的半径-1则是不给定又程序检测，上同
	bool bDisplay){

		LOG_INFO("preparing images");
		map<int, PanoImage *> images;
		map<int, PanoAsso> imgAssos;

		for(int i = 0; i<nPicNum; ++i){
			PanoImage * pImg = NULL;
			if (nCenterPixX != -1 && nCenterPixY != -1)
			{
				cv::Point2f center(nCenterPixX,nCenterPixY);
				pImg->setCenter(center);
			}
			if (nRadius != -1)
			{
				pImg = new PanoImage(aSrcPics[i], nRadius);
			}else{
				pImg = new PanoImage(aSrcPics[i], nDestWidth/4);
			}
			images[i+1] = pImg;
		}
		for(int i = 0; i<nPicNum; ++i){
			PanoAsso pa;
			pa.setImg1Id(images[i+1]);
			if(i+1 == nPicNum){
				pa.setImg2Id(images[1]);
			}else{
				pa.setImg2Id(images[i+2]);
			}
			for (int j = 0; j < 3; ++j)
			{
				//int a = pPatPtInfo[i*3+j].nPic1;
				pa.addPatchPoint(pPatPtInfo[i*3+j]);
			}
			pa.calMatA();
			imgAssos[i + 1] = pa;
		}

		LOG_INFO("cal matB");
		C44Matrix matB;
		matB.m_aData[0][0]=1;
		matB.m_aData[1][1]=cos(PI/2);
		matB.m_aData[1][2]=-sin(PI/2);
		matB.m_aData[2][1]=sin(PI/2);
		matB.m_aData[2][2]=cos(PI/2);
		matB.m_aData[3][3] = 1;
		matB.Uni();
		cout<<"finished cal the matrix A and B"<<endl;

		cv::Size outSize(nDestWidth, nDestWidth/2);
		cv::Mat outImg(outSize,CV_8UC3);


		int rows = outImg.rows;
		int cols = outImg.cols;

		stringstream ss;
		ss << "rows=" << rows << "  cols=" << outImg.cols << " channels=" << outImg.channels();
		LOG_INFO(ss.str());
		ss.str("");
		for (int y = 0; y < outImg.rows; ++y)
		{
			uchar *ptr = outImg.ptr<uchar>(y);
			for(int x = 0; x<cols; ++x){
				double theta = x*(2*PI/cols);//theta(0,2PI)
				double phi = (rows/2-y)*(PI/rows);//phi(-2/PI, 2/PI)
				double xp,yp,zp;
				xp = cos(theta)*cos(phi);
				yp = sin(theta)*cos(phi);
				zp = sin(phi);

				vector<CvPoint3D32f> points;
				C44Matrix matXp,matX1,matRecB;
				double dDet;

				matRecB = matB;
				dDet = matRecB.Rec();
				matRecB.Uni();
				if (dDet == 0)
				{
					LOG_INFO("matB can not be inversed");
					return -1;
				}

				LOG_INFO("cal fisheye 3d pos respectively");
				imgAssos[1].setSpherePos(Util::matXpoint(matRecB, cv::Point3f(xp, yp, zp)));
				for(int k = 0; k < nPicNum - 1; ++k){
					imgAssos[k+2].setSpherePos(Util::matXpoint(imgAssos[k+1].getMatA() , imgAssos[k+1].getSpherePos()));
				}
				for(int k = 0; k < nPicNum - 1; ++k){
					cv::Point3f point = imgAssos[k+1].getSpherePos();
					double theta1 = atan2(point.y, point.x);
					double phi1 = atan2(point.z, sqrt(pow(point.x,2) + pow(point.y, 2)));
					if (theta1<0)
					{
						theta1 += 2*PI;
					}


				}
				//ptr[x*3] = 0;
				//ptr[x*3 + 1] = 0;
				//ptr[x*3 + 2] = 255;
				//LOG_INFO(ss.str());
			}
		}

		cv::imwrite("out.jpg",outImg);
		LOG_INFO("release phase");
		for(int i = 0; i<nPicNum; ++i){
			images[i+1] = NULL;
			delete images[i+1];
		}
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