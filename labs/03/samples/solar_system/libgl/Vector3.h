#pragma once

#include "Vector2.h"

/************************************************************************/
/* ��������� ����� "���������� ������"                                  */
/************************************************************************/
template <class T>
class CVector3
{
public:
	T x, y, z;

	/*
	����������� �� ���������
	*/
	CVector3(void)
		: x(0)
		, y(0)
		, z(0)
	{
	}

	/*
	������������� ������� ���������� �� �������
	*/
	explicit CVector3(const T* p)
		: x(p[0])
		, y(p[1])
		, z(p[2])
	{
	}

	/*
	���������������� ������������� ��������� �������
	*/
	CVector3(T const& x0, T const& y0, T const& z0)
		: x(x0)
		, y(y0)
		, z(z0)
	{
	}

	/*
	�������������� 2-� ������������� ������� � 3-� ������������
	*/
	CVector3(CVector2<T> const& v, T const& z0 = 0)
		: x(v.x)
		, y(v.y)
		, z(z0)
	{
	}

	/*
	��������� ������ ������� �������� ����������� �������
	*/
	operator T const*() const
	{
		return &x;
	}

	/*
	��������������� �������
	*/
	CVector3& operator*=(T const& scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}

	/*
	������� ������� �� ������
	*/
	CVector3& operator/=(T const& scalar)
	{
		return *this *= (1 / scalar);
	}

	/*
	������� ������� �� ������
	*/
	CVector3 const operator/(T const& scalar) const
	{
		return *this * (1 / scalar);
	}

	/*
	�������� ��������
	*/
	CVector3 const operator-(CVector3 const& v) const
	{
		return CVector3(x - v.x, y - v.y, z - v.z);
	}

	/*
	������� �����
	*/
	CVector3 const operator-() const
	{
		return CVector3(-x, -y, -z);
	}

	/*
	������� ����
	*/
	CVector3 const operator+() const
	{
		return *this;
	}

	/*
	�������������� ������������ ��������
	*/
	CVector3 const operator*(CVector3 const& v) const
	{
		return CVector3(x * v.x, y * v.y, z * v.z);
	}

	/*
	�������������� ������� ��������
	*/
	CVector3 const operator/(CVector3 const& v) const
	{
		return CVector3(x / v.x, y / v.y, z / v.z);
	}

	/*
	����� ��������
	*/
	CVector3 const operator+(CVector3 const& v) const
	{
		return CVector3(x + v.x, y + v.y, z + v.z);
	}

	/*
	�������� ��������
	*/
	CVector3& operator+=(CVector3 const& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	/*
	�������������� ��������� ��������
	*/
	CVector3& operator*=(CVector3 const& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	/*
	�������������� ������� ��������
	*/
	CVector3& operator/=(CVector3 const& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	/*
	��������� ��������
	*/
	CVector3& operator-=(CVector3 const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	/*
	��������� ������� �� ������
	*/
	CVector3 const operator*(T const& scale) const
	{
		return CVector3(x * scale, y * scale, z * scale);
	}

	/*
	���������� ����� �������
	*/
	T GetLength() const
	{
		return static_cast<T>(std::sqrt(x * x + y * y + z * z));
	}

	// ������������ �������
	void Normalize()
	{
		T invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}
};

template <class T>
CVector3<T> const operator*(T const& scalar, CVector3<T> const& v)
{
	return v * scalar;
}

using CVector3f = CVector3<float>;
using CVector3d = CVector3<double>;
