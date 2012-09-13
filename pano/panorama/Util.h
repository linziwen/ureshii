#ifndef PANO_UTIL_INCLUDE_H
#define PANO_UTIL_INCLUDE_H
#pragma once

#define PI 3.14159265
#define ERR         1.4568159901474E+144

#include <stdlib.h>
#include "44Matrix.h"
#include <stdio.h>
#include <math.h>
#include <cv.h>

using Pano::C44Matrix;

namespace Pano{
	class Util
	{
		public:
			Util(void);
			~Util(void);

		public:
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
			static double SoluteEquation(double **ppMatrix,int nUpBnd1,int nUpBnd2);

			static void findImageRadiusAndCenter( const IplImage *img ,int &radius,CvPoint &center);
		public:

	};
}



#endif