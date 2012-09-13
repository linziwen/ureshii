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
			//�����Է�����
			//������ppMatrix����������ϵ���������Ϊ n*(n+1) ����
			//		nUpBnd1���������������ޣ�����0
			//		nUpBnd2���������������ޣ�����0
			//���������󱣴���ppMatrix[i][n]  (i=0,1��n-1).
			//      ��ÿ�е�����Ԫ������
			//����ֵ�����ط���ϵ�����������ʽֵ��
			//		  ����ֵΪ0���򷽳̲��ɽ⣨�޽������⣩
			//		  ����ֵΪ-1.45e144����Ϊ�������һ��Ϊ���ò���
			//�㷨������Ԫ��˹��ȥ��
			static double SoluteEquation(double **ppMatrix,int nUpBnd1,int nUpBnd2);

			static void findImageRadiusAndCenter( const IplImage *img ,int &radius,CvPoint &center);
		public:

	};
}



#endif