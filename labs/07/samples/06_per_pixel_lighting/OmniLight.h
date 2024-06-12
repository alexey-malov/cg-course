#pragma once

#include "Light.h"

class COmniLight : public CLight
{
public:
	COmniLight(glm::vec3 const& position = glm::vec3(0, 0, 0));

	// Положение источника света
	void SetPosition(glm::vec3 const& position);
	// активизация источника света
	void SetLight(GLenum light)const;
	// параметры ослабления интенсивности в зависимости от расстояния
	void SetQuadraticAttenuation(GLfloat quadraticAttenuation);
	void SetLinearAttenuation(GLfloat linearAttenuation);
	void SetConstantAttenuation(GLfloat constantAttenuation);
private:
	glm::vec3 m_position;
	GLfloat m_quadraticAttenuation;
	GLfloat m_linearAttenuation;
	GLfloat m_constantAttenuation;
};
