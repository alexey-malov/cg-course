#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

/************************************************************************/
/* Набор вспомогательных функций по работе с векторами и матрицами      */
/************************************************************************/

template <class T>
T Sqr(T const& x) throw()
{
	return x * x;
}

/************************************************************************/
/* Fract - взятие дробной части скаляра или координат вектора           */
/************************************************************************/
template <class T>
T Fract(T const& value) throw()
{
	return value - (T)floor(value);
}

template <class T>
CVector2<T> Fract(CVector2<T> const& arg) throw()
{
	return CVector2<T>(Fract(arg.x), Fract(arg.y));
}

template <class T>
CVector3<T> Fract(CVector3<T> const& arg) throw()
{
	return CVector3<T>(Fract(arg.x), Fract(arg.y), Fract(arg.z));
}

template <class T>
CVector4<T> Fract(CVector4<T> const& arg) throw()
{
	return CVector4<T>(Fract(arg.x), Fract(arg.y), Fract(arg.z), Fract(arg.w));
}

/***************************************************************************/
/* Step - возвращает 0, если значение параметра меньше некоторого значения */
/* и 1 в противном случае                                                  */
/****************************************************************************/
template <class T>
T Step(T const& edge, T const& x) throw()
{
	return x < edge ? 0 : 1;
}

template <class T>
CVector2<T> Step(T const& edge, CVector2<T>  const& vec) throw()
{
	return CVector2<T>(Step(edge, vec.x), Step(edge, vec.y));
}

template <class T>
CVector2<T> Step(CVector2<T> const& edge, CVector2<T>  const& vec) throw()
{
	return CVector2<T>(Step(edge.x, vec.x), Step(edge.y, vec.y));
}

template <class T>
CVector3<T> Step(T const& edge, CVector3<T>  const& vec) throw()
{
	return CVector3<T>(Step(edge, vec.x), Step(edge, vec.y), Step(edge, vec.z));
}

template <class T>
CVector3<T> Step(CVector3<T> const& edge, CVector3<T>  const& vec) throw()
{
	return CVector3<T>(Step(edge.x, vec.x), Step(edge.y, vec.y), Step(edge.z, vec.z));
}

template <class T>
CVector4<T> Step(T const& edge, CVector4<T>  const& vec) throw()
{
	return CVector4<T>(Step(edge, vec.x), Step(edge, vec.y), Step(edge, vec.z), Step(edge, vec.w));
}

template <class T>
CVector4<T> Step(CVector4<T> const& edge, CVector4<T> const& vec) throw()
{
	return CVector4<T>(Step(edge.x, vec.x), Step(edge.y, vec.y), Step(edge.z, vec.z), Step(edge.w, vec.w));
}

/************************************************************************/
/* Скалярное произведение векторов                                      */
/************************************************************************/
template <class T>
T Dot(CVector2<T> const& a, CVector2<T> const& b) throw()
{
	return (a.x * b.x + a.y * b.y);
}

template <class T>
inline T Dot(CVector3<T> const& a, CVector3<T> const& b) throw()
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}


template <class T>
inline T Dot(CVector4<T> const& a, CVector4<T> const& b) throw()
{
	return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

/************************************************************************/
/* Min - поиск минимума                                                 */
/************************************************************************/
template <class T>
T Min(T const & x, T const& y) throw()
{
	return x < y ? x : y;
}

template <class T>
inline CVector2<T> Min(CVector2<T> const& vec, T const& scalar) throw()
{
	return CVector2<T>(Min(vec.x, scalar), Min(vec.y, scalar));
};

template <class T>
inline CVector3<T> Min(CVector3<T> const& vec, T const& scalar) throw()
{
	return CVector3<T>(Min(vec.x, scalar), Min(vec.y, scalar), Min(vec.z, scalar));
};


template <class T>
inline CVector4<T> Min(CVector4<T> const& vec, T const& scalar) throw()
{
	return CVector4<T>(Min(vec.x, scalar), Min(vec.y, scalar), Min(vec.z, scalar), Min(vec.w, scalar));
};


/************************************************************************/
/* Max - поиск максимума                                                */
/************************************************************************/
template <class T>
T Max(T const& x, T const& y) throw()
{
	return x > y ? x : y;
}

template <class T>
inline CVector2<T> Max(CVector2<T> const& vec, T const& scalar) throw()
{
	return CVector2<T>(Max(vec.x, scalar), Max(vec.y, scalar));
};

template <class T>
inline CVector3<T> Max(CVector3<T> const& vec, T const& scalar) throw()
{
	return CVector3<T>(Max(vec.x, scalar), Max(vec.y, scalar), Max(vec.z, scalar));
};


template <class T>
inline CVector4<T> Max(CVector4<T> const& vec, T const& scalar) throw()
{
	return CVector4<T>(Max(vec.x, scalar), Max(vec.y, scalar), Max(vec.z, scalar), Max(vec.w, scalar));
};


/************************************************************************/
/* Clamp - приведение скаляра или вектора к заданному диапазону         */
/************************************************************************/
template <class T>
inline CVector2<T> Clamp(CVector2<T> const& vec, T const& minValue, T const& maxValue) throw()
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T>
inline CVector2<T> Clamp(CVector2<T> const& vec, CVector2<T> const& minValue, CVector2<T> const& maxValue) throw()
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T>
inline CVector3<T> Clamp(CVector3<T> const& vec, T const& minValue, T const& maxValue) throw()
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T>
inline CVector3<T> Clamp(CVector3<T> const& vec, CVector3<T> const& minValue, CVector3<T> const& maxValue) throw()
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T>
inline CVector4<T> Clamp(CVector4<T> const& vec, T const& minValue, T const& maxValue) throw()
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T>
inline CVector4<T> Clamp(CVector4<T> const& vec, CVector4<T> const& minValue, CVector4<T> const& maxValue) throw()
{
	return Max(Min(vec, maxValue), minValue);
}
/************************************************************************/
/* Нормализация векторов                                                */
/************************************************************************/
template <class T>
inline CVector2<T> Normalize(CVector2<T> const& vec) throw()
{
	CVector2<T> result(vec);
	result.Normalize();
	return result;
}

template <class T>
inline CVector3<T> Normalize(CVector3<T> const& vec) throw()
{
	CVector3<T> result(vec);
	result.Normalize();
	return result;
}

template <class T>
inline CVector4<T> Normalize(CVector4<T> const& vec) throw()
{
	CVector4<T> result(vec);
	result.Normalize();
	return result;
}

/************************************************************************/
/* Векторное произведение векторов                                      */
/************************************************************************/
template <class T>
CVector3<T> const Cross(CVector3<T> const & a, CVector3<T> const & b) throw()
{
	/*
	Векторное произведение векторов A и B равно определителю матрицы:
	      |i  j  k |
	AxB = |Ax Ay Az|
	      |Bx By Bz|
	*/
	return CVector3<T>(
		a.y * b.z - a.z * b.y, 
		a.z * b.x - a.x * b.z, 
		a.x * b.y - a.y * b.x );

}

/************************************************************************/
/* Трансформация векторов (произведение матрицы и вектора)              */
/************************************************************************/
template <class T>
CVector4<T> operator*(CMatrix4<T> const& mat, CVector4<T> const& vec) throw()
{
	return CVector4<T>(
		mat.a00 * vec[0] + mat.a01 * vec[1] + mat.a02 * vec[2] + mat.a03 * vec[3],
		mat.a10 * vec[0] + mat.a11 * vec[1] + mat.a12 * vec[2] + mat.a13 * vec[3],
		mat.a20 * vec[0] + mat.a21 * vec[1] + mat.a22 * vec[2] + mat.a23 * vec[3],
		mat.a30 * vec[0] + mat.a31 * vec[1] + mat.a32 * vec[2] + mat.a33 * vec[3]
		);
}

template <class T>
CVector3<T> operator*(CMatrix3<T> const& mat, CVector3<T> const& vec) throw()
{
	return CVector4<T>(
		mat.a00 * vec[0] + mat.a01 * vec[1] + mat.a02 * vec[2],
		mat.a10 * vec[0] + mat.a11 * vec[1] + mat.a12 * vec[2],
		mat.a20 * vec[0] + mat.a21 * vec[1] + mat.a22 * vec[2]
	);
}


