#include "pch.h"
#include "ShadowPass.h"

ShadowPass::ShadowPass()
	: m_program(
		  // Vertex shader
		  R"VS(
#version 330 core

layout(location = 0) in vec4 aPos;

uniform mat4 uLightSpaceMatrix;

void main()
{
	gl_Position = uLightSpaceMatrix * aPos;
}
)VS",
		  // Fragment shader
		  R"FS(
void main()
{
	// Do nothing with color. Just write depth
}
)FS")
{
	m_lightSpaceMatrixLocation = m_program.GetProgram().GetUniformLocation("uLightSpaceMatrix");
}

void ShadowPass::Activate() const
{
	glUseProgram(m_program.GetProgram());
	glUniformMatrix4fv(m_lightSpaceMatrixLocation,
		/* count = */ 1, /* transpose= */ GL_FALSE,
		glm::value_ptr(m_lightSpaceMatrix));
}
