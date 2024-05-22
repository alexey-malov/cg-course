#pragma once

#include "Vector3.h"

/*
Класс "Луч", характеризующийся точкой испускания и направлением
В момент времени t=0 луч находится в стартовой точке.
За промежуток времени t=1 луч проходит расстояние direction
*/
class CRay
{
public:
	CRay(CVector3d const& start, CVector3d const& direction) noexcept
		:m_start(start), m_direction(direction)
	{
	}

	// Возвращает точку на луче в указанный момент времени
	CVector3d GetPointAtTime(double time)const noexcept
	{
		return m_start + m_direction * time;
	}

	/*
	Начальная точка луча
	*/
	CVector3d const& GetStart()const noexcept
	{
		return m_start;
	}

	/*
	Направление луча
	*/
	CVector3d const& GetDirection()const noexcept
	{
		return m_direction;
	}

private:
	CVector3d m_start;
	CVector3d m_direction;
};

#include "VectorMath.h"
#include "Matrix4.h"

/*
Трансформация луча с использованием заданной матрицы
*/
inline CRay Transform(CRay const& ray, CMatrix4d const& matrix) noexcept
{
	CVector3d start = (matrix * CVector4d(ray.GetStart(), 1)).Project();
	CVector3d direction = (matrix * CVector4d(ray.GetDirection(), 0));
	return CRay(start, direction);
}