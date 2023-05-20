#pragma once

#include "Vector3.h"

/************************************************************************/
/* ��������� ����� "������������� ������"                               */
/************************************************************************/
template <class T>
class CVector4
{
public:
	T x;
	T y;
	T z;
	T w;

	/*
	����������� �� ���������
	*/
	CVector4(void)
		: x(0)
		, y(0)
		, z(0)
		, w(0)
	{
	}

	/*
	������������� ������� ���������� �� �������
	*/
	explicit CVector4(const T* p)
		: x(p[0])
		, y(p[1])
		, z(p[2])
		, w(p[3])
	{
	}

	/*
	���������������� ������������� ��������� �������
	*/
	CVector4(T const& x0, T const& y0, T const& z0, T const& w0 = 0)
		: x(x0)
		, y(y0)
		, z(z0)
		, w(w0)
	{
	}

	/*
	�������������� 3-� ������������� ������� � 4-� ������������
	*/
	CVector4(CVector3<T> const& v, T const& w0 = 0)
		: x(v.x)
		, y(v.y)
		, z(v.z)
		, w(w0)
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
	CVector4& operator*=(T scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return *this;
	}

	/*
	������� ������� �� ������
	*/
	CVector4& operator/=(T const& scalar)
	{
		return *this *= (1 / scalar);
	}

	/*
	������� ������� �� ������
	*/
	CVector4 const operator/(T const& scalar) const
	{
		return *this * (1 / scalar);
	}

	/*
	�������� ��������
	*/
	CVector4 const operator-(CVector4 const& v) const
	{
		return CVector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	/*
	������� �����
	*/
	CVector4 const operator-() const
	{
		return CVector4(-x, -y, -z, -w);
	}

	/*
	������� ����
	*/
	CVector4 const operator+() const
	{
		return *this;
	}

	/*
	�������������� ������������ ��������
	*/
	CVector4 const operator*(CVector4 const& v) const
	{
		return CVector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	/*
	�������������� ������� ��������
	*/
	CVector4 const operator/(CVector4 const& v) const
	{
		return CVector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	/*
	����� ��������
	*/
	CVector4 const operator+(CVector4 const& v) const
	{
		return CVector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	/*
	�������� ��������
	*/
	CVector4& operator+=(CVector4 const& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	/*
	�������������� ��������� ��������
	*/
	CVector4& operator*=(CVector4 const& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	/*
	�������������� ������� ��������
	*/
	CVector4& operator/=(CVector4 const& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	/*
	��������� ��������
	*/
	CVector4& operator-=(CVector4 const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	/*
	��������� ������� �� ������
	*/
	CVector4 const operator*(T scale) const
	{
		return CVector4(x * scale, y * scale, z * scale, w * scale);
	}

	/*
	���������� ����� �������
	*/
	T GetLength() const
	{
		return static_cast<T>(std::sqrt(x * x + y * y + z * z + w * w));
	}

	/*
	�������������� � ����������� ������� ����� ������������ ���������� w
	*/
	operator CVector3<T>() const
	{
		return CVector3<T>(x, y, z);
	}

	/*
	�������������� � ����������� ������� ����� ���������� �������������� �������
	*/
	CVector3<T> Project() const
	{
		T invW = 1 / w;
		return CVector3<T>(x * invW, y * invW, z * invW);
	}

	// ������������ �������
	void Normalize()
	{
		T invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}
};

template <class T>
CVector4<T> const operator*(T const& scalar, CVector4<T> const& v)
{
	return v * scalar;
}

using CVector4f = CVector4<float>;
using CVector4d = CVector4<double>;
