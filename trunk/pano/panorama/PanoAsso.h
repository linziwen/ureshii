#ifndef PANO_PANOASSO_INCLUDE_H_
#define PANO_PANOASSO_INCLUDE_H_
#pragma once

#ifdef DLL_CLASS 
#else 
#define DLL_CLASS __declspec(dllimport)
#endif 
#define LOG_INFO(str) Pano::Logger::instance().info(str, __LINE__, __FILE__)
#define LOG_ERROR(str) Pano::Logger::instance().error(str, __LINE__, __FILE__)

#include <string>
#include <iostream>
#include "44Matrix.h"
#include "Logger.h"
#include "Util.h"
#include "panorama.h"


using Pano::C44Matrix;
using Pano::Logger;
namespace Pano{
	class PanoImage;
}

namespace Pano{
class DLL_CLASS PanoAsso
{
public:
	PanoAsso(void);
	~PanoAsso(void);

public:
	void setImg1Id(PanoImage *);
	PanoImage * getImg1Id() const;

	void setImg2Id(PanoImage *);
	PanoImage * getImg2Id() const;

	void addPatchPoint(const TPatchPntInfo point);

	void calMatA();

	const cv::Point3f getSpherePos() const;
	void setSpherePos(const cv::Point3f val);

	const C44Matrix getMatA() const;

	const double getR0() const;
	void setR0(const double r0) ;
private:
	vector<TPatchPntInfo> patchPnts;
	PanoImage *img1Id;
	PanoImage *img2Id;
	C44Matrix matA;
	cv::Point3f spherePos;
	double r0;

};

}
#endif
