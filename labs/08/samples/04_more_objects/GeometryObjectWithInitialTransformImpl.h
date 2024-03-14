#pragma once
#include "GeometryObjectImpl.h"

class CGeometryObjectWithInitialTransformImpl :
	public CGeometryObjectImpl
{
public:

	CGeometryObjectWithInitialTransformImpl(CMatrix4d const& tranform = CMatrix4d())
		:CGeometryObjectImpl(tranform)
	{
		UpdateInverseTransform();
	}

	/*
	Перегруженный метод получения матрицы обратной трансформации объекта
	*/
	virtual CMatrix4d const& GetInverseTransform()const
	{
		// При последующих вызовах метода GetInverseTransform() будет возвращено ранее вычисленное преобразование
		return m_inverseTransform;
	}

protected:
	void SetInitialTransform(CMatrix4d const& initialTransform)
	{
		m_initialTransform = initialTransform;
		UpdateInverseTransform();
	}

	void OnUpdateTransform()
	{
		CGeometryObjectImpl::OnUpdateTransform();

		UpdateInverseTransform();
	}
private:
	void UpdateInverseTransform()
	{
		// Инвертируем матрицу начального преобразования
		CMatrix4d inverseInitialTransform = m_initialTransform.GetInverseMatrix();

		// Получаем обратную матрицу преобразования геометрического объект в целом
		CMatrix4d const& inverseGeomObjectTransform = CGeometryObjectImpl::GetInverseTransform();

		// Вычисляем результирующую матрицу преобразования, перемножив обратные матрицы в обратном порядке
		m_inverseTransform = inverseInitialTransform * inverseGeomObjectTransform;
	}
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
