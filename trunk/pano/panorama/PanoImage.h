#ifndef PANO_PANOIMAGE_INCLUDE_H_
#define PANO_PANOIMAGE_INCLUDE_H_
#pragma once

#ifdef DLL_CLASS 
#else 
#define DLL_CLASS __declspec(dllimport)
#endif 

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
using std::string;

namespace Pano{

class DLL_CLASS  PanoImage
{
public:
	PanoImage(void);
	~PanoImage(void);

public:
	void hello();

private:
	string imgName;
	string imgPath;
	cv::Mat image;
	cv::Point2f center;
	float radiu;

};

}
#endif 