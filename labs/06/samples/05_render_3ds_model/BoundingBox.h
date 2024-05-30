#pragma once

#include "Vector3.h"

/*
Класс "ограничивающий блок"
*/
class CBoundingBox
{
public:
	// Создаем пустой блок
	CBoundingBox();

	// Создаем блок по двум точкам, задающим минимальные
	// и максимальные координаты
	CBoundingBox(CVector3f const& minCoord, CVector3f const& maxCoord);

	// Сообщаем о том, является ли блок пустым
	bool IsEmpty()const;

	// Возвращаем результат объекдинения двух ограничивающих блоков
	CBoundingBox const Union(CBoundingBox const& other)const;

	// Возвращаем минимальные и максимальные координаты
	CVector3f const & GetMinCoord()const;
	CVector3f const & GetMaxCoord()const;

	// Возвращаем размер блока: (min - max) 
	CVector3f const GetSize()const;

	// Возвращаем координаты центра блока
	CVector3f const GetCenter()const;
private:
	bool	m_isEmpty;
	CVector3f m_minCoord;
	CVector3f m_maxCoord;
};
