#pragma once
#include "Program.h"

class DrawWithShadowMap
{
public:
private:
	Program m_program;
	GLuint m_depthTextureId = 0;
	GLint m_depthTextureLocation = -1;
	GLint m_lightSpaceMatrixLocation = -1;
	glm::mat4 m_lightSpaceMatrix{ 1.0f };
};
