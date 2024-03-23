#pragma once
#include "GeometryObjectWithInitialTransformImpl.h"

/************************************************************************/
/* Геометрический объект с начальной трансформацией "Сфера"             */
/************************************************************************/
class CSphere :
	public CGeometryObjectWithInitialTransformImpl
{
public:
	/*
	Инициализация сферы заданного радиуса и положения в пространстве
	*/
	CSphere(
		double radius = 1, 
		CVector3d const& center = CVector3d(), 
		CMatrix4d const& transform = CMatrix4d());

	/*
	Пересечение луча со сферой
	*/
	virtual bool Hit(CRay const& ray, CIntersection & intersection)const;
private:
};
