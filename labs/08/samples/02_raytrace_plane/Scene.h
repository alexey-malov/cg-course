#pragma once

#include "Vector4.h"
#include "SceneObject_fwd.h"

class CRay;
class CIntersection;

/************************************************************************/
/* Класс "Сцена" - хранит объекты, предоставляет методы для нахождения  */
/* пересечений луча с объектами сцены                                   */
/************************************************************************/
class CScene
{
public:
	CScene(void);

	// Задать цвет заднего фона сцены
	void SetBackdropColor(CVector4f const& backdropColor);

	/*
	Добавляем объект в сцену
	*/
	void AddObject(CSceneObjectPtr pSceneObject);

	/*
	Возвращает цвет луча, столкнувшегося с объектами сцены
	*/
	CVector4f Shade(CRay const& ray)const;

	/*
	Трассирует луч вглубь сцены и возвращает информацию о первом столкновении луча с объектам сцены
	*/
	bool GetFirstHit(CRay const& ray, CIntersection & bestIntersection, CSceneObject const ** ppIntersectionObject)const;
private:
	// Коллекция объектов сцены
	typedef std::vector<CSceneObjectPtr> SceneObjects;
	SceneObjects m_objects;

	// Цвет заднего фона сцены
	CVector4f m_backdropColor;
};
