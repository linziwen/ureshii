
#include "DllControl.h"

#include "PanoAsso.h"
#include "PanoImage.h"
#include "Logger.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>

using std::string;
using std::cout;
using std::endl;
using Pano::PanoImage;
using Pano::Logger;

namespace Pano{
PanoAsso::PanoAsso(void)
{
}


PanoAsso::~PanoAsso(void)
{
	this->img1Id = NULL;
	this->img2Id = NULL;
}

void PanoAsso::setImg1Id(PanoImage * src)
{
	this->img1Id = src;
}

PanoImage * PanoAsso::getImg1Id() const
{
	return this->img1Id;
}

void PanoAsso::setImg2Id(PanoImage *src)
{
	this->img2Id = src;
}

PanoImage * PanoAsso::getImg2Id() const
{
	return this->img2Id;
}

void PanoAsso::addPatchPoint(const TPatchPntInfo point )
{
	patchPnts.push_back(point);
}

void PanoAsso::calMatA()
{
	LOG_INFO("change the ori point from lefttop to center");
	float radiu = this->img1Id->getRadiu();
	vector<TPatchPntInfo>::iterator it = patchPnts.begin();
	for (; it != patchPnts.end(); ++it)
	{
		it->x1 = it->x1 -radiu;
		it->x2 = it->x2 - radiu;
		it->y1 = -it->y1 + radiu;
		it->y2 = -it->y2 + radiu;
	}

	LOG_INFO("change 2d point to 3d point");
	vector<cv::Point3f> point3ds1;
	vector<cv::Point3f> point3ds2;
	it = patchPnts.begin();
	for (; it != patchPnts.end(); ++it)
	{
		double theta1 = atan2(double(it->y1),double(it->x1));
		double theta2 = atan2(double(it->y2),double(it->x2));

		double r1 = sqrt(pow(double(it->y1),2)+pow(double(it->x1),2));
		double r2 = sqrt(pow(double(it->y2),2)+pow(double(it->x2),2));

		double r0 = (r1+r2)/2;
		double phi1 = PI/2*(1-r1/r0);
		double phi2 = PI/2*(1-r2/r0);

		float x1,y1,z1,x2,y2,z2;
		x1 = float(cos(theta1)*cos(phi1));
		y1 = float(sin(theta1)*cos(phi1));
		z1 = float(sin(phi1));
		x2 = float(cos(theta2)*cos(phi2));
		y2 = float(sin(theta2)*cos(phi2));
		z2 = float(sin(phi2));

		point3ds1.push_back(cv::Point3f(x1, y1, z1));
		point3ds2.push_back(cv::Point3f(x2, y2, z2));
	}

	LOG_INFO("cal the matrixA");
	double **ppA,*pA[9],dDet;
	double A[9][10]={
		point3ds1[0].x,point3ds1[0].y,point3ds1[0].z,0,0,0,0,0,0,point3ds2[0].x,
		0,0,0,point3ds1[0].x,point3ds1[0].y,point3ds1[0].z,0,0,0,point3ds2[0].y,
		0,0,0,0,0,0,point3ds1[0].x,point3ds1[0].y,point3ds1[0].z,point3ds2[0].z,
		point3ds1[1].x,point3ds1[1].y,point3ds1[1].z,0,0,0,0,0,0,point3ds2[1].x,
		0,0,0,point3ds1[1].x,point3ds1[1].y,point3ds1[1].z,0,0,0,point3ds2[1].y,
		0,0,0,0,0,0,point3ds1[1].x,point3ds1[1].y,point3ds1[1].z,point3ds2[1].z,
		point3ds1[2].x,point3ds1[2].y,point3ds1[2].z,0,0,0,0,0,0,point3ds2[2].x,
		0,0,0,point3ds1[2].x,point3ds1[2].y,point3ds1[2].z,0,0,0,point3ds2[2].y,
		0,0,0,0,0,0,point3ds1[2].x,point3ds1[2].y,point3ds1[2].z,point3ds2[2].z,
	};
	for (int k=0;k<9;++k)
	{
		pA[k] = A[k];
	}
	ppA = pA;
	dDet = Util::SoluteEquation(ppA,8,9);
	if (ERR == dDet)
	{
		LOG_ERROR("devoke error");
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
	this->matA = mat;
}
}