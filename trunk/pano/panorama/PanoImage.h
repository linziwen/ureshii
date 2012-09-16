#ifndef PANO_PANOIMAGE_INCLUDE_H_
#define PANO_PANOIMAGE_INCLUDE_H_
#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
using std::string;

namespace Pano{

class PanoImage
{
public:
	PanoImage(void);
	~PanoImage(void);

private:
	string imgName;
	string imgPath;
	cv::Mat image;
	cv::Point2f center;
	float radiu;

};

}
#endif 