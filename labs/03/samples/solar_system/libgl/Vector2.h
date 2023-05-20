#pragma once

#include "common_inc.h"

/************************************************************************/
/* Шаблонный класс "Двухмерный вектор"                                  */
/************************************************************************/
template <class T>
class CVector2
{
public:
	/*
	Конструктор по умолчанию
	*/
	CVector2()
		: x(0)
		, y(0)
	{
	}

	/*
	Инициализация вектора значениями из массива
	*/
	explicit CVector2(const T* p)
		: x(p[0])
		, y(p[1])
	{
	}

	/*
	Непосредственная инициализация элементов вектора
	*/
	CVector2(T const& x0, T const& y0)
		: x(x0)
		, y(y0)
	{
	}

	/*
	Получение адреса массива значений компонентов вектора
	*/
	operator T const*() const
	{
		return &x;
	}

	/*
	Масштабирование вектора
	*/
	CVector2& operator*=(T const& scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	/*
	Деление вектора на скаляр
	*/
	CVector2& operator/=(T const& scalar)
	{
		return *this *= (1 / scalar);
	}

	/*
	Деление вектора на скаляр
	*/
	CVector2 const operator/(T const& scalar) const
	{
		return *this * (1 / scalar);
	}

	/*
	Разность векторов
	*/
	CVector2 const operator-(CVector2 const& v) const
	{
		return CVector2(x - v.x, y - v.y);
	}

	/*
	Унарный минус
	*/
	CVector2 const operator-() const
	{
		return CVector2(-x, -y);
	}

	/*
	Унарный плюс
	*/
	CVector2 const operator+() const
	{
		return *this;
	}

	/*
	Покомпонентное произведение векторов
	*/
	CVector2 const operator*(CVector2 const& v) const
	{
		return CVector2(x * v.x, y * v.y);
	}

	/*
	Покомпонентное деление векторов
	*/
	CVector2 const operator/(CVector2 const& v) const
	{
		return CVector2(x / v.x, y / v.y);
	}

	/*
	Сумма векторов
	*/
	CVector2 const operator+(CVector2 const& v) const
	{
		return CVector2(x + v.x, y + v.y);
	}

	/*
	Сложение векторов
	*/
	CVector2& operator+=(CVector2 const& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	/*
	Покомпонентное умножение векторов
	*/
	CVector2& operator*=(CVector2 const& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	/*
	Покомпонентное деление векторов
	*/
	CVector2& operator/=(CVector2 const& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	/*
	Вычитание векторов
	*/
	CVector2& operator-=(CVector2 const& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	/*
	Умножение вектора на скаляр
	*/
	CVector2 const operator*(T const& scale) const
	{
		return CVector2(x * scale, y * scale);
	}

	/*
	Вычисление длины вектора
	*/
	T GetLength() const
	{
		return static_cast<T>(std::sqrt(x * x + y * y));
	}

	// Нормализация вектора
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
