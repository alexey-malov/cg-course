#include "pch.h"
#include "Material.h"

CMaterial::CMaterial()
{
	SetDiffuse(1, 1, 1);
	SetAmbient(0.2f, 0.2f, 0.2f);
	SetSpecular(1, 1, 1);
	SetShininess(0);
}

void CMaterial::SetDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_diffuse[0] = r;
	m_diffuse[1] = g;
	m_diffuse[2] = b;
	m_diffuse[3] = a;
}

void CMaterial::SetAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_ambient[0] = r;
	m_ambient[1] = g;
	m_ambient[2] = b;
	m_diffuse[3] = a;
}

void CMaterial::SetSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_specular[0] = r;
	m_specular[1] = g;
	m_specular[2] = b;
	m_specular[3] = a;
}

void CMaterial::SetShininess(GLfloat shininess)
{
	m_shininess = shininess;
}

void CMaterial::Activate(GLenum side) const
{
	glMaterialfv(side, GL_DIFFUSE, m_diffuse);
	glMaterialfv(side, GL_AMBIENT, m_ambient);
	glMaterialfv(side, GL_SPECULAR, m_specular);
	glMaterialf(side, GL_SHININESS, m_shininess);
}
