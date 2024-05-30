#include "StdAfx.h"
#include "DirectLight.h"

CDirectLight::CDirectLight(CVector3f const& lightDirection)
:m_direction(lightDirection)
{
}

void CDirectLight::SetLight(GLenum light)const
{
	GLfloat lightDirection[4] = 
	{
		m_direction.x, 
		m_direction.y, 
		m_direction.z, 
		0
	};
	glLightfv(light, GL_POSITION, lightDirection);
	CLight::SetLight(light);
}

void CDirectLight::SetDirection(CVector3f const& direction)
{
	m_direction = direction;
}

