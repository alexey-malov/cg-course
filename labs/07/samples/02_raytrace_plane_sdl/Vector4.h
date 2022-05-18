#pragma once

#include "Vector3.h"

/************************************************************************/
/* Шаблонный класс "Четырехмерный вектор"                               */
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
	Конструктор по умолчанию
	*/
	CVector4(void) throw()
		:x(0), y(0), z(0), w(0)
	{
	}

	/*
	Инициализация вектора значениями из массива
	*/
	explicit CVector4(const T * p) throw()
		:x(p[0]), y(p[1]), z(p[2]), w(p[3])
	{
	}

	/*
	Непосредственная инициализация элементов вектора
	*/
	CVector4(T const& x0, T const& y0, T const& z0, T const& w0 = 0) throw()
		:x(x0), y(y0), z(z0), w(w0)
	{
	}

	/*
	Преобразование 3-х компонентного вектора в 4-х компонентный
	*/
	CVector4(CVector3<T> const& v, T const& w0 = 0) throw()
		:x(v.x), y(v.y), z(v.z), w(w0)
	{
	}

	/*
	Получение адреса массива значений компонентов вектора
	*/
	operator T const*()const throw()
	{
		return &x;
	}

	/*
	Масштабирование вектора
	*/
	CVector4 & operator *= (T scale) throw()
	{
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return *this;
	}


	/*
	Деление вектора на скаляр
	*/
	CVector4 & operator /= (T const& scalar) throw()
	{
		return *this *= (1/scalar);
	}

	/*
	Деление вектора на скаляр
	*/
	CVector4 const operator / (T const& scalar)const throw()
	{
		return *this * (1/scalar);
	}

	/*
	Разность векторов
	*/
	CVector4 const operator-(CVector4 const& v)const throw()
	{
		return CVector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}


	/*
	Унарный минус
	*/
	CVector4 const operator-()const throw()
	{
		return CVector4(-x, -y, -z, -w);
	}

	/*
	Унарный плюс
	*/
	CVector4 const operator+()const throw()
	{
		return *this;
	}

	/*
	Покомпонентное произведение векторов
	*/
	CVector4 const operator*(CVector4 const& v)const throw()
	{
		return CVector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	/*
	Покомпонентное деление векторов
	*/
	CVector4 const operator/(CVector4 const& v)const throw()
	{
		return CVector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	/*
	Сумма векторов
	*/
	CVector4 const operator+(CVector4 const& v)const throw()
	{
		return CVector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	/*
	Сложение векторов
	*/
	CVector4 & operator+=(CVector4 const& v) throw()
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	/*
	Покомпонентное умножение векторов
	*/
	CVector4 & operator*=(CVector4 const& v) throw()
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	/*
	Покомпонентное деление векторов
	*/
	CVector4 & operator/=(CVector4 const& v) throw()
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	/*
	Вычитание векторов
	*/
	CVector4 & operator-=(CVector4 const& v) throw()
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	/*
	Умножение вектора на скаляр
	*/
	CVector4 const operator*(T scale)const throw()
	{
		return CVector4(x * scale, y * scale, z * scale, w * scale);
	}

	/*
	Вычисление длины вектора
	*/
	T GetLength()const throw()
	{
		return static_cast<T>(sqrt(x * x + y * y + z * z + w * w));
	}

	/*
	Преобразование к трехмерному вектору путем отбрасывания координаты w
	*/
	operator CVector3<T> ()const throw()
	{
		return CVector3<T>(x, y, z);
	}

	/*
	Преобразование к трехмерному вектору путем выполнения перспективного деления
	*/
	CVector3<T> Project()const throw()
	{
		T invW = 1 / w;
		return CVector3<T>(x * invW, y * invW, z * invW);
	}

	// Нормализация вектора
	void Normalize() throw()
	{
		T invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}
};


template<class T>
CVector4<T> const operator*(T const& scalar, CVector4<T> const& v) throw()
{
	return v * scalar;
}

typedef CVector4<float> CVector4f;
typedef CVector4<double> CVector4d;
