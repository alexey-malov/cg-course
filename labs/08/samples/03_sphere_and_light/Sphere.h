#pragma once
#include "GeometryObjectImpl.h"

/************************************************************************/
/* Геометрический объект "Сфера"                                        */
/************************************************************************/
class CSphere :
	public CGeometryObjectImpl
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
	Перегруженный метод получения матрицы обратной трансформации объекта
	*/
	virtual CMatrix4d const& GetInverseTransform()const;

	/*
	Пересечение луча со сферой
	*/
	virtual bool Hit(CRay const& ray, CIntersection & intersection)const;
protected:
	virtual void OnUpdateTransform();
private:

	/*
	Параметры обратной матрицы трансформации, построенной с учетом начального
	преобразования базовой сферы и матрицы трансформации сферы
	*/
	CMatrix4d m_inverseTransform;

	/*
	Начальная трансформация, выполняющая преобразование базовой сферы единичного радиуса с центром
	в начале координат в сферу заданного радиуса с центром в указанной точке
	*/
	CMatrix4d m_initialTransform;
};
