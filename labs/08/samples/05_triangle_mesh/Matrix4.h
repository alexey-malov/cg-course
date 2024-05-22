#pragma once

#include "Vector4.h"
#include "Matrix3.h"

/************************************************************************/
/* Шаблонный класс матриц размером 4x4                                  */
/************************************************************************/
template <class T>
class CMatrix4
{
public:
	/*
	Конструктор по умолчанию - создает единичную матрицу
	*/
	CMatrix4(void) noexcept
	{
		LoadIdentity();
	}

	/*
	Инициализация матрицы значениями из массива значений-столбцов 
	*/
	CMatrix4(T const * src) noexcept
	{
		memcpy(data, src, sizeof(data));
	}

	/*
	Непосредственная инициализация значений матрицы
	*/
	explicit CMatrix4(
		T const& v00, T const& v10, T const& v20, T const& v30,	// Элементы 0 столбца
		T const& v01, T const& v11, T const& v21, T const& v31,	// Элементы 1 столбца
		T const& v02, T const& v12, T const& v22, T const& v32,	// Элементы 2 столбца
		T const& v03, T const& v13, T const& v23, T const& v33	// Элементы 3 столбца
		) noexcept
	{
		a00 = v00; a10 = v10; a20 = v20; a30 = v30;
		a01 = v01; a11 = v11; a21 = v21; a31 = v31;
		a02 = v02; a12 = v12; a22 = v22; a32 = v32;
		a03 = v03; a13 = v13; a23 = v23; a33 = v33;
	}

	/*
	Доступ к массиву столбцов матрицы
	*/
	operator const T*()const noexcept
	{
		return &data[0];
	}

	/*
	Доступ к массиву столбцов матрицы
	*/
	operator T*() noexcept
	{
		return &data[0];
	}

	// Произведение матриц
	CMatrix4& operator*=(CMatrix4 const& rhs) noexcept
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

	// Умножение элементов матрицы на скаляр
	CMatrix4& operator*=(T s) noexcept
	{
		data[0] *= s;
		data[1] *= s;
		data[2] *= s;
		data[3] *= s;
		data[4] *= s;
		data[5] *= s;
		data[6] *= s;
		data[7] *= s;
		data[8] *= s;
		data[9] *= s;
		data[10] *= s;
		data[11] *= s;
		data[12] *= s;
		data[13] *= s;
		data[14] *= s;
		data[15] *= s;
		return *this;
	}

	// Произведение матриц (по правилу матричного умножения)
	CMatrix4 const operator*(CMatrix4 const& rhs)const noexcept
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

	// Умножение элементов матрицы на скаляр
	CMatrix4 const operator*(T s)const noexcept
	{
		return CMatrix4(
			data[0] * s,
			data[1] * s,
			data[2] * s,
			data[3] * s,
			data[4] * s,
			data[5] * s,
			data[6] * s,
			data[7] * s,
			data[8] * s,
			data[9] * s,
			data[10] * s,
			data[11] * s,
			data[12] * s,
			data[13] * s,
			data[14] * s,
			data[15] * s
			);
	}

	// Загрузка матрицы переноса
	void LoadTranslation(T dx, T dy, T dz) noexcept
	{
		LoadIdentity();
		x = dx;
		y = dy;
		z = dz;
	}

	// Умножение текущей матрицы на матрицу переноса
	void Translate(T dx, T dy, T dz) noexcept
	{
		CMatrix4 translationMatrix;
		translationMatrix.x = dx;
		translationMatrix.y = dy;
		translationMatrix.z = dz;

		*this *= translationMatrix;
	}

	// Загрузка матрицы масштабирования
	void LoadScale(T sx0, T sy0, T sz0) noexcept
	{
		LoadIdentity();
		sx = sx0;
		sy = sy0;
		sz = sz0;
	}

	// Умножение на матрицу масштабирования
	void Scale(T sx0, T sy0, T sz0) noexcept
	{
		CMatrix4 scaleMatrix;
		scaleMatrix.sx = sx0;
		scaleMatrix.sy = sy0;
		scaleMatrix.sz = sz0;
		*this *= scaleMatrix;
	}

	// Загрузка матрицы вращения на угол вокруг заданной оси
	void LoadRotation(T angle, T ux, T uy, T uz) noexcept
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
	void Rotate(T angle, T ux, T uy, T uz) noexcept
	{
		CMatrix4 rotationMatrix;
		rotationMatrix.LoadRotation(angle, ux, uy, uz);
		*this *= rotationMatrix;
	}

	/*
	Возвращаем вектор-столбец
	*/
	CVector4<T> GetRow(unsigned i)const noexcept
	{
		assert(i < 4);
		return
			CVector4<T>(
			mat[0][i], 
			mat[1][i], 
			mat[2][i], 
			mat[3][i]
		);
	}

	/*
	Возвращаем вектор-строку
	*/
	CVector4<T> GetColumn(unsigned i)const noexcept
	{
		assert(i < 4);
		return CVector4<T>(mat[i]);
	}

	// Установка значений строки матрицы
	void SetRow(unsigned i, const CVector4<T>& v) noexcept
	{
		assert(i < 4);
		mat[0][i] = v.x;
		mat[1][i] = v.y;
		mat[2][i] = v.z;
		mat[3][i] = v.w;
	}

	// Установка значений столбца матрицы
	void SetColumn(unsigned i, const CVector4<T> & v) noexcept
	{
		assert(i < 4);
		mat[i][0] = v.x;
		mat[i][1] = v.y;
		mat[i][2] = v.z;
		mat[i][3] = v.w;
	}


	/*
	Загрузка матрицы перспективного преобразования (аналогично glFrustum)
	*/
	void LoadFrustum(T left, T right, T bottom, T top, T zNear, T zFar) noexcept
	{
		const T n2 = zNear + zNear;
		const T invRminusL = 1 / (right - left);
		const T invFminusN = 1 / (zFar - zNear);
		const T invTminusB = 1 / (top - bottom);
		
		a00 = n2 * invRminusL;	// 2*near/(right-left);
		a10 = a20 = a30 = a01 = 0;
		a11 = n2 * invTminusB;	// 2*near/(top-bottom)
		a21 = a31 = 0;
		a02 = (right + left) * invRminusL;	// (right+left)/(right-left);
		a12 = (top + bottom) * invTminusB;	// (top+bottom)/(top-bottom)
		a22 = -(zFar + zNear) * invFminusN;	// -(far+near)/(far-near)
		a32 = -1;
		a03 = a13 = 0;
		a23 = -n2 * zFar * invFminusN;		// -2*far*near/(far-near)
		a33 = 0;
	}

	/*
	Загрузка матрицы перспективного преобразования аналогично gluPerspective
	*/
	void LoadPerspective(double fovY, double aspect, double zNear, double zFar) noexcept
	{
		fovY = (T)(fovY * 3.14159265358979323846 / 180);
		T top = zNear * tan(fovY * 0.5);
		T right = aspect * top;
		LoadFrustum(-right, right, -top, top, zNear, zFar);
	}

	/*
	Вычисление определителя матрицы
	*/
	T GetDeterminant()const noexcept
	{
		// Быстрое вычисление определителя матрицы 4*4 взято отсюда:
		// http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
		return (
			a03 * a12 * a21 * a30 -
			a02 * a13 * a21 * a30 -
			a03 * a11 * a22 * a30 +
			a01 * a13 * a22 * a30 +

			a02 * a11 * a23 * a30 -
			a01 * a12 * a23 * a30 -
			a03 * a12 * a20 * a31 +
			a02 * a13 * a20 * a31 +

			a03 * a10 * a22 * a31 -
			a00 * a13 * a22 * a31 -
			a02 * a10 * a23 * a31 +
			a00 * a12 * a23 * a31 +

			a03 * a11 * a20 * a32 -
			a01 * a13 * a20 * a32 -
			a03 * a10 * a21 * a32 +
			a00 * a13 * a21 * a32 +

			a01 * a10 * a23 * a32 -
			a00 * a11 * a23 * a32 -
			a02 * a11 * a20 * a33 +
			a01 * a12 * a20 * a33 +

			a02 * a10 * a21 * a33 -
			a00 * a12 * a21 * a33 -
			a01 * a10 * a22 * a33 +
			a00 * a11 * a22 * a33
			);
	}

	/*
	Вычисление обратной матрицы
	*/
	CMatrix4<T> GetInverseMatrix()const noexcept
	{
		// Быстрое извлечение обратной матрицы 4*4. Принцип взят отсюда:
		// http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm

		T const & n11 = data[0], & n12 = data[4], & n13 = data[8], & n14 = data[12];
		T const & n21 = data[1], & n22 = data[5], & n23 = data[9], & n24 = data[13];
		T const & n31 = data[2], & n32 = data[6], & n33 = data[10], & n34 = data[14];
		T const & n41 = data[3], & n42 = data[7], & n43 = data[11], & n44 = data[15];

		T invDet = 1 / GetDeterminant();

		return CMatrix4(
			invDet * (n23*n34*n42 - n24*n33*n42 + n24*n32*n43 - n22*n34*n43 - n23*n32*n44 + n22*n33*n44),
			invDet * (n24*n33*n41 - n23*n34*n41 - n24*n31*n43 + n21*n34*n43 + n23*n31*n44 - n21*n33*n44),
			invDet * (n22*n34*n41 - n24*n32*n41 + n24*n31*n42 - n21*n34*n42 - n22*n31*n44 + n21*n32*n44),
			invDet * (n23*n32*n41 - n22*n33*n41 - n23*n31*n42 + n21*n33*n42 + n22*n31*n43 - n21*n32*n43),

			invDet * (n14*n33*n42 - n13*n34*n42 - n14*n32*n43 + n12*n34*n43 + n13*n32*n44 - n12*n33*n44),
			invDet * (n13*n34*n41 - n14*n33*n41 + n14*n31*n43 - n11*n34*n43 - n13*n31*n44 + n11*n33*n44),
			invDet * (n14*n32*n41 - n12*n34*n41 - n14*n31*n42 + n11*n34*n42 + n12*n31*n44 - n11*n32*n44),
			invDet * (n12*n33*n41 - n13*n32*n41 + n13*n31*n42 - n11*n33*n42 - n12*n31*n43 + n11*n32*n43),

			invDet * (n13*n24*n42 - n14*n23*n42 + n14*n22*n43 - n12*n24*n43 - n13*n22*n44 + n12*n23*n44),
			invDet * (n14*n23*n41 - n13*n24*n41 - n14*n21*n43 + n11*n24*n43 + n13*n21*n44 - n11*n23*n44),
			invDet * (n12*n24*n41 - n14*n22*n41 + n14*n21*n42 - n11*n24*n42 - n12*n21*n44 + n11*n22*n44),
			invDet * (n13*n22*n41 - n12*n23*n41 - n13*n21*n42 + n11*n23*n42 + n12*n21*n43 - n11*n22*n43),

			invDet * (n14*n23*n32 - n13*n24*n32 - n14*n22*n33 + n12*n24*n33 + n13*n22*n34 - n12*n23*n34),
			invDet * (n13*n24*n31 - n14*n23*n31 + n14*n21*n33 - n11*n24*n33 - n13*n21*n34 + n11*n23*n34),
			invDet * (n14*n22*n31 - n12*n24*n31 - n14*n21*n32 + n11*n24*n32 + n12*n21*n34 - n11*n22*n34),
			invDet * (n12*n23*n31 - n13*n22*n31 + n13*n21*n32 - n11*n23*n32 - n12*n21*n33 + n11*n22*n33)
		);
	}

	// Загрузка правосторонней матрицы камеры (аналогично gluLookAt)
	void LoadLookAtRH(T eyeX, T eyeY, T eyeZ, T atX, T atY, T atZ, T upX, T upY, T upZ) noexcept
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
	void LookAtRH(T eyeX, T eyeY, T eyeZ, T atX, T atY, T atZ, T upX, T upY, T upZ) noexcept
	{
		CMatrix4 lookAtMatrix;
		lookAtMatrix.LoadLookAtRH(eyeX, eyeY, eyeZ, atX, atY, atZ, upX, upY, upZ);
		*this *= lookAtMatrix;
	}

	// Очистка столбца
	void ClearColumn(size_t i) noexcept
	{
		assert(i < 4);
		mat[i][0] = 0;
		mat[i][1] = 0;
		mat[i][2] = 0;
		mat[i][3] = 0;
	}

	// Очистка строки
	void ClearRow(size_t i) noexcept
	{
		assert(i < 4);
		mat[0][i] = 0;
		mat[1][i] = 0;
		mat[2][i] = 0;
		mat[3][i] = 0;
	}

	// Загрузка единичной матрицы
	void LoadIdentity() noexcept
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

	void Normalize() noexcept
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
		SetRow(0, xAxis);
		SetRow(1, yAxis);
		SetRow(2, zAxis);
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
