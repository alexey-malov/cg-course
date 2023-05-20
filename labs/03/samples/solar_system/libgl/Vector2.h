#pragma once

#include "common_inc.h"

/************************************************************************/
/* ��������� ����� "���������� ������"                                  */
/************************************************************************/
template <class T>
class CVector2
{
public:
	/*
	����������� �� ���������
	*/
	CVector2()
		: x(0)
		, y(0)
	{
	}

	/*
	������������� ������� ���������� �� �������
	*/
	explicit CVector2(const T* p)
		: x(p[0])
		, y(p[1])
	{
	}

	/*
	���������������� ������������� ��������� �������
	*/
	CVector2(T const& x0, T const& y0)
		: x(x0)
		, y(y0)
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
	CVector2& operator*=(T const& scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	/*
	������� ������� �� ������
	*/
	CVector2& operator/=(T const& scalar)
	{
		return *this *= (1 / scalar);
	}

	/*
	������� ������� �� ������
	*/
	CVector2 const operator/(T const& scalar) const
	{
		return *this * (1 / scalar);
	}

	/*
	�������� ��������
	*/
	CVector2 const operator-(CVector2 const& v) const
	{
		return CVector2(x - v.x, y - v.y);
	}

	/*
	������� �����
	*/
	CVector2 const operator-() const
	{
		return CVector2(-x, -y);
	}

	/*
	������� ����
	*/
	CVector2 const operator+() const
	{
		return *this;
	}

	/*
	�������������� ������������ ��������
	*/
	CVector2 const operator*(CVector2 const& v) const
	{
		return CVector2(x * v.x, y * v.y);
	}

	/*
	�������������� ������� ��������
	*/
	CVector2 const operator/(CVector2 const& v) const
	{
		return CVector2(x / v.x, y / v.y);
	}

	/*
	����� ��������
	*/
	CVector2 const operator+(CVector2 const& v) const
	{
		return CVector2(x + v.x, y + v.y);
	}

	/*
	�������� ��������
	*/
	CVector2& operator+=(CVector2 const& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	/*
	�������������� ��������� ��������
	*/
	CVector2& operator*=(CVector2 const& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	/*
	�������������� ������� ��������
	*/
	CVector2& operator/=(CVector2 const& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	/*
	��������� ��������
	*/
	CVector2& operator-=(CVector2 const& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	/*
	��������� ������� �� ������
	*/
	CVector2 const operator*(T const& scale) const
	{
		return CVector2(x * scale, y * scale);
	}

	/*
	���������� ����� �������
	*/
	T GetLength() const
	{
		return static_cast<T>(std::sqrt(x * x + y * y));
	}

	// ������������ �������
	void Normalize()
	{
		T invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
	}

	T x;
	T y;
};

template <class T>
CVector2<T> const operator*(T const& scalar, CVector2<T> const& v)
{
	return v * scalar;
}

using CVector2f = CVector2<float>;
using CVector2d = CVector2<double>;
