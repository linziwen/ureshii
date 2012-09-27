#include "Util.h"

namespace Pano{

Util::Util(void)
{
}


Util::~Util(void)
{
}

double Util::SoluteEquation( double **ppMatrix,int nUpBnd1,int nUpBnd2 )
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


cv::Point3f Util::matXpoint(C44Matrix mat, cv::Point3f point){
	C44Matrix resultMax;
	C44Matrix pointMax;
	pointMax.m_aData[0][0] = point.x;
	pointMax.m_aData[1][0] = point.y;
	pointMax.m_aData[2][0] = point.z;
	pointMax.m_aData[3][0] = 1;
	resultMax = mat*pointMax;
	return cv::Point3f(resultMax.m_aData[0][0], resultMax.m_aData[1][0], resultMax.m_aData[2][0]);

}

}