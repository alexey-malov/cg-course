#pragma once

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
	CVector2(void) throw()
		:x(0),y(0)
	{
	}

	/*
	Инициализация вектора значениями из массива
	*/
	explicit CVector2(const T * p) throw()
		:x(p[0]), y(p[1])
	{
	}

	/*
	Непосредственная инициализация элементов вектора
	*/
	CVector2(T const& x0, T const& y0) throw()
		:x(x0), y(y0)
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
	CVector2 & operator *= (T const& scale) throw()
	{
		x *= scale;
		y *= scale;
		return *this;
	}


	/*
	Деление вектора на скаляр
	*/
	CVector2 & operator /= (T const& scalar) throw()
	{
		return *this *= (1/scalar);
	}

	/*
	Деление вектора на скаляр
	*/
	CVector2 const operator / (T const& scalar)const throw()
	{
		return *this * (1/scalar);
	}


	/*
	Разность векторов
	*/
	CVector2 const operator-(CVector2 const& v)const throw()
	{
		return CVector2(x - v.x, y - v.y);
	}

	/*
	Унарный минус
	*/
	CVector2 const operator-()const throw()
	{
		return CVector2(-x, -y);
	}

	/*
	Унарный плюс
	*/
	CVector2 const operator+()const throw()
	{
		return *this;
	}

	/*
	Покомпонентное произведение векторов
	*/
	CVector2 const operator*(CVector2 const& v)const throw()
	{
		return CVector2(x * v.x, y * v.y);
	}

	/*
	Покомпонентное деление векторов
	*/
	CVector2 const operator/(CVector2 const& v)const throw()
	{
		return CVector2(x / v.x, y / v.y);
	}

	/*
	Сумма векторов
	*/
	CVector2 const operator+(CVector2 const& v)const throw()
	{
		return CVector2(x + v.x, y + v.y);
	}

	/*
	Сложение векторов
	*/
	CVector2 & operator+=(CVector2 const& v) throw()
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	/*
	Покомпонентное умножение векторов
	*/
	CVector2 & operator*=(CVector2 const& v) throw()
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	/*
	Покомпонентное деление векторов
	*/
	CVector2 & operator/=(CVector2 const& v) throw()
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	/*
	Вычитание векторов
	*/
	CVector2 & operator-=(CVector2 const& v) throw()
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	/*
	Умножение вектора на скаляр
	*/
	CVector2 const operator*(T const& scale)const throw()
	{
		return CVector2(x * scale, y * scale);
	}

	/*
	Вычисление длины вектора
	*/
	T GetLength()const throw()
	{
		return static_cast<T>(sqrt(x * x + y * y));
	}

	// Нормализация вектора
	void Normalize() throw()
	{
		T invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
	}

	T x;
	T y;
};

template<class T>
CVector2<T> const operator*(T const& scalar, CVector2<T> const& v) throw()
{
	return v * scalar;
}

typedef CVector2<float> CVector2f;
typedef CVector2<double> CVector2d;
