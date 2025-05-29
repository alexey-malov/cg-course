#pragma once
#include "Program.h"

class ShadowPass
{
public:
	ShadowPass();

	void SetLightSpaceMatrix(const glm::mat4& matrix)
	{
		m_lightSpaceMatrix = matrix;
	}

	void Activate() const;

private:
	Program m_program;

	glm::mat4 m_lightSpaceMatrix{ 1.0f };
	GLint m_lightSpaceMatrixLocation = -1;
};
