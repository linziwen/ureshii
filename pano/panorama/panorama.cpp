#define DLL_API extern "C" __declspec(dllexport)

#include "Util.h"

using std::vector;
using std::cout;
using std::endl;

int GenPano(char* aSrcPics[], //2个或多个鱼眼源图像文件名(带路径)
			int nPicNum,    //源图像的个数
			char* sDestPic,  //给生成的全景图像起的名字
			int nDestWidth,  //生成的图像宽度，nHeight = nWidth/2
struct TPatchPntInfo * pPatPtInfo, //匹配点集
	int nPatchPntsNum,  //匹配点对的个数
	int nCenterPixX,int nCenterPixY, //鱼眼图像圆的中心像素坐标. 
	int nRadius,  //鱼眼图像圆的半径-1则是不给定又程序检测，上同
	bool bDisplay){




		vector<IplImage*> plImageVec;
		for (int imageNum = 0;imageNum<nPicNum;++imageNum)
		{
			IplImage *img = cvLoadImage(aSrcPics[imageNum]);
			plImageVec.push_back(img);
		}

		cout<<"begin to cal the center and radius"<<endl;
		int findR;
		int sumR = 0;
		CvPoint findCenter;
		CvPoint sumCenter = cvPoint(0,0);

		for (int i = 0; i<nPicNum; ++i){
			findImageRadiusAndCenter(plImageVec[i],findR,findCenter);
			sumR += findR;
			sumCenter.x += findCenter.x;
			sumCenter.y += findCenter.y;
		}

		if ((nCenterPixX == -1) && (nCenterPixY == -1))
		{
			nCenterPixX = sumCenter.x/nPicNum;
			nCenterPixY = sumCenter.y/nPicNum;
		}
		if (nRadius == -1)
		{
			nRadius = sumR/nPicNum;
		}

		int rmax = nRadius;
		int r,r180,rmin;
		if (nPicNum==2)
		{
			rmin = rmax*(180-(180-360/nPicNum)-5)/192;
			r180 = rmax*180/192;
		}

		if (nPicNum==3)
		{
			r180 = rmax*175/180;
			rmin = rmax*(4-nPicNum)/nPicNum-8;
		} 

		cout<<"finished cal the center and radius"<<endl;
		cout<<"begin to set the matrix a and b"<<endl;
		cout<<"1 center the original point from lefttop to img's center and recal the match point position"<<endl;
		for (int pt=0;pt<nPatchPntsNum;++pt)
		{
			pPatPtInfo[pt].x1 = pPatPtInfo[pt].x1 - nCenterPixX;
			pPatPtInfo[pt].y1 = (-pPatPtInfo[pt].y1) + nCenterPixY;
			pPatPtInfo[pt].x2 = pPatPtInfo[pt].x2 - nCenterPixX;
			pPatPtInfo[pt].y2 = (-pPatPtInfo[pt].y2) + nCenterPixY;

		}

		cout<<"2 change 2d fisheye position to 3d position"<<endl;
		vector<CvPoint3D32f> pointVec1;
		vector<CvPoint3D32f> pointVec2;
		for (int i=0; i<nPatchPntsNum;++i)
		{
			double r1 = sqrt(pow(double(pPatPtInfo[i].y1),2)+pow(double(pPatPtInfo[i].x1),2));
			double r2 = sqrt(pow(double(pPatPtInfo[i].y2),2)+pow(double(pPatPtInfo[i].x2),2));
			double r0 = (r1+r2)/2;
			double theta1 = atan2(double(pPatPtInfo[i].y1),double(pPatPtInfo[i].x1));
			double theta2 = atan2(double(pPatPtInfo[i].y2),double(pPatPtInfo[i].x2));

			double phy1;
			double phy2;
			if (nPicNum==2)
			{
				phy1 = PI/2*(1-r1/r0);
				phy2 = PI/2*(1-r2/r0);
			}
			if (nPicNum==3)
			{
				phy1 = PI/2*(1-r1/r180);
				phy2 = PI/2*(1-r2/r180);
			}

			float x1,y1,z1,x2,y2,z2;
			x1 = float(cos(theta1)*cos(phy1));
			y1 = float(sin(theta1)*cos(phy1));
			z1 = float(sin(phy1));
			x2 = float(cos(theta2)*cos(phy2));
			y2 = float(sin(theta2)*cos(phy2));
			z2 = float(sin(phy2));
			pointVec1.push_back(cvPoint3D32f(x1,y1,z1));
			pointVec2.push_back(cvPoint3D32f(x2,y2,z2));
		}
		cout<<"3 cal the matricx A to N ,the order by default is from 12,23,34,...,(n-1)n,n1"<<endl;
		vector<C44Matrix> matVecA;
		for(int i = 0 ; i < nPatchPntsNum ; i+=3)
		{
			double **ppA,*pA[9],dDet;
			double A[9][10]={
				pointVec1[i].x,pointVec1[i].y,pointVec1[i].z,0,0,0,0,0,0,pointVec2[i].x,
				0,0,0,pointVec1[i].x,pointVec1[i].y,pointVec1[i].z,0,0,0,pointVec2[i].y,
				0,0,0,0,0,0,pointVec1[i].x,pointVec1[i].y,pointVec1[i].z,pointVec2[i].z,
				pointVec1[i+1].x,pointVec1[i+1].y,pointVec1[i+1].z,0,0,0,0,0,0,pointVec2[i+1].x,
				0,0,0,pointVec1[i+1].x,pointVec1[i+1].y,pointVec1[i+1].z,0,0,0,pointVec2[i+1].y,
				0,0,0,0,0,0,pointVec1[i+1].x,pointVec1[i+1].y,pointVec1[i+1].z,pointVec2[i+1].z,
				pointVec1[i+2].x,pointVec1[i+2].y,pointVec1[i+2].z,0,0,0,0,0,0,pointVec2[i+2].x,
				0,0,0,pointVec1[i+2].x,pointVec1[i+2].y,pointVec1[i+2].z,0,0,0,pointVec2[i+2].y,
				0,0,0,0,0,0,pointVec1[i+2].x,pointVec1[i+2].y,pointVec1[i+2].z,pointVec2[i+2].z,
			};
			for (int k=0;k<9;++k)
			{
				pA[k] = A[k];
			}
			ppA = pA;
			dDet = SoluteEquation(ppA,8,9);
			if (ERR == dDet)
			{
				cout<<"devoke error"<<endl;
			}
			C44Matrix mat;
			mat.m_aData[0][0] = ppA[0][9];
			mat.m_aData[0][1] = ppA[1][9];
			mat.m_aData[0][2] = ppA[2][9];
			mat.m_aData[1][0] = ppA[3][9];
			mat.m_aData[1][1] = ppA[4][9];
			mat.m_aData[1][2] = ppA[5][9];
			mat.m_aData[2][0] = ppA[6][9];
			mat.m_aData[2][1] = ppA[7][9];
			mat.m_aData[2][2] = ppA[8][9];
			mat.m_aData[3][3] = 1;

			mat.Uni();
			matVecA.push_back(mat);

		}
		cout<<"4 cal the matrix b"<<endl;
		C44Matrix matB;
		matB.m_aData[0][0]=1;
		matB.m_aData[1][1]=cos(PI/2);
		matB.m_aData[1][2]=-sin(PI/2);
		matB.m_aData[2][1]=sin(PI/2);
		matB.m_aData[2][2]=cos(PI/2);
		matB.m_aData[3][3] = 1;
		matB.Uni();
		cout<<"finished cal the matrix A and B"<<endl;

		CvSize imgsize=cvSize(nDestWidth,nDestWidth/2);
		IplImage *out=cvCreateImage(imgsize , IPL_DEPTH_8U,3);

		cout<<"begin fetch the color and assign it to the output image"<<endl;
		for (int y = 0; y<out->height; ++y)
		{

			uchar *ptr=(uchar *)(out->imageData+y*out->widthStep);
			for (int x = 0;x<out->width;++x)
			{
				double theta = x*(2*PI/out->width);
				double phy = (out->height/2-y)*(PI/out->height);

				double xp,yp,zp;
				xp = cos(theta)*cos(phy);
				yp = sin(theta)*cos(phy);
				zp = sin(phy);

				vector<CvPoint3D32f> points;
				C44Matrix matXp,matX1,matRecB;
				double dDet;

				matRecB = matB;
				dDet = matRecB.Rec();
				matRecB.Uni();
				if (dDet == 0)
				{
					cout<<"matB can not be inversed"<<endl;
					return -1;
				}

				for (int n=0; n < nPicNum;++n)
				{
					if (n == 0)
					{
						matXp.m_aData[0][0] = xp;
						matXp.m_aData[1][0] = yp;
						matXp.m_aData[2][0] = zp;
						matXp.m_aData[3][0] = 1;
						matX1 = matRecB*matXp;
						points.push_back(cvPoint3D32f(matX1.m_aData[0][0],matX1.m_aData[1][0],matX1.m_aData[2][0]));
					}else{
						C44Matrix matPrev,matNext;
						matPrev.m_aData[0][0] = points[n-1].x;
						matPrev.m_aData[1][0] = points[n-1].y;
						matPrev.m_aData[2][0] = points[n-1].z;
						matPrev.m_aData[3][0] = 1;
						matNext = matVecA[n-1] *matPrev;
						points.push_back(cvPoint3D32f(matNext.m_aData[0][0],matNext.m_aData[1][0],matNext.m_aData[2][0]));
					}
				}
				for (int n=0; n<nPicNum;++n)
				{
					double phy1,r1,theta1,r2,theta2,phy2;
					phy1 = atan2(points[n].z,sqrt(points[n].x*points[n].x+points[n].y*points[n].y));
					theta1 = atan2(points[n].y,points[n].x);



					if (theta1<0)
					{
						theta1 += 2*PI;
					}
					r1 = r180*(PI/2-phy1)/(PI/2);
					double rx,rxabs,ry,ryabs;
					rx = r1*cos(theta1);
					ry = r1*sin(theta1);
					rxabs = abs(rx);
					ryabs = abs(ry);
		
					if (r1<rmin)
					{
						double xi,yi;//image coord which the original point is in the center of the image
						int xcv,ycv;//image coord which the original point is in the left top of the image
						xi = r1*cos(theta1);
						yi = r1*sin(theta1);
						xcv = int(xi + nCenterPixX);
						ycv = int((-yi) + nCenterPixY);
						if (ycv<0)
						{
							ycv=0;
						}
						uchar* pixelColor = &((uchar*)(plImageVec[n]->imageData + plImageVec[n]->widthStep*ycv))[xcv*3];
						ptr[x*3] = pixelColor[0];
						ptr[x*3+1] = pixelColor[1];
						ptr[x*3+2] = pixelColor[2];

						break;
					}
					else if (r1>rmin && r1<=rmax && rx>=0){
						int firstPic = n+1;
						int secondPic = (firstPic+1)>nPicNum ? 1 : (firstPic+1);
						for (int ptnum=0 ; ptnum<nPatchPntsNum; ++ptnum)
						{
							if ((firstPic == pPatPtInfo[ptnum].nPic1) && (secondPic == pPatPtInfo[ptnum].nPic2))
							{
								phy2 = atan2(points[secondPic-1].z,sqrt(points[secondPic-1].x*points[secondPic-1].x+points[secondPic-1].y*points[secondPic-1].y));
								theta2 = atan2(points[secondPic-1].y,points[secondPic-1].x);

								if (theta2<0)
								{
									theta2 += 2*PI;
								}

								r2 = r180*(PI/2-phy2)/(PI/2);
								float p2ex = rmax*cos(theta2);
								float p2ey = rmax*sin(theta2);
								CvPoint2D32f p2e = cvPoint2D32f(p2ex,p2ey);
								CvPoint2D32f p2e1,p1,p1e;
								C44Matrix transmat = matVecA[firstPic-1];//TODO:problem
								transmat.Rec();
								transmat.Uni();
								findCounterpartPt(p2e,transmat,rmax,r180,p2e1);
								p1.x = float(r1*cos(theta1));
								p1.y = float(r1*sin(theta1));
								p1e.x = float(rmax*cos(theta1));
								p1e.y = float(rmax*sin(theta1));

								double dist1,distAll,k1,k2;
								dist1 = sqrt((p1.x-p1e.x)*(p1.x-p1e.x)+(p1.y-p1e.y)*(p1.y-p1e.y));
								distAll = sqrt((p1e.x-p2e1.x)*(p1e.x-p2e1.x)+(p1e.y-p2e1.y)*(p1e.y-p2e1.y));
								k1 = dist1/distAll;
								k2 = 1-k1;

								double xi1,yi1,xi2,yi2;
								int xcv1,ycv1,xcv2,ycv2;
								xi1 = r1*cos(theta1);
								yi1 = r1*sin(theta1);
								xcv1 = int(xi1 + nCenterPixX);
								ycv1 = int((-yi1) + nCenterPixY);
								xi2 = r2*cos(theta2);
								yi2 = r2*sin(theta2);
								xcv2 = int(xi2 + nCenterPixX);
								ycv2 = int((-yi2) + nCenterPixY);



								uchar* pixelColor1 = &((uchar*)(plImageVec[pPatPtInfo[ptnum].nPic1-1]->imageData + plImageVec[pPatPtInfo[ptnum].nPic1-1]->widthStep*ycv1))[xcv1*3];
								uchar* pixelColor2 = &((uchar*)(plImageVec[pPatPtInfo[ptnum].nPic2-1]->imageData + plImageVec[pPatPtInfo[ptnum].nPic2-1]->widthStep*ycv2))[xcv2*3];
									
								if (k1>1)
								{
									k1=1;
									k2=0;
								}
								ptr[x*3] = pixelColor1[0]*k1+pixelColor2[0]*k2;
								ptr[x*3+1] = pixelColor1[1]*k1+pixelColor2[1]*k2;
								ptr[x*3+2] = pixelColor1[2]*k1+pixelColor2[2]*k2;

							break;
							}
						}
						break;
					}
					else if(r1>rmin && r1<=rmax && rx<=0){
						int secondPic = n+1;
						int firstPic = n > 0 ? n : nPicNum ;
						for (int ptnum=0 ; ptnum<nPatchPntsNum; ++ptnum)
						{
							if ((firstPic == pPatPtInfo[ptnum].nPic1) && (secondPic == pPatPtInfo[ptnum].nPic2))
							{
								phy2 = atan2(points[firstPic-1].z,sqrt(points[firstPic-1].x*points[firstPic-1].x+points[firstPic-1].y*points[firstPic-1].y));
								theta2 = atan2(points[firstPic-1].y,points[firstPic-1].x);

								if (theta2<0)
								{
									theta2 += 2*PI;
								}

								r2 = r180*(PI/2-phy2)/(PI/2);
								float p2ex = rmax*cos(theta1);
								float p2ey = rmax*sin(theta1);
								CvPoint2D32f p2e = cvPoint2D32f(p2ex,p2ey);
								CvPoint2D32f p2e1,p1,p1e;
								C44Matrix transmat = matVecA[firstPic-1];//TODO:problem
								transmat.Rec();
								transmat.Uni();
								findCounterpartPt(p2e,transmat,rmax,r180,p2e1);
								p1.x = float(r2*cos(theta2));
								p1.y = float(r2*sin(theta2));
								p1e.x = float(rmax*cos(theta2));
								p1e.y = float(rmax*sin(theta2));

								double dist1,distAll,k1,k2;
								dist1 = sqrt((p1.x-p1e.x)*(p1.x-p1e.x)+(p1.y-p1e.y)*(p1.y-p1e.y));
								distAll = sqrt((p1e.x-p2e1.x)*(p1e.x-p2e1.x)+(p1e.y-p2e1.y)*(p1e.y-p2e1.y));
								k1 = dist1/distAll;
								k2 = 1-k1;

								double xi1,yi1,xi2,yi2;
								int xcv1,ycv1,xcv2,ycv2;
								xi1 = r2*cos(theta2);
								yi1 = r2*sin(theta2);
								xcv1 = int(xi1 + nCenterPixX);
								ycv1 = int((-yi1) + nCenterPixY);
								xi2 = r1*cos(theta1);
								yi2 = r1*sin(theta1);
								xcv2 = int(xi2 + nCenterPixX);
								ycv2 = int((-yi2) + nCenterPixY);

								uchar* pixelColor1 = &((uchar*)(plImageVec[pPatPtInfo[ptnum].nPic1-1]->imageData + plImageVec[pPatPtInfo[ptnum].nPic1-1]->widthStep*ycv1))[xcv1*3];
								uchar* pixelColor2 = &((uchar*)(plImageVec[pPatPtInfo[ptnum].nPic2-1]->imageData + plImageVec[pPatPtInfo[ptnum].nPic2-1]->widthStep*ycv2))[xcv2*3];


								if (k1>1)
								{
									k1=1;
									k2=0;
								}

								ptr[x*3] = pixelColor1[0]*k1+pixelColor2[0]*k2;
								ptr[x*3+1] = pixelColor1[1]*k1+pixelColor2[1]*k2;
								ptr[x*3+2] = pixelColor1[2]*k1+pixelColor2[2]*k2;

								break;
							}
						}
						break;
					}
				}
			}

		}
		cout<<"finished fetch the color and assign it to the output image"<<endl;
		cvSaveImage(sDestPic,out);

		return 0;
}



int GenCubeFromSphere( char* sSphereFilePathName,  char* sCubeFilenPathPreName, int nCubePicWidth, bool bDisplay)
{
	std::cout<<"generating cube from sphere..."<<std::endl;
	IplImage *img = cvLoadImage(sSphereFilePathName);
	vector<IplImage *> outImgVec;

	for (int n = 0; n < 6; ++n)
	{
		outImgVec.push_back(genOnePlane(img,nCubePicWidth,n));
	}

	for (int n = 0; n < 6; ++n)
	{
		char filename[500];
		if (n == 4)
		{
			sprintf(filename,".//cube//%s_top.jpg",sCubeFilenPathPreName);
			cvFlip(outImgVec[n],outImgVec[n],0);
			cvSaveImage(filename,outImgVec[n]);
		}
		if (n == 5)
		{
			sprintf(filename,".//cube//%s_bottom.jpg",sCubeFilenPathPreName);
			cvSaveImage(filename,outImgVec[n]);
		}
		if (n == 0)
		{
			sprintf(filename,".//cube//%s_right.jpg",sCubeFilenPathPreName);
			cvSaveImage(filename,outImgVec[n]);
		}
		if (n == 1)
		{
			sprintf(filename,".//cube//%s_back.jpg",sCubeFilenPathPreName);
			cvSaveImage(filename,outImgVec[n]);
		}
		if (n == 2)
		{
			sprintf(filename,".//cube//%s_left.jpg",sCubeFilenPathPreName);
			cvSaveImage(filename,outImgVec[n]);
		}
		if (n == 3)
		{
			sprintf(filename,".//cube//%s_front.jpg",sCubeFilenPathPreName);
			cvSaveImage(filename,outImgVec[n]);
		}
	}
	cvReleaseImage(&img);


	IplImage *displayImg = cvCreateImage(cvSize(nCubePicWidth*4,nCubePicWidth*3),IPL_DEPTH_8U,3);

	for (int num = 0; num<6; num++)
	{
		if (num == 0)
		{
			cvSetImageROI(displayImg,cvRect(nCubePicWidth*2,nCubePicWidth,nCubePicWidth,nCubePicWidth));
			cvCopy(outImgVec[num],displayImg);
			cvResetImageROI(displayImg);
		}
		if (num == 1)
		{
			cvSetImageROI(displayImg,cvRect(nCubePicWidth,nCubePicWidth,nCubePicWidth,nCubePicWidth));
			cvCopy(outImgVec[num],displayImg);
			cvResetImageROI(displayImg);
		}
		if (num == 2)
		{
			cvSetImageROI(displayImg,cvRect(0,nCubePicWidth,nCubePicWidth,nCubePicWidth));
			cvCopy(outImgVec[num],displayImg);
			cvResetImageROI(displayImg);
		}
		if (num == 3)
		{
			cvSetImageROI(displayImg,cvRect(nCubePicWidth*3,nCubePicWidth,nCubePicWidth,nCubePicWidth));
			cvCopy(outImgVec[num],displayImg);
			cvResetImageROI(displayImg);
		}
		if (num == 4)
		{
			cvSetImageROI(displayImg,cvRect(nCubePicWidth,0,nCubePicWidth,nCubePicWidth));
			cvCopy(outImgVec[num],displayImg);
			cvResetImageROI(displayImg);
		}
		if (num == 5)
		{
			cvSetImageROI(displayImg,cvRect(nCubePicWidth,nCubePicWidth*2,nCubePicWidth,nCubePicWidth));
			cvCopy(outImgVec[num],displayImg);
			cvResetImageROI(displayImg);
		}
	}
	cvSaveImage(".//cube//allTogether.jpg",displayImg);

	if (bDisplay)
	{


		IplImage *resize = cvCreateImage(cvSize(800,600),IPL_DEPTH_8U,3);
		cvResize(displayImg,resize);

		cvNamedWindow("cube",CV_WINDOW_AUTOSIZE);
		cvShowImage("cube",resize);
		cvWaitKey(0);
		cvReleaseImage(&resize);
		cvReleaseImage(&displayImg);

		cvDestroyWindow("cube");
		std::cout<<"finished generating cube from sphere..."<<std::endl;
	}
	cvReleaseImage(&displayImg);

	for (int i=0;i<6;++i)
	{
		cvReleaseImage(&outImgVec[i]);
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