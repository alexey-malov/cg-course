#pragma once
#include "IGeometryObject.h"
#include "Matrix4.h"

/*
Реализация функционала геометрических объектов, не зависящего от типа объекта
*/
class CGeometryObjectImpl : public IGeometryObject
{
public:
	/*
	Конструктор, принимающий матрицу, задающую трансформацию объекта
	*/
	CGeometryObjectImpl(CMatrix4d const& transform = CMatrix4d())
	{
		SetTransform(transform);
	}

	/*
	Возвращает матрицу трансформации объекта
	*/
	CMatrix4d const& GetTransform()const
	{
		return m_transform;
	}

	/*
	Задает матрицу трансформации объекта
	*/
	void SetTransform(CMatrix4d const& transform)
	{
		m_transform = transform;

		// Также вычисляем матрицу обратного преобразования
		m_invTransform = transform.GetInverseMatrix();

		// Матрица нормали - обратная транспонированная матрица моделирования-вида
		m_normalMatrix.SetColumn(0, m_invTransform.GetRow(0));
		m_normalMatrix.SetColumn(1, m_invTransform.GetRow(1));
		m_normalMatrix.SetColumn(2, m_invTransform.GetRow(2));
		
	}

	/*
	Возвращает матрицу, обратную матрице трансформации.
	Как правило, гораздо проще применить обратную трансформацию к лучу и найти
	точку его пересечения с базовой формой геометрического объекта, нежели выполнять
	прямую трансформацию объекта и искать пересечени луча с ним
	*/
	CMatrix4d const& GetInverseTransform()const
	{
		return m_invTransform;
	}

	// Матрица нормали
	CMatrix3d const& GetNormalMatrix()const
	{
		return m_normalMatrix;
	}
private:
	// Матрица трансформации объекта
	CMatrix4d m_transform;

	// Матрица нормалей
	CMatrix3d m_normalMatrix;

	// Обратная матрица
	CMatrix4d m_invTransform;
};
