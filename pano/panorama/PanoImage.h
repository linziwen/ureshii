#ifndef PANO_PANOIMAGE_INCLUDE_H_
#define PANO_PANOIMAGE_INCLUDE_H_
#pragma once

#ifdef DLL_CLASS 
#else 
#define DLL_CLASS __declspec(dllimport)
#endif 

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
using std::string;

namespace Pano{

class DLL_CLASS  PanoImage
{
public:
	explicit PanoImage(string pathAndName, float radiu);
	~PanoImage(void);

public:
	void hello();

	const string getName() const;
	void setName(const string );

	const string getPath() const;
	void setPath(const string );

	const cv::Mat getImage() const;
	void setImage(const cv::Mat val);

	const float getRadiu() const;
	void setRadiu(const float );

	const cv::Point2f getCenter() const;
	void setCenter(const cv::Point2f val);

private:
	void assignCenterAndRadiu();
	void loadImage();
	void resizeImgToSquare();
	void seperatePathAndName(string pathAndName);

private:
	string imgName;
	string imgPath;
	cv::Mat image;
	cv::Point2f center;
	float radiu;

};

}
#endif 