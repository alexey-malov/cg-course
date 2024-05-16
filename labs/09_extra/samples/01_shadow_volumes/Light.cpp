#include "StdAfx.h"
#include "Light.h"

CLight::CLight(void)
{
	SetDiffuseIntensity(0.8, 0.8, 0.8, 1);
	SetAmbientIntensity(0.2, 0.2, 0.2, 1);
	SetSpecularIntensity(0.5, 0.5, 0.5, 1);
}

CLight::~CLight(void)
{
}

void CLight::SetLight(GLenum light)const
{
	glLightfv(light, GL_DIFFUSE, m_diffuse);
	glLightfv(light, GL_AMBIENT, m_ambient);
	glLightfv(light, GL_SPECULAR, m_specular);
}

void CLight::SetDiffuseIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_diffuse[0] = r;
	m_diffuse[1] = g;
	m_diffuse[2] = b;
	m_diffuse[3] = a;
}

void CLight::SetAmbientIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_ambient[0] = r;
	m_ambient[1] = g;
	m_ambient[2] = b;
	m_ambient[3] = a;
}

void CLight::SetSpecularIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_specular[0] = r;
	m_specular[1] = g;
	m_specular[2] = b;
	m_specular[3] = a;
}
