// Camera.cpp: implementation of the CCamera class.
//
//////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "stdafx.h"
#include "Camera.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
	m_transform.loadIdentity();
}

CCamera::~CCamera()
{

}

void CCamera::LookAt(
			double eyeX, double eyeY, double eyeZ,
			double lookAtX, double lookAtY, double lookAtZ,
			double upX, double upY, double upZ)
{
	m_transform.loadLookAtRH(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
}

void CCamera::LoadToOpenGL() const
{
	glLoadMatrixd(&m_transform[0]);
}

void CCamera::Normalize()
{
	doubleVector3 xAxis;
	doubleVector3 yAxis;
	doubleVector3 zAxis;
	
	m_transform.getRow(0, xAxis);
	m_transform.getRow(1, yAxis);

	xAxis.normalize();
	yAxis.normalize();
	
	cross(zAxis, xAxis, yAxis).normalize();
	cross(yAxis, zAxis, xAxis).normalize();

	m_transform.setRow(0, xAxis);
	m_transform.setRow(1, yAxis);
	m_transform.setRow(2, zAxis);
}

void CCamera::Move(double dx, double dy, double dz)
{
	doubleMatrix4 mat;
	doubleMatrix4 rv;
	mat.loadTranslation(dx, dy, dz);
	mult(rv, mat, m_transform);
	m_transform = rv;
}


void CCamera::Roll(double angleInRadians)
{
	doubleMatrix4 mat;
	doubleMatrix4 rv;
	mat.loadRotation(angleInRadians, 0, 0, 1);
	mult(rv, mat, m_transform);
	m_transform = rv;
}

void CCamera::Yaw(double angleInRadians)
{
	doubleMatrix4 mat;
	doubleMatrix4 rv;
	mat.loadRotation(angleInRadians, 0, 1, 0);
	mult(rv, mat, m_transform);
	m_transform = rv;
}

void CCamera::Pitch(double angleInRadians)
{
	doubleMatrix4 mat;
	doubleMatrix4 rv;
	mat.loadRotation(angleInRadians, 1, 0, 0);
	mult(rv, mat, m_transform);
	m_transform = rv;
}
