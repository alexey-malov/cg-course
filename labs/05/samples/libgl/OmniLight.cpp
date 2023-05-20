#include "pch.h"
#include "OmniLight.h"

COmniLight::COmniLight(CVector3f const& position)
	: m_position(position)
	, m_quadraticAttenuation(0)
	, m_linearAttenuation(0)
	, m_constantAttenuation(1)
{
}

void COmniLight::SetLight(GLenum light) const
{
	GLfloat lightPosition[4] = {
		m_position.x,
		m_position.y,
		m_position.z,
		1
	};
	glLightfv(light, GL_POSITION, lightPosition);

	glLightf(light, GL_QUADRATIC_ATTENUATION, m_quadraticAttenuation);
	glLightf(light, GL_LINEAR_ATTENUATION, m_linearAttenuation);
	glLightf(light, GL_CONSTANT_ATTENUATION, m_constantAttenuation);

	CLight::SetLight(light);
}

void COmniLight::SetPosition(CVector3f const& position)
{
	m_position = position;
}

void COmniLight::SetQuadraticAttenuation(GLfloat quadraticAttenuation)
{
	m_quadraticAttenuation = quadraticAttenuation;
}

void COmniLight::SetLinearAttenuation(GLfloat linearAttenuation)
{
	m_linearAttenuation = linearAttenuation;
}

void COmniLight::SetConstantAttenuation(GLfloat constantAttenuation)
{
	m_constantAttenuation = constantAttenuation;
}
