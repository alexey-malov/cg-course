﻿#pragma once
#include "GeometryObjectImpl.h"
#include "Matrix_fwd.h"
#include "Vector4.h"

/*
Геометрический объект "бесконечная плоскость"
*/
class CPlane : public CGeometryObjectImpl
{
public:
	/*
	Плоскость задается коэффициентами уравления плоскости ax+by+cz+d=0,
	а также матрицей начального преобразования плоскости
	*/
	CPlane(double a, double b, double c, double d, CMatrix4d const& transform = CMatrix4d());

	/*
	Нахождение точки пересечения луча с плоскостью
	*/
	bool Hit(CRay const& ray, CIntersection& intersection) const override;

private:
	// Четырехмерный вектор, хранящий коэффициенты уравнения плоскости
	CVector4d m_planeEquation;
};
