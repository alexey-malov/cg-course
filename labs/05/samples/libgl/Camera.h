// Camera.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__86340ABE_E112_436C_A17C_64799E1122A3__INCLUDED_)
#define AFX_CAMERA_H__86340ABE_E112_436C_A17C_64799E1122A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "linear_algebra.h"

class CCamera  
{
public:
	void Pitch(double angleInRadians);
	void Yaw(double angleInRadians);
	void Roll(double angleInRadians);
	void Move(double dx, double dy, double dz);
	void LoadToOpenGL()const;
	void LookAt(
		double eyeX, double eyeY, double eyeZ,
		double lookAtX, double lookAtY, double lookAtZ,
		double upX, double upY, double upZ);
	CCamera();
	virtual ~CCamera();
	void Normalize();
private:
	doubleMatrix4	m_transform;
};

#endif // !defined(AFX_CAMERA_H__86340ABE_E112_436C_A17C_64799E1122A3__INCLUDED_)
