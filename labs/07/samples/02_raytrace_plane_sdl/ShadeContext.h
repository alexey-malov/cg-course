#pragma once

#include "Vector_fwd.h"
class CRay;
class CScene;

/*
Контекст закрашивания, используемый шейдером для вычисления цвета поверхности
Хранит информацию о координатах обрабатываемой точки, нормали и направлении луча, а также ссылку на сцену
(в дальнейшем понадобится для построения теней, а также расчета преломлений/отражений)
*/
class CShadeContext
{
public:
	/*
	Инициализирует контекст закрашивания
	*/
	CShadeContext(
		CScene const& scene,
		CVector3d const& sufracePoint,
		CVector3d const& sufracePointInObjectSpace,
		CVector3d const& surfaceNormal,	// нормаль в мировой системе координат
		CVector3d const& rayDirection	// направление трассируемого луча в мировой системе координат
		) throw()
		:m_sufracePoint(sufracePoint)
		,m_surfacePointInObjectSpace(sufracePointInObjectSpace)
		,m_surfaceNormal(surfaceNormal)
		,m_rayDirection(rayDirection)
		,m_scene(scene)
	{
	}

	/*
	Возвращает координаты точки в мировой системе координат.
	*/
	CVector3d const& GetSurfacePoint()const throw()
	{
		return m_sufracePoint;
	}

	/*
	Возвращает координаты точки в системе координат объекта
	*/
	CVector3d const& GetSurfacePointInObjectSpace()const throw()
	{
		return m_surfacePointInObjectSpace;
	}

	/*
	Возвращает нормаль к поверхности в мировой системе координат
	*/
	CVector3d const& GetSurfaceNormal()const throw()
	{
		return m_surfaceNormal;
	}

	/*
	Возвращает направление луча, попавшего в данную точку
	*/
	CVector3d const& GetRayDirection()const throw()
	{
		return m_rayDirection;
	}

	/*
	Возвращает ссылку на сцену
	*/
	CScene const& GetScene()const throw()
	{
		return m_scene;
	}

private:
	CVector3d const& m_sufracePoint;
	CVector3d const& m_surfacePointInObjectSpace;
	CVector3d const& m_surfaceNormal;
	CVector3d const& m_rayDirection;
	CScene const& m_scene;
};
