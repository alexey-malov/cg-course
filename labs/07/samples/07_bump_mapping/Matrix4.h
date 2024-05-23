#pragma once

#include "Vector3.h"

template <class T>
class CMatrix4
{
public:

	CMatrix4(void)
	{
		LoadIdentity();
	}

	CMatrix4(T const * src)
	{
		memcpy(data, src, sizeof(data));
	}

	operator const T*()const
	{
		return &data[0];
	}

	operator T*()
	{
		return &data[0];
	}

	// Произведение матриц
	CMatrix4& operator*=(CMatrix4 const& rhs)
	{
		T result[4][4];

		result[0][0] = a00*rhs.a00 + a01*rhs.a10 + a02*rhs.a20 + a03*rhs.a30;
		result[0][1] = a10*rhs.a00 + a11*rhs.a10 + a12*rhs.a20 + a13*rhs.a30;
		result[0][2] = a20*rhs.a00 + a21*rhs.a10 + a22*rhs.a20 + a23*rhs.a30;
		result[0][3] = a30*rhs.a00 + a31*rhs.a10 + a32*rhs.a20 + a33*rhs.a30;

		result[1][0] = a00*rhs.a01 + a01*rhs.a11 + a02*rhs.a21 + a03*rhs.a31;
		result[1][1] = a10*rhs.a01 + a11*rhs.a11 + a12*rhs.a21 + a13*rhs.a31;
		result[1][2] = a20*rhs.a01 + a21*rhs.a11 + a22*rhs.a21 + a23*rhs.a31;
		result[1][3] = a30*rhs.a01 + a31*rhs.a11 + a32*rhs.a21 + a33*rhs.a31;

		result[2][0] = a00*rhs.a02 + a01*rhs.a12 + a02*rhs.a22 + a03*rhs.a32;
		result[2][1] = a10*rhs.a02 + a11*rhs.a12 + a12*rhs.a22 + a13*rhs.a32;
		result[2][2] = a20*rhs.a02 + a21*rhs.a12 + a22*rhs.a22 + a23*rhs.a32;
		result[2][3] = a30*rhs.a02 + a31*rhs.a12 + a32*rhs.a22 + a33*rhs.a32;

		result[3][0] = a00*rhs.a03 + a01*rhs.a13 + a02*rhs.a23 + a03*rhs.a33;
		result[3][1] = a10*rhs.a03 + a11*rhs.a13 + a12*rhs.a23 + a13*rhs.a33;
		result[3][2] = a20*rhs.a03 + a21*rhs.a13 + a22*rhs.a23 + a23*rhs.a33;
		result[3][3] = a30*rhs.a03 + a31*rhs.a13 + a32*rhs.a23 + a33*rhs.a33;
		memcpy(data, result, sizeof(data));

		return *this;
	}

	// Произведение матриц
	CMatrix4 const operator*(CMatrix4 const& rhs)const
	{
		T result[4][4];

		result[0][0] = a00*rhs.a00 + a01*rhs.a10 + a02*rhs.a20 + a03*rhs.a30;
		result[0][1] = a10*rhs.a00 + a11*rhs.a10 + a12*rhs.a20 + a13*rhs.a30;
		result[0][2] = a20*rhs.a00 + a21*rhs.a10 + a22*rhs.a20 + a23*rhs.a30;
		result[0][3] = a30*rhs.a00 + a31*rhs.a10 + a32*rhs.a20 + a33*rhs.a30;

		result[1][0] = a00*rhs.a01 + a01*rhs.a11 + a02*rhs.a21 + a03*rhs.a31;
		result[1][1] = a10*rhs.a01 + a11*rhs.a11 + a12*rhs.a21 + a13*rhs.a31;
		result[1][2] = a20*rhs.a01 + a21*rhs.a11 + a22*rhs.a21 + a23*rhs.a31;
		result[1][3] = a30*rhs.a01 + a31*rhs.a11 + a32*rhs.a21 + a33*rhs.a31;

		result[2][0] = a00*rhs.a02 + a01*rhs.a12 + a02*rhs.a22 + a03*rhs.a32;
		result[2][1] = a10*rhs.a02 + a11*rhs.a12 + a12*rhs.a22 + a13*rhs.a32;
		result[2][2] = a20*rhs.a02 + a21*rhs.a12 + a22*rhs.a22 + a23*rhs.a32;
		result[2][3] = a30*rhs.a02 + a31*rhs.a12 + a32*rhs.a22 + a33*rhs.a32;

		result[3][0] = a00*rhs.a03 + a01*rhs.a13 + a02*rhs.a23 + a03*rhs.a33;
		result[3][1] = a10*rhs.a03 + a11*rhs.a13 + a12*rhs.a23 + a13*rhs.a33;
		result[3][2] = a20*rhs.a03 + a21*rhs.a13 + a22*rhs.a23 + a23*rhs.a33;
		result[3][3] = a30*rhs.a03 + a31*rhs.a13 + a32*rhs.a23 + a33*rhs.a33;

		return CMatrix4(&result[0][0]);
	}

	// Загрузка матрицы переноса
	void LoadTranslation(T dx, T dy, T dz)
	{
		LoadIdentity();
		x = dx;
		y = dy;
		z = dz;
	}

	// Умножение текущей матрицы на матрицу переноса
	void Translate(T dx, T dy, T dz)
	{
		CMatrix4 translationMatrix;
		translationMatrix.x = dx;
		translationMatrix.y = dy;
		translationMatrix.z = dz;

		*this *= translationMatrix;
	}

	// Загрузка матрицы масштабирования
	void LoadScale(T sx0, T sy0, T sz0)
	{
		LoadIdentity();
		sx = sx0;
		sy = sy0;
		sz = sz0;
	}

	// Умножение на матрицу масштабирования
	void Scale(T sx0, T sy0, T sz0)
	{
		CMatrix4 scaleMatrix;
		scaleMatrix.sx = sx0;
		scaleMatrix.sy = sy0;
		scaleMatrix.sz = sz0;
		*this *= scaleMatrix;
	}

	// Загрузка матрицы вращения на угол вокруг заданной оси
	void LoadRotation(T angle, T ux, T uy, T uz)
	{
		angle = (T)(angle * 3.14159265358979323846 / 180);

		T inv_len = (T)(1.0 / sqrt(ux * ux + uy * uy + uz * uz));
		ux *= inv_len;
		uy *= inv_len;
		uz *= inv_len;

		T c = (T)cos(angle);
		T s = (T)sin(angle);
		T one_minus_c = (T)1 - c;

		T uxuy = ux * uy;
		T uxuz = ux * uz;
		T uyuz = uy * uz;

		T sux = s * ux;
		T suy = s * uy;
		T suz = s * uz;

		a00 = c + one_minus_c * ux * ux;
		a10 = one_minus_c * uxuy + suz;
		a20 = one_minus_c * uxuz - suy;

		a01 = one_minus_c * uxuy - suz;
		a11 = c + one_minus_c * uy * uy;
		a21 = one_minus_c * uyuz + sux;

		a02 = one_minus_c * uxuz + suy;
		a12 = one_minus_c * uyuz - sux;
		a22 = c + one_minus_c * uz * uz;

		a30 = a31 = a32 = a03 = a13 = a23 = 0;
		a33 = 1;
	}

	// Вращение на угол вокруг заданной оси
	void Rotate(T angle, T ux, T uy, T uz)
	{
		CMatrix4 rotationMatrix;
		rotationMatrix.LoadRotation(angle, ux, uy, uz);
		*this *= rotationMatrix;
	}

	// Установка значений строки матрицы
	void SetRow(size_t i, const CVector3<T>& v, T const & w = 0)
	{
		assert(i < 4);
		mat[0][i] = v.x;
		mat[1][i] = v.y;
		mat[2][i] = v.z;
		mat[3][i] = w;
	}

	// Установка значений столбца матрицы
	void SetColumn(size_t i, const CVector3<T> & v, T const& w = 0)
	{
		assert(i < 4);
		mat[i][0] = v.x;
		mat[i][1] = v.y;
		mat[i][2] = v.z;
		mat[i][3] = w;
	}

	// Загрузка правосторонней матрицы камеры (аналогично gluLookAt)
	void LoadLookAtRH(T eyeX, T eyeY, T eyeZ, T atX, T atY, T atZ, T upX, T upY, T upZ)
	{
		CVector3<T> eye(eyeX, eyeY, eyeZ);
		CVector3<T> at(atX, atY, atZ);
		CVector3<T> up(upX, upY, upZ);

		CVector3<T> zaxis = (eye - at);
		zaxis.Normalize();
		CVector3<T> xaxis = Cross(up, zaxis);
		xaxis.Normalize();
		CVector3<T> yaxis = Cross(zaxis, xaxis);
		yaxis.Normalize();


		SetRow(0, xaxis);
		SetRow(1, yaxis);
		SetRow(2, zaxis);
		ClearRow(3);
		x = -Dot(xaxis, eye);
		y = -Dot(yaxis, eye);
		z = -Dot(zaxis, eye);
		w = 1;
	}

	// Умножение текущей матрицы на матрицу камеры
	void LookAtRH(T eyeX, T eyeY, T eyeZ, T atX, T atY, T atZ, T upX, T upY, T upZ)
	{
		CMatrix4 lookAtMatrix;
		lookAtMatrix.LoadLookAtRH(eyeX, eyeY, eyeZ, atX, atY, atZ, upX, upY, upZ);
		*this *= lookAtMatrix;
	}

	// Очистка столбца
	void ClearColumn(size_t i)
	{
		assert(i < 4)
		mat[i][0] = 0;
		mat[i][1] = 0;
		mat[i][2] = 0;
		mat[i][3] = 0;
	}

	// Очистка строки
	void ClearRow(size_t i)
	{
		assert(i < 4);
		mat[0][i] = 0;
		mat[1][i] = 0;
		mat[2][i] = 0;
		mat[3][i] = 0;
	}

	// Загрузка единичной матрицы
	void LoadIdentity()
	{
		static const T identityMatrix[4][4] = 
		{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1},
		};

		memcpy(data, identityMatrix, sizeof(identityMatrix));
	}

	void Normalize()
	{
		/*
		Ортонормирование - приведение координатных осей к единичной длине (нормирование)
		и взаимной перпендикулярности (ортогонализация)
		Достичь этого можно при помощи нормализации координатных осей
		и векторного произведения
		*/
		CVector3<T> xAxis(data[0], data[4], data[8]);
		xAxis.Normalize();	
		CVector3<T> yAxis(data[1], data[5], data[9]);
		yAxis.Normalize();

		// Ось Z вычисляем через векторное произведение X и Y
		// Z будет перпендикулярна плоскости векторов X и Y
		CVector3<T> zAxis = Cross(xAxis, yAxis);
		// И иметь единичную длину
		zAxis.Normalize();
		// То же самое проделываем с осями x и y
		xAxis = Cross(yAxis, zAxis);
		xAxis.Normalize();

		yAxis = Cross(zAxis, xAxis);
		yAxis.Normalize();

		// Сохраняем вектора координатных осей обратно в матрицу
		SetRow(0, xAxis, x);
		SetRow(1, yAxis, y);
		SetRow(2, zAxis, z);
	}

	// Коэффициенты матрицы
	union
	{
		struct 
		{
			union
			{
				T a00;
				T sx;
			};
			T a10, a20;
			union
			{
				T a30;
				T p;
			};

			T a01;
			union
			{
				T a11;
				T sy;
			};
			T a21;
			union
			{
				T a31;
				T q;
			};

			T a02;
			T a12;
			union
			{
				T a22;
				T sz;
			};
			union
			{
				T a32;
				T r;
			};

			union
			{
				T a03;
				T x;
			};
			union
			{
				T a13;
				T y;
			};
			union
			{
				T a23;
				T z;
			};
			union
			{
				T a33;
				T w;
			};
		};

		T mat[4][4];
		T data[16];
	};
};

typedef CMatrix4<double> CMatrix4d;
typedef CMatrix4<float> CMatrix4f;
