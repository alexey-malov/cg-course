#pragma once

#include "common_inc.h"

class CMaterial
{
public:
	CMaterial();

	// Коэффициент диффузного отражения
	void SetDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);

	// Коэффициент фонового отражения
	void SetAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);

	// Коэффициент зеркального отражения
	void SetSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);

	// Степень зеркального отражения (для модели Фонга)
	void SetShininess(GLfloat shininess);

	// Делаем материал активным для заданной грани объекта
	void Activate(GLenum side = GL_FRONT) const;

private:
	GLfloat m_shininess;
	GLfloat m_diffuse[4];
	GLfloat m_specular[4];
	GLfloat m_ambient[4];
};
