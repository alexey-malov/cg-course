#pragma once

#include "Vector3.h"

/************************************************************************/
/* Шаблонный класс матриц размером 3x3                                  */
/************************************************************************/
template <class T>
class CMatrix3
{
public:
	// Конструктор по умолчанию (загрузка единичной матрицы)
	CMatrix3() throw()
	{
		LoadIdentity();
	}

	// Инициализация матрицы значениями из массива столбцов
	explicit CMatrix3(T const * src) throw()
	{
		memcpy(&mat[0][0], src, sizeof(mat));
	}

	// Непосредственная инициализация значений матрицы
	CMatrix3(
		T const& v00, T const& v10, T const& v20,	// Элементы 0 столбца
		T const& v01, T const& v11, T const& v21,	// Элементы 1 столбца 
		T const& v02, T const& v12, T const& v22	// Элементы 2 столбца
		) throw()
	{
		mat[0][0] = v00;	mat[0][1] = v10;	mat[0][2] = v20;
		mat[1][0] = v01;	mat[1][1] = v11;	mat[1][2] = v21;
		mat[2][0] = v02;	mat[2][1] = v12;	mat[2][2] = v22;
	}

	/*
	Вычисление определителя матрицы
	*/
	T GetDeterminant()const throw()
	{
		return
			mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) - 
			mat[1][0] * (mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2]) +
			mat[2][0] * (mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2]);
	}

	/*
	Загрузка единичной матрицы
	*/
	void LoadIdentity() throw()
	{
		mat[0][1] = mat[0][2] = mat[1][0] = mat[1][2] = mat[2][0] = mat[2][1] = 0;
		mat[0][0] = mat[1][1] = mat[2][2] = 1;
	}

	/*
	Возвращаем вектор-столбец
	*/
	CVector3<T> GetRow(unsigned i)const throw()
	{
		assert(i < 3);
		return
			CVector3<T>(
			mat[0][i], 
			mat[1][i], 
			mat[2][i]
		);
	}

	/*
	Возвращаем вектор-строку
	*/
	CVector3<T> GetColumn(unsigned i)const throw()
	{
		assert(i < 3);
		return CVector3<T>(mat[i]);
	}

	// Установка значений строки матрицы
	void SetRow(unsigned i, const CVector3<T>& v) throw()
	{
		assert(i < 3);
		mat[0][i] = v.x;
		mat[1][i] = v.y;
		mat[2][i] = v.z;
	}

	// Установка значений столбца матрицы
	void SetColumn(unsigned i, const CVector3<T> & v) throw()
	{
		assert(i < 3);
		mat[i][0] = v.x;
		mat[i][1] = v.y;
		mat[i][2] = v.z;
	}


	union
	{
		T mat[3][3];
		struct
		{
			T a00, a10, a20;
			T a01, a11, a21;
			T a02, a12, a22;
		};
	};
};

typedef CMatrix3<double> CMatrix3d;
typedef CMatrix3<float> CMatrix3f;