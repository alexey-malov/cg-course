#include "stdafx.h"
#include "OmniLightSource.h"
#include "VectorMath.h"

/*
Инициализация параметров источника света (положение и трансформация)
*/
COmniLightSource::COmniLightSource(CVector3d const& position, CMatrix4d const& transform)
:CLightSourceImpl(transform)
,m_position(position)
,m_constantAttenuation(1)
,m_linearAttenuation(0)
,m_quadraticAttenuation(0)
{
	UpdatePositionInWorldSpace();
}

/*
Установка параметров ослабления света от расстояния до объекта
*/
void COmniLightSource::SetAttenuation(
	double const & constantAttenuation, 
	double const & linearAttenuation, 
	double const & quadraticAttenuation)
{
	m_constantAttenuation = constantAttenuation;
	m_linearAttenuation = linearAttenuation;
	m_quadraticAttenuation = quadraticAttenuation;
}

void COmniLightSource::SetTransform(CMatrix4d const& transform)
{
	CLightSourceImpl::SetTransform(transform);

	/*
	При установке матрицы трансформации придется вычислить заново положение источника в мировой системе координат
	*/
	UpdatePositionInWorldSpace();
}

void COmniLightSource::UpdatePositionInWorldSpace()
{
	m_positionInWorldSpace = GetTransform() * CVector4d(m_position, 1);
}

CVector3d COmniLightSource::GetDirectionFromPoint(CVector3d const& point)const
{
	/*
	Направление от точки к источнику в мировых координатах
	*/
	return GetPositionInWorldSpace() - point;
}

double COmniLightSource::GetIntensityInDirection(CVector3d const& direction)const
{
	/*
	Вычисляем расстояние (длина вектора направления)
	*/
	double distance = direction.GetLength();

	/*
	Вычисляем интенсивность света с учетом коэффициентов ослабления
	*/
	return 1.0 / (distance * distance * m_quadraticAttenuation + distance * m_linearAttenuation + m_constantAttenuation);
}

CVector3d const& COmniLightSource::GetPositionInWorldSpace()const
{
	return m_positionInWorldSpace;
}

