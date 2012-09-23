#include "DllControl.h"
#include "PanoImage.h"
#include "Logger.h"
#include <iostream>

using std::string;
using std::stringstream;
using Pano::Logger;

namespace Pano{
PanoImage::PanoImage(string pathAndName,float radiu)
	:radiu(radiu)
{
	LOG_INFO("begin prepare image: " + pathAndName);
	this->seperatePathAndName(pathAndName);
	this->loadImage();
	this->resizeImgToSquare();
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

void PanoImage::resizeImgToSquare(){
	LOG_INFO("begin to find center and radiu");
	LOG_INFO("convert colored image to gray");
	cv::Mat grayImg(this->getImage().size(),CV_8U);
	cv::Mat binImg(this->getImage().size(),CV_8U);
	cv::cvtColor(this->getImage(),grayImg,CV_RGB2GRAY);
	LOG_INFO("convert gray image to bin image");
	cv::threshold(grayImg, binImg, 100, 255, cv::THRESH_BINARY);
	LOG_INFO("finding the contours");
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point> points;
	cv::findContours(binImg,
		contours, // a vector of contours
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); // all pixels of each contours

	cv::Mat result(binImg.size(),CV_8U);
	//cv::drawContours(result,contours,
	//	-1, // draw all contours
	//	cv::Scalar(0), // in black
	//	2); // with a thickness of 2
	for(int i = 0; i<contours.size(); ++i){
		for(int j = 0; j<contours[i].size(); ++j){
			points.push_back(contours[i][j]);
		}
	}
	LOG_INFO("finding the bounding box");
	cv::Rect bounding = cv::boundingRect(points);
	std::stringstream ss;
	ss << "width = " << bounding.width <<", height = " << bounding.height << std::endl;
	LOG_INFO(ss.str());
	cv::rectangle(result,bounding,cv::Scalar(0),2);

	cv::Mat roi = this->getImage()(bounding);
	int edge = this->getRadiu()*2;
	ss.str("");
	ss << "r=" << this->getRadiu();
	LOG_INFO(ss.str());
	cv::Size destSize(edge,edge);
	cv::Mat resize(destSize, CV_32F);
	cv::resize(roi,resize,destSize);
	this->setImage(resize);
	ss.str("");
	ss << "width = " << resize.cols <<", height = " << resize.rows << std::endl;
	LOG_INFO(ss.str());
	//cv::namedWindow("hello",0);
	//cv::imshow("hello",this->getImage());
	//cv::resizeWindow("hello",1024,768);
	//while(1){
	//	if(cvWaitKey(100) == 27){
	//		break;
	//	}
	//}
}

void PanoImage::assignCenterAndRadiu()
{
	//this->setRadiu(this->getImage().cols);
	stringstream ss;
	ss << "radiu is : " << this->getRadiu();
	LOG_INFO(ss.str());
	ss.str("");
	cv::Point2f center(this->getRadiu(), this->getRadiu());
	ss << "center is: x=" << center.x << "  y=" <<center.y;
	LOG_INFO(ss.str());
	this->setCenter(center);
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

const float PanoImage::getRadiu() const
{
	return this->radiu;
}

void PanoImage::setRadiu( const float r)
{
	this->radiu = r;
}

const cv::Point2f PanoImage::getCenter() const
{
	return this->center;
}

void PanoImage::setCenter( const cv::Point2f val )
{
	this->center = val;
}
}