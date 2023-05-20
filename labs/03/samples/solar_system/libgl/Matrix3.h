#pragma once

#include "Vector3.h"

/************************************************************************/
/* ��������� ����� ������ �������� 3x3                                  */
/************************************************************************/
template <class T>
class CMatrix3
{
public:
	// ����������� �� ��������� (�������� ��������� �������)
	CMatrix3()
	{
		LoadIdentity();
	}

	// ������������� ������� ���������� �� ������� ��������
	explicit CMatrix3(T const* src)
	{
		memcpy(&mat[0][0], src, sizeof(mat));
	}

	// ���������������� ������������� �������� �������
	CMatrix3(
		T const& v00, T const& v10, T const& v20, // �������� 0 �������
		T const& v01, T const& v11, T const& v21, // �������� 1 �������
		T const& v02, T const& v12, T const& v22 // �������� 2 �������
	)
	{
		mat[0][0] = v00;
		mat[0][1] = v10;
		mat[0][2] = v20;
		mat[1][0] = v01;
		mat[1][1] = v11;
		mat[1][2] = v21;
		mat[2][0] = v02;
		mat[2][1] = v12;
		mat[2][2] = v22;
	}

	/*
	���������� ������������ �������
	*/
	T GetDeterminant() const
	{
		return mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) - mat[1][0] * (mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2]) + mat[2][0] * (mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2]);
	}

	/*
	�������� ��������� �������
	*/
	void LoadIdentity()
	{
		mat[0][1] = mat[0][2] = mat[1][0] = mat[1][2] = mat[2][0] = mat[2][1] = 0;
		mat[0][0] = mat[1][1] = mat[2][2] = 1;
	}

	/*
	���������� ������-�������
	*/
	CVector3<T> GetRow(unsigned i) const
	{
		assert(i < 3);
		return CVector3<T>(
			mat[0][i],
			mat[1][i],
			mat[2][i]);
	}

	/*
	���������� ������-������
	*/
	CVector3<T> GetColumn(unsigned i) const
	{
		assert(i < 3);
		return CVector3<T>(mat[i]);
	}

	// ��������� �������� ������ �������
	void SetRow(unsigned i, const CVector3<T>& v)
	{
		assert(i < 3);
		mat[0][i] = v.x;
		mat[1][i] = v.y;
		mat[2][i] = v.z;
	}

	// ��������� �������� ������� �������
	void SetColumn(unsigned i, const CVector3<T>& v)
	{
		assert(i < 3);
		mat[i][0] = v.x;
		mat[i][1] = v.y;
		mat[i][2] = v.z;
	}

	T mat[3][3];
};

using CMatrix3d = CMatrix3<double>;
using CMatrix3f = CMatrix3<float>;
