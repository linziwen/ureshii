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
namespace Pano{
C44Matrix::C44Matrix()
{
	//Ԫ��ȫ����ʼ��Ϊ0
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			m_aData[i][j]=0;
}

C44Matrix::~C44Matrix()
{

}

//���������=
void C44Matrix::operator = (const C44Matrix Src)
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			m_aData[i][j]=Src.m_aData[i][j];
}
//���������=
void C44Matrix::operator = (double aData[4][4])
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			m_aData[i][j]=aData[i][j];
}

//���������==
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

//���������+
//����ӷ�
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

//���������-
//�������
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

//���������
//����˷�
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

//���������[]
double* C44Matrix::operator [](int i)
{
	if(i<4 && i>=0)
		return m_aData[i];
	else
	{
		//AfxMessageBox("�������鷶Χ��Error in C44Matrix::operator []");
		//ASSERT(0);
		return NULL;
	}
}

//��������
//����ֵ�������ؾ��������ʽ������ʽΪ0�����Ϊ���췽�󣬲�������
double C44Matrix::Rec()
{
	int n=4, nTmp;                //�м����; 
	double *l=new double[n]; //����
	int *pOrder=new int[n];  //�����¼
	int i,j,k,ik;            //������
	double dDet=1;           //����ʽֵ
	double dMaxNum;          //���������ֵ
	double dTem;             //�м����

	//pOrder��ֵ
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
            //AfxMessageBox("��ԪΪ0��Error in CGraphicItem::SoluteEquation()");
            //ASSERT(0);
			dDet=0;
			return dDet;
		}
        else if(ik!=k)
		{
			//m_aData��ֵ����
            for(j=0;j<n;j++)
			{
                dTem = m_aData[k][j];
                m_aData[k][j]=m_aData[ik][j];
                m_aData[ik][j]=dTem;
            }
			//���������¼
			nTmp=pOrder[k];
			pOrder[k]=pOrder[ik];
			pOrder[ik]=nTmp;
			//����ʽ���
			dDet=-dDet;
        }
		}

		dDet=dDet*m_aData[k][k];
		//��Ϊ���췽��
		if(fabs(dDet)<0.000001)
		{
			//AfxMessageBox("����Ϊ����������ʧ�ܡ�");
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

	//ð����������˳��
	for(i=0;i<n-1;i++)
	{
		for(j=i+1;j<n;j++)
		{
			if(pOrder[i]>pOrder[j])
			{
				//����ԭ
				nTmp=pOrder[i];
				pOrder[i]=pOrder[j];
				pOrder[j]=nTmp;
				//����ݽ���
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


//��λ��
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
			return FALSE;  //ĳ��ȫΪ0��aLen[i]Ϊ0�����������ܵ�λ��
	}
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			m_aData[i][j] = m_aData[i][j]/aLen[i];
	return TRUE;
}

}
