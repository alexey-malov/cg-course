//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stdafx.h"
#include "Utils.h"
#include <vector>
#include "TextureLoader.h"
#include "GdiPlusInitializer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// вычисляем нормаль к треугольнику
void CalculateTriangleNormal(Vector3d const& a, Vector3d const& b, Vector3d const& c, Vector3d* normal)
{
	/*
		  A
		 / \
		/   \
	   /     \
	  /       \
	 /         \
	B-----------C
	Нормаль к треугольнику ABC - векторное произведение BC(x1, y1, z1) x BA(x2, y2, z2):
	|i  j  k |   |y1*z2 - y2*z1|
	|x1 y1 z1| = |z1*x2 - z2*x1|
	|x2 y2 z2|   |x1*y2 - x2*y1|
	где:
		x1 = (c-b).x
		y1 = (c-b).y
		z1 = (c-b).z
		x2 = (a-b).x
		y2 = (a-b).y
		z2 = (a-b).z
	*/
	// вычисляем координаты вектора BC
	Vector3d bc = c - b;
	// вычисляем координаты вектора BA
	Vector3d ba = a - b;
	// вычисляем их векторное произведение - нормаль к треугольнику
	normal->CrossProduct(bc, ba);
	// нормируем результат, т.к. вектор нормали должен быть единичной длины
	normal->Normalize();
}

bool IsPowerOfTwo(int n)
{
	return (n & (n - 1)) == 0;
}

