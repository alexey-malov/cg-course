#pragma once

template <class T>
class CVector2
{
public:
	T x, y;

	CVector2(void)
		:x(0), y(0)
	{
	}

	explicit CVector2(const T * p)
		:x(p[0]), y(p[1])
	{
	}

	CVector2(T x0, T y0)
		:x(x0), y(y0)
	{
	}

	operator T const*()const
	{
		return &x;
	}

	CVector2 & operator *= (T scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	CVector2 const operator-(CVector2 const& v)const
	{
		return CVector2(x - v.x, y - v.y);
	}

	CVector2 const operator+(CVector2 const& v)const
	{
		return CVector2(x + v.x, y + v.y);
	}

	CVector2 const operator*(T scale)const
	{
		return CVector2(x * scale, y * scale);
	}

	T GetLength()const
	{
		return static_cast<T>(sqrt(x * x + y * y));
	}

	// Нормализуем
	void Normalize()
	{
		float invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
	}
};

template <class T>
T Dot(CVector2<T> const& a, CVector2<T> const& b)
{
	return (a.x * b.x + a.y * b.y);
}

typedef CVector2<float> CVector2f;
typedef CVector2<double> CVector2d;
