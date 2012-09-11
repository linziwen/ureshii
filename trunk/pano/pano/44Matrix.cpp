// 44Matrix.cpp: implementation of the C44Matrix class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "mfcvega.h"
#include "44Matrix.h"
#include "math.h"
//#include <Windows.h>

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C44Matrix::C44Matrix()
{
	//元素全部初始化为0
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			m_aData[i][j]=0;
}

C44Matrix::~C44Matrix()
{

}

//重载运算符=
void C44Matrix::operator = (const C44Matrix Src)
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			m_aData[i][j]=Src.m_aData[i][j];
}
//重载运算符=
void C44Matrix::operator = (double aData[4][4])
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			m_aData[i][j]=aData[i][j];
}

//重载运算符==
BOOL C44Matrix::operator == (const C44Matrix Src)
{
	BOOL bEqual=TRUE;
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(m_aData[i][j]!=Src.m_aData[i][j])
			{
				bEqual=FALSE;
				break;
			}
		}
	}

	return bEqual;
}

//重载运算符+
//矩阵加法
C44Matrix C44Matrix::operator + (const C44Matrix matrix)
{
	int i,j;
	C44Matrix mtxTem;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			mtxTem.m_aData[i][j]=m_aData[i][j]
								 +matrix.m_aData[i][j];
	return mtxTem;
}

//重载运算符-
//矩阵减法
C44Matrix C44Matrix::operator - (const C44Matrix matrix)
{
	int i,j;
	C44Matrix mtxTem;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			mtxTem.m_aData[i][j]=m_aData[i][j]
								 -matrix.m_aData[i][j];
	return mtxTem;
}

//重载运算符
//矩阵乘法
C44Matrix C44Matrix::operator * (const C44Matrix matrix)
{
	int i,j,k;
	C44Matrix mtxTem;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			for(k=0;k<4;k++)
				mtxTem.m_aData[i][j] += m_aData[i][k]
						*matrix.m_aData[k][j];
	return mtxTem;
}

//重载运算符[]
double* C44Matrix::operator [](int i)
{
	if(i<4 && i>=0)
		return m_aData[i];
	else
	{
		//AfxMessageBox("超出数组范围。Error in C44Matrix::operator []");
		//ASSERT(0);
		return NULL;
	}
}

//矩阵求逆
//返回值——返回矩阵的行列式，行列式为0则矩阵为奇异方阵，不可求逆
double C44Matrix::Rec()
{
	int n=4, nTmp;                //中间变量; 
	double *l=new double[n]; //乘数
	int *pOrder=new int[n];  //次序记录
	int i,j,k,ik;            //计数器
	double dDet=1;           //行列式值
	double dMaxNum;          //列中最大绝对值
	double dTem;             //中间变量

	//pOrder赋值
	for(k=0;k<n;k++)
		pOrder[k]=k;

	for(k=0;k<n;k++)
	{
		if(k!=n-1)
		{
        dMaxNum = fabs(m_aData[k][k]);
        ik = k;
        for(i=k+1;i<n;i++)
		{
            if(fabs(m_aData[i][k]) > dMaxNum)
			{
                dMaxNum = fabs(m_aData[i][k]);
                ik = i;
			}
        }
        
        if(dMaxNum < 0.0000000001)
		{
            //AfxMessageBox("主元为0，Error in CGraphicItem::SoluteEquation()");
            //ASSERT(0);
			dDet=0;
			return dDet;
		}
        else if(ik!=k)
		{
			//m_aData数值交换
            for(j=0;j<n;j++)
			{
                dTem = m_aData[k][j];
                m_aData[k][j]=m_aData[ik][j];
                m_aData[ik][j]=dTem;
            }
			//交换次序记录
			nTmp=pOrder[k];
			pOrder[k]=pOrder[ik];
			pOrder[ik]=nTmp;
			//行列式变号
			dDet=-dDet;
        }
		}

		dDet=dDet*m_aData[k][k];
		//若为奇异方阵
		if(fabs(dDet)<0.000001)
		{
			//AfxMessageBox("方阵为奇异阵，求逆失败。");
//			ASSERT(0);
			return 0;
		}

		for(i=0;i<n;i++)
		{
			if(i!=k)
			{
				l[i]=m_aData[i][k]/m_aData[k][k];
				for(j=0;j<n;j++)
				{
					if(j!=k)
						m_aData[i][j]=m_aData[i][j]-
									  l[i]*m_aData[k][j];
					else
						m_aData[i][j]=-l[i];
				}
			}
			else
			{
				l[i]=1/m_aData[k][k];
			}
		}
		for(j=0;j<n;j++)
		{
			if(j!=k)
				m_aData[k][j]=m_aData[k][j]*l[k];
			else
				m_aData[k][j]=l[k];
		}
	}

	//冒泡排序整理顺序
	for(i=0;i<n-1;i++)
	{
		for(j=i+1;j<n;j++)
		{
			if(pOrder[i]>pOrder[j])
			{
				//次序复原
				nTmp=pOrder[i];
				pOrder[i]=pOrder[j];
				pOrder[j]=nTmp;
				//列数据交换
				for(k=0;k<n;k++)
				{
					dTem=m_aData[k][i];
					m_aData[k][i]=m_aData[k][j];
					m_aData[k][j]=dTem;
				}
			}
		}
	}


	delete [] l;
	delete [] pOrder;
/*
#ifdef _DEBUG
	TRACE("****The conversion of the Matrix****\n");
	CString szNum;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			szNum.Format("%6.2f,",m_aData[i][j]);
			TRACE(szNum);
		}
		TRACE("\n");
	}
	TRACE("***********END***********\n");
#endif //_DEBUG
*/

	return dDet;
}


//单位化
BOOL C44Matrix::Uni() 
{
	double aLen[4];
	int i, j;
	for(i=0;i<4;i++)
		aLen[i] = 0;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			aLen[i] += m_aData[i][j]*m_aData[i][j];
	for(i=0;i<4;i++)
	{
		aLen[i] = sqrt(aLen[i]);
		if(fabs(aLen[i])<0.00000001)
			return FALSE;  //某行全为0，aLen[i]为0不能做除数，不能单位化
	}
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			m_aData[i][j] = m_aData[i][j]/aLen[i];
	return TRUE;
}