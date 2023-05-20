#pragma once

#include "common_inc.h"

class CLight
{
public:
	virtual void SetLight(GLenum light) const;

	void SetDiffuseIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);
	void SetAmbientIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);
	void SetSpecularIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);

protected:
	CLight();

private:
	GLfloat m_diffuse[4];
	GLfloat m_ambient[4];
	GLfloat m_specular[4];
};
