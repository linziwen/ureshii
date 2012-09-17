#include "DllControl.h"
#include "PanoImage.h"
#include "Logger.h"
#include <iostream>
using std::string;
using Pano::Logger;

namespace Pano{
PanoImage::PanoImage(string pathAndName)
{
	LOG_INFO("begin prepare image: " + pathAndName);
	this->seperatePathAndName(pathAndName);
	this->loadImage();
	this->assignCenterAndRadiu();
}


PanoImage::~PanoImage(void)
{
}

void PanoImage::hello()
{
	LOG_INFO("hello from panoimage");
	cv::namedWindow("hello",0);
	cv::imshow("hello",this->getImage());
	cv::resizeWindow("hello",1024,768);
	while(1){
		if(cvWaitKey(100) == 27){
			break;
		}
	}
}

void PanoImage::assignCenterAndRadiu()
{
	LOG_INFO("begin to find center and radiu");
	cv::Mat image = this->getImage().channels()[0];
	cv::Mat binImg;
	cv::threshold(image, binImg, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
	cv::namedWindow("hello",0);
	cv::imshow("hello",binImg);
	cv::resizeWindow("hello",1024,768);
	while(1){
		if(cvWaitKey(100) == 27){
			break;
		}
	}


	//std::vector<std::vector<cv::Point> > contours;
	//cv::findContours(binImg,
	//	contours, // a vector of contours
	//	CV_RETR_EXTERNAL, // retrieve the external contours
	//	CV_CHAIN_APPROX_NONE); // all pixels of each contours
	// Draw black contours on a white image
	//cv::Mat result(this->getImage().size(),CV_8U,cv::Scalar(255));
	//cv::drawContours(result,contours,
	//	-1, // draw all contours
	//	cv::Scalar(0), // in black
	//	2); // with a thickness of 2
	//while(1){
	//	if(cvWaitKey(100) == 27){
	//		break;
	//	}
	//}
}

void PanoImage::loadImage()
{
	LOG_INFO("loading img");
	this->setImage(cv::imread(this->getName()));
	LOG_INFO("finish load img");
}

void PanoImage::seperatePathAndName( string pathAndName )
{
	std::string::size_type pos = pathAndName.find_last_of("\\");
	string path = pathAndName.substr(0,pos);
	LOG_INFO("image path is:"+path);
	this->setPath(path);
	string name = pathAndName.substr(pos + 1);
	LOG_INFO("image name is:" + name);
	this->setName(name);
}

const string PanoImage::getName() const
{
	return this->imgName;
}

void PanoImage::setName( const string name)
{
	this->imgName = name;
}

const string PanoImage::getPath() const
{
	return this->imgPath;
}

void PanoImage::setPath( const string path)
{
	this->imgPath = path;
}

const cv::Mat PanoImage::getImage() const
{
	return this->image;
}

void PanoImage::setImage( const cv::Mat val )
{
	this->image = val;
}
}