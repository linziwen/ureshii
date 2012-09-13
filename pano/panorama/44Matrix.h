// 44Matrix.h: interface for the C44Matrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_44MATRIX_H__BC38206E_C3B3_450F_8CEB_97117E4A39D0__INCLUDED_)
#define AFX_44MATRIX_H__BC38206E_C3B3_450F_8CEB_97117E4A39D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>
namespace Pano{
class C44Matrix  
{
public:
	double m_aData[4][4];
public:
	C44Matrix();
	virtual ~C44Matrix();

	void operator = (const C44Matrix Src);
	void operator = (double aData[4][4]);
	BOOL operator == (const C44Matrix Src);
	C44Matrix operator + (const C44Matrix matrix);
	C44Matrix operator - (const C44Matrix matrix);
	C44Matrix operator * (const C44Matrix matrix);
	double* operator [](int i);
	double Rec(); //求逆阵
	BOOL Uni(); //单位化

};
}
#endif // !defined(AFX_44MATRIX_H__BC38206E_C3B3_450F_8CEB_97117E4A39D0__INCLUDED_)
