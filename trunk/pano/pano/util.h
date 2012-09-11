// EquationExample.cpp : Defines the entry point for the console application.
//


//reference from sifthelper
#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"


#include <cv.h>
#include <cxmisc.h>
#include <cxcore.h>
#include <cvaux.h>
//#include <cxeigen.h>
//#include <ml.h>

#include <highgui.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "44Matrix.h"
#include "stdio.h"
#include "math.h"
#include "panorama.h"



#define PI 3.14159265
#define ERR         1.4568159901474E+144

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 500

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.4

extern "C"{
	extern struct kd_node* kdtree_build( struct feature* features, int n );
	extern  int kdtree_bbf_knn( struct kd_node* kd_root, struct feature* feat,
		int k, struct feature*** nbrs, int max_nn_chks );
	extern void kdtree_release( struct kd_node* kd_root );
	extern double descr_dist_sq( struct feature* f1, struct feature* f2 );
	extern int _sift_features( IplImage* img, struct feature** feat, int intvls,
		double sigma, double contr_thr, int curv_thr,
		int img_dbl, int descr_width, int descr_hist_bins );
	extern IplImage* stack_imgs( IplImage* img1, IplImage* img2 );
	extern void fatal_error( char* format, ... );
	extern CvMat* ransac_xform( struct feature* features, int n, int mtype,
		ransac_xform_fn xform_fn, int m,
		double p_badxform, ransac_err_fn err_fn,
		double err_tol, struct feature*** inliers,
		int* n_in );
	extern CvMat* lsq_homog( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n );
	extern double homog_xfer_err( CvPoint2D64f pt, CvPoint2D64f mpt, CvMat* H );

};


//解线性方程组
//参数：ppMatrix——方程组系数增广矩阵，为 n*(n+1) 矩阵
//		nUpBnd1——矩阵行数上限，基于0
//		nUpBnd2——矩阵列数上限，基于0
//结果：解最后保存在ppMatrix[i][n]  (i=0,1…n-1).
//      即每行的最后的元素那里
//返回值：返回方程系数矩阵的行列式值，
//		  若此值为0，则方程不可解（无解或无穷解）
//		  若此值为-1.45e144，则为程序出错，一般为调用不当
//算法：列主元高斯消去法
double SoluteEquation(double **ppMatrix,int nUpBnd1,int nUpBnd2)
{
    int i,j,k,ik;
    double dTem,dMaxNum;
	double dDet;  //行列式值
    
    //容错
	////矩阵不为方阵则退出，返回ERR
    if(nUpBnd1!=nUpBnd2-1)
	{
		//AfxMessageBox("所解方程矩阵不为方阵。Error in CGraphicItem::SoluteEquation()");
		//ASSERT(0);
		dDet=ERR;
		return dDet;
	}
	//检查指针数组ppMatrix中元素是否有效
	for(k=0;k<=nUpBnd1;k++)
	{
		if(ppMatrix[k]==(double*)NULL || 
		   ppMatrix[k]==(double*)0xcccccccc ||
		   ppMatrix[k]==(double*)0xcdcdcdcd || 
		   ppMatrix[k]==(double*)0xdddddddd)
		{
			//ASSERT(0);
			dDet=ERR;
			return dDet;
		}
	}

	dDet=1;//初始化为1
    
    for(k=0;k<=nUpBnd1-1;k++)
	{
        //按列选主元
        dMaxNum = fabs(ppMatrix[k][k]);
        ik = k;
        for(i=k+1;i<=nUpBnd1;i++)
		{
            if(fabs(ppMatrix[i][k]) > dMaxNum)
			{
                dMaxNum = fabs(ppMatrix[i][k]);
                ik = i;
			}
        }
        
        //主元为0则退出,主元为ppMatrix[k][k]则跳过if,否则换行
        if(dMaxNum < 0.0000000001)
		{
            //AfxMessageBox("主元为0，Error in CGraphicItem::SoluteEquation()");
            //ASSERT(0);
			dDet=0;
			return dDet;
		}
        else if(ik!=k)
		{
            for(j=k;j<=nUpBnd2;j++)
			{
                dTem = ppMatrix[k][j];
                ppMatrix[k][j]=ppMatrix[ik][j];
                ppMatrix[ik][j]=dTem;
            }
			dDet=-dDet;
        }
        
        //计算乘数l[i][k],存入ppMatrix[i][k]单元。
        for(i=k+1;i<=nUpBnd1;i++)
		{
            ppMatrix[i][k] = ppMatrix[i][k] / ppMatrix[k][k];
        }
        
        //消元
        for(i=k+1;i<=nUpBnd1;i++)
		{
            for(j=k+1 ;j<=nUpBnd2;j++)
			{
                ppMatrix[i][j] = ppMatrix[i][j]-ppMatrix[i][k] * ppMatrix[k][j];
            }
        }
        dDet = ppMatrix[k][k] * dDet;
    }
    
    
    
    if(ppMatrix[nUpBnd1][nUpBnd1]==0)//
	{
        dDet = 0;
        return dDet;
	}
    else
	{
        dDet = ppMatrix[nUpBnd1][nUpBnd1] * dDet;//
	}
    
    
    //求解，解存入ppMatrix[k][n+1]中
    for(k=nUpBnd1;k>=0;k--)
	{
        dTem = 0;
        for(j=k+1;j<=nUpBnd1;j++)
		{
            dTem = dTem + ppMatrix[k][j] * ppMatrix[j][nUpBnd2];
        }
		ppMatrix[k][nUpBnd2] = (ppMatrix[k][nUpBnd2] - 
								dTem) / ppMatrix[k][k];
    }
    
	return dDet;
}



void findImageRadiusAndCenter( const IplImage *img ,int &radius,CvPoint &center)
{
	IplImage *gray=cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);

	CvRect outline;
	CvMemStorage *storage=NULL;
	CvSeq *contours=0;

	//find the contours
	storage=cvCreateMemStorage(0);
	cvCvtColor(img,gray,CV_BGR2GRAY);
	cvThreshold(gray,gray,100,255,CV_THRESH_BINARY);
	cvFindContours(gray,storage,&contours);
	cvZero(gray);
	cvDrawContours(gray,contours,cvScalarAll(255),cvScalarAll(255),100);

	//find the bounding rectangle
	outline=cvBoundingRect(gray);

	//is it a square? if not may be have another thing to do.here just omit the complecated.
	if (outline.width==outline.height)
	{
		radius=outline.width/2;
	}else{
		radius = (outline.width/2 + outline.height/2)/2;
	}
	center.x = outline.x + outline.width/2;
	center.y = outline.y + outline.height/2;
}


IplImage* genOnePlane( IplImage *img, int l, int planeIndex )
{
	IplImage *outimg = cvCreateImage(cvSize(l,l),IPL_DEPTH_8U,3);
	for (int y = 0; y<outimg->height; ++y)
	{
		uchar *ptr=(uchar *)(outimg->imageData + y * outimg->widthStep);
		for (int x = 0; x<outimg->width; ++x)
		{
			double xc, yc;
			xc = x - l/2;
			yc = (-y) + l/2;

			double theta, phy;
			if (planeIndex == 4)
			{
				theta = atan2(yc,xc);
				phy = atan2(double(l),2 * sqrt(xc*xc+yc*yc));
			}else if(planeIndex == 5){
				theta = atan2(yc,xc);
				phy = atan2(double(-l),2 * sqrt(xc*xc+yc*yc));
			}else{
				theta = planeIndex*PI/2 - atan2(xc,l/2);
				phy = atan2(yc,sqrt(xc*xc+(l/2)*(l/2)));
			}


			if (theta<0)
			{
				theta += 2*PI;
			}

			int xp, yp;
			xp = int(theta * img->width/(2*PI));
			yp = int(img->height/2 - phy * img->height/PI);

			uchar* pixelColor = &((uchar*)(img->imageData + img->widthStep*yp))[xp*3];
			ptr[x*3] = pixelColor[0];
			ptr[x*3+1] = pixelColor[1];
			ptr[x*3+2] = pixelColor[2];
		}
	}

	return outimg;
}


void findCounterpartPt( const CvPoint2D32f p2e,C44Matrix mat,double rMax,double r,CvPoint2D32f &p2e1 )
{
	double theta,phy,x,y,z,theta2,phy2,x2,y2,z2,r2;
	C44Matrix mat2e,mat2e1;
	theta = atan2(p2e.y,p2e.x);
	phy = PI/2*(1-rMax/r);
	x = cos(theta)*cos(phy);
	y = sin(theta)*cos(phy);
	z = sin(phy);

	mat2e.m_aData[0][0] = x;
	mat2e.m_aData[1][0] = y;
	mat2e.m_aData[2][0] = z;
	mat2e.m_aData[3][0] = 1;

	mat2e1 = mat*mat2e;
	x2 = mat2e1.m_aData[0][0];
	y2 = mat2e1.m_aData[1][0];
	z2 = mat2e1.m_aData[2][0];

	theta2 = atan2(y2,x2);
	phy2 = atan2(z2,sqrt(x2*x2+y2*y2));
	r2 = r*(PI/2-phy2)/(PI/2);

	p2e1.x = float(r2*cos(theta2));
	p2e1.y = float(r2*sin(theta2));
}

void findPatchPoint(char * aSrcPics[],int nPicNum,struct TPatchPntInfo * pPatPtInfo,int nPatchPntsNum,int rmax,int r180,int rmin,CvPoint center){
	//const int MAX_CONNERS = 500;
	std::vector<IplImage *> images;
	int ptnIndex = 0;


	for (int i = 0; i<nPicNum; ++i)
	{
		IplImage * img = cvLoadImage(aSrcPics[i],1);
		cvCircle(img,center,rmin,CV_RGB(0,0,0),CV_FILLED);
		images.push_back(img);
	}

	for (int i=0; i<nPicNum; ++i)
	{
		//i=1;
		int pFirst = i+1;
		int pNext = pFirst+1;
		if (pNext>nPicNum)
		{
			pNext=1;
		}
		IplImage* img1, * img2, * stacked;
		struct feature* feat1, * feat2, * feat;
		struct feature** nbrs;
		struct kd_node* kd_root;
		CvPoint pt1, pt2;
		double d0, d1;
		int n1, n2, k, j, m = 0;

		img1 =cvCloneImage( images[pFirst-1]);
		if( ! img1 )
			fatal_error( "unable to load image from %s", aSrcPics[pFirst-1] );
		img2 = cvCloneImage(images[pNext-1]);
		if( ! img2 )
			fatal_error( "unable to load image from %s", aSrcPics[pNext-1] );
		stacked = stack_imgs( img1, img2 );

		fprintf( stderr, "Finding features in %s...\n", aSrcPics[pFirst-1] );
		n1 = sift_features( img1, &feat1 );
		//struct feature **ppf = &feat1;
		//ransac_xform(feat1,n1,FEATURE_BCK_MATCH,lsq_homog, 4, 0.01,
		//	homog_xfer_err, 3.0, &ppf, &n1 );

		fprintf( stderr, "Finding features in %s...\n",aSrcPics[pNext-1] );
		n2 = sift_features( img2, &feat2 );
		//ppf = &feat2;
		//ransac_xform(feat2,n2,FEATURE_FWD_MATCH,lsq_homog, 4, 0.01,
		//	homog_xfer_err, 3.0, &ppf, &n2 );

		kd_root = kdtree_build( feat2, n2 );
		fprintf( stderr, "drawing line...\n",aSrcPics[pNext-1] );
		for( j = 0; j < n1; j++ )
		{
			feat = feat1 + j;
			k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
			if( k == 2 )
			{
				d0 = descr_dist_sq( feat, nbrs[0] );
				d1 = descr_dist_sq( feat, nbrs[1] );
				if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
				{

					pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
					pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );

					//if (pt1.x == pPatPtInfo[ptnIndex-1].x1 || pt1.y == pPatPtInfo[ptnIndex-1].y1)
					//{
					//	free( nbrs );
					//	continue;
					//}
					//if ((pt1.x-center.x)*(pt2.x-center.x)>0 || (pt1.y-center.y)*(pt2.y-center.y)<0 
					//	|| sqrt(float((pt1.x-center.x)*(pt1.x-center.x))+float((pt1.y-center.y)*(pt1.y-center.y)))<rmin)
					//{
					//	free( nbrs );
					//	continue;
					//}
					pt2.x += img1->width;
					cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
					//if (ptnIndex<pFirst*3)
					//{
					//	pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
					//	pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );

					//	if (pt1.x == pPatPtInfo[ptnIndex-1].x1 || pt1.y == pPatPtInfo[ptnIndex-1].y1)
					//	{
					//		free( nbrs );
					//		continue;
					//	}
					//	if ((pt1.x-center.x)*(pt2.x-center.x)>0 || (pt1.y-center.y)*(pt2.y-center.y)<0 
					//		|| sqrt(float((pt1.x-center.x)*(pt1.x-center.x))+float((pt1.y-center.y)*(pt1.y-center.y)))<rmin)
					//	{
					//		free( nbrs );
					//		continue;
					//	}
					//	
					//	
					//	pPatPtInfo[ptnIndex].nPic1 = pFirst;
					//	pPatPtInfo[ptnIndex].nPic2 = pNext;
					//	pPatPtInfo[ptnIndex].x1 = pt1.x;
					//	pPatPtInfo[ptnIndex].y1 = pt1.y;
					//	pPatPtInfo[ptnIndex].x2 = pt2.x;
					//	pPatPtInfo[ptnIndex].y2 = pt2.y;

					//	pt2.x += img1->width;
					//	cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );

					//	ptnIndex++;
					//}else{
					//	free( nbrs );
					//	break;
					//}
					

					m++;
					//if (m>100)
					//{
					//	m=0;
					//	cvSaveImage("match.jpg",stacked);
					//	break;
					//}
					
					feat1[j].fwd_match = nbrs[0];
				}
			}
			free( nbrs );
		}
		//////use ransac to accurate the feature point
		//fprintf( stderr, "finish draw lines \n",aSrcPics[pNext-1] );
		//struct feature **ppf = &feat1;
		//ransac_xform(feat1,n1,FEATURE_FWD_MATCH,lsq_homog, 4, 0.01,
		//	homog_xfer_err, 3.0, &ppf, &n1 );
		//for (int k = 0;k<n1; ++k)
		//{
		//	CvPoint p1 = cvPoint(cvRound(ppf[k]->x),cvRound(ppf[k]->y));
		//	CvPoint p2 = cvPoint(cvRound(ppf[k]->fwd_match->x),cvRound(ppf[k]->fwd_match->y));
		//	cvLine( stacked, pt1, pt2, CV_RGB(255,255,0), 5, 8, 0 );
		//	if (pPatPtInfo[ptnIndex-1].x1==p1.x || p1.y == pPatPtInfo[ptnIndex-1].y1)
		//	{
		//		continue;
		//	}
		//	pPatPtInfo[ptnIndex].x1 = p1.x;
		//	pPatPtInfo[ptnIndex].y1 = p1.y;
		//	pPatPtInfo[ptnIndex].x2 = p2.x;
		//	pPatPtInfo[ptnIndex].y2 = p2.y;
		//	ptnIndex++;
		//}

		cvSaveImage("match.jpg",stacked);
		fprintf( stderr, "Found %d total matches\n", m );
		cvNamedWindow( "Matches", 1 );
		cvShowImage( "Matches", stacked );
		cvWaitKey( 0 );

		cvReleaseImage( &stacked );
		cvReleaseImage( &img1 );
		cvReleaseImage( &img2 );
		kdtree_release( kd_root );
		free( feat1 );
		free( feat2 );
		
	}
	//CvSize img_sz = cvGetSize(images[0]);
	//int win_size = 10;

	//IplImage *eig_image = cvCreateImage(img_sz,IPL_DEPTH_32F,1);
	//IplImage *tmp_image = cvCreateImage(img_sz,IPL_DEPTH_32F,1);
	//int conner_count = MAX_CONNERS;
	//CvPoint2D32f *connersA = new CvPoint2D32f[MAX_CONNERS];

	//for (int i = 0; i<nPicNum; ++i)
	//{
	//	int pNext = i+1;
	//	if (pNext==nPicNum)
	//	{
	//		pNext=0;
	//	}
	//	cvGoodFeaturesToTrack(images[i],eig_image,tmp_image,connersA,&conner_count,0.01,15.0,0,3,0,0.04);
	//	cvFindCornerSubPix(images[i],connersA,conner_count,cvSize(win_size,win_size),cvSize(-1,-1),
	//		cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
	//	char feature_found[MAX_CONNERS];
	//	float ferture_error[MAX_CONNERS];
	//	CvSize pry_sz = cvSize(images[i]->width+8,images[pNext]->height/3);

	//	IplImage *pryA = cvCreateImage(pry_sz,IPL_DEPTH_32F,1);
	//	IplImage *pryB = cvCreateImage(pry_sz,IPL_DEPTH_32F,1);

	//	CvPoint2D32f *connersB = new CvPoint2D32f[MAX_CONNERS];

	//	//CalcOpticalFlowPyrLK();
	//	
	//	cvCalcOpticalFlowPyrLK(images[i],images[pNext],pryA,pryB,connersA,connersB,conner_count,
	//		cvSize(win_size,win_size),5,feature_found,ferture_error,cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.3),0);

	//	for (int j=0; j<conner_count; ++j)
	//	{
	//		if (feature_found[j]==0)
	//		{
	//			continue;
	//		}
	//		CvPoint p0 = cvPoint(cvRound(connersA[j].x),cvRound(connersA[j].y));
	//		CvPoint p1 = cvPoint(cvRound(connersB[j].x),cvRound(connersB[j].y));
	//		cout<<j<<"   p0x="<<p0.x<<" p0y="<<p0.y<<" p1x="<<p1.x<<" p1y="<<p1.y<<endl;
	//	}
	//	
	//}

	for (int i = 0; i<nPicNum; ++i)
	{
		cvReleaseImage(&images[i]);
	}
}