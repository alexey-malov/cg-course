#include "stdafx.h"
#include "Scene.h"
#include "Ray.h"
#include "Intersection.h"
#include "IGeometryObject.h"
#include "SceneObject.h"
#include "IShader.h"
#include "ShadeContext.h"

CScene::CScene(void)
{
}

/*
Задает цвет заднего фона, который будет возвращаться при трассировке лучей,
не пересекающихся ни с одним из объектов сцены
*/
void CScene::SetBackdropColor(CVector4f const& backdropColor)
{
	m_backdropColor = backdropColor;
}

/*
Добавляем источник света к сцене
*/
void CScene::AddLightSource(ILightSourcePtr pLightSource)
{
	m_lightSources.push_back(pLightSource);
}

/*
Возврат количества источников света в сцене
*/
size_t CScene::GetLightsCount()const
{
	return m_lightSources.size();
}

/*
Возвращаем ссылку на источник света с указанным индексом
*/
ILightSource const& CScene::GetLight(size_t index)const
{
	assert(index < m_lightSources.size());

	return *m_lightSources[index];
}

/*
Добавляем в сцену объект
*/
void CScene::AddObject(CSceneObjectPtr pSceneObject)
{
	m_objects.push_back(pSceneObject);
}

CVector4f CScene::Shade(CRay const& ray)const
{
	CIntersection bestIntersection;
	CSceneObject const * pSceneObject = NULL;

	// Находим первое столкновение луча со сценой
	if (GetFirstHit(ray, bestIntersection, &pSceneObject))
	{
		// Связан ли шейдер с найденным объектом сцены?
		if (pSceneObject->HasShader())
		{
			IShader const& shader = pSceneObject->GetShader();

			// Берем информацию о первом столкновении луча с объектом
			CHitInfo const& hit = bestIntersection.GetHit(0);

			// Инициализируем контекст закрашивания для передачи его шейдеру
			// Контекст затенения хранит информацию о закрашиваемой точке, а также о сцене
			CShadeContext shadeContext(
				*this,
				hit.GetHitPoint(),
				hit.GetHitPointInObjectSpace(),
				hit.GetNormal(),
				ray.GetDirection()
				);

			// Шейдер, связанный с объектом, выполнит вычисление цвета
			return shader.Shade(shadeContext);
		}
	}

	// Точек пересечения этого луча с объектами сцены нет,
	// поэтому возвращаем цвет заднего фона
	return m_backdropColor;
}

bool CScene::GetFirstHit(CRay const& ray, CIntersection & bestIntersection, CSceneObject const ** ppIntersectionObject)const
{
	// Очищаем информацию о точках столкновения
	bestIntersection.Clear();

	const size_t n = m_objects.size();

	CIntersection intersection;

	// Пробегаем по всем объектам сцены
	for (size_t i = 0; i < n; ++i)
	{
		// Очищаем информацию о ранее найденных столкновениях
		intersection.Clear();

		CSceneObject const& sceneObject = *m_objects[i];

		// Получаем геометрический объект, связанный с объектом сцены
		IGeometryObject const& geometryObject = sceneObject.GetGeometryObject();
		
		// Ищем точки пересечения луча с данным геометрическим объектом
		if (geometryObject.Hit(ray, intersection))
		{
			// Точки пересечения есть

			// Если это первый объект, с которым было найдено пересечение,
			// либо момент столкновения луча с данным объектом произошел раньше
			// (точка пересечения с текущим объектом находится ближе к точке испускания 
			// луча, чем с ранее обработанными объектами),
			// то за лучшую точку пересечения принимается найденная точка пересечения
			if (
				(bestIntersection.GetHitsCount() == 0) || 
				(intersection.GetHit(0).GetHitTime() < bestIntersection.GetHit(0).GetHitTime())
				)
			{
				bestIntersection = intersection;
				*ppIntersectionObject = &sceneObject;
			}
		}
	}

	// Возвращаем true, если было найдено хоть одно столкновение
	return bestIntersection.GetHitsCount() > 0;
}

