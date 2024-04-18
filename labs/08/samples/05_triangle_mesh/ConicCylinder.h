#pragma once
#include "GeometryObjectWithInitialTransformImpl.h"

/************************************************************************/
/* Геометрический объект "Конический цилиндр"                           */
/* Ось цилиндра совпадает с осью z. Основание находится в плоскости z=0 */
/* "Крышка" находится в плоскости z="высота"                            */
/************************************************************************/
class CConicCylinder :
	public CGeometryObjectWithInitialTransformImpl
{
public:
	CConicCylinder(
		double const & height = 1,		// Высота цилиндра (>=0)
		double const& baseRadius = 1,	// Радиус основания (>=0)
		double const& capRadius = 0,	// Радиус крышки (>=0)
		CMatrix4d const& tranform = CMatrix4d());

	virtual bool Hit(CRay const& ray, CIntersection & intersection)const;
private:
	double m_smallRadius;
};
