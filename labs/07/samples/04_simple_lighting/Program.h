#pragma once
#include "Shader.h"
#include "ShaderProgram.h"

class Program
{
public:
	Program(const char* vertexShaderSource, const char* fragmentShaderSource);

	GLuint GetId() const noexcept { return m_program; }

private:
	Shader m_vertexShader{ GL_VERTEX_SHADER };
	Shader m_fragmentShader{ GL_FRAGMENT_SHADER };
	ShaderProgram m_program;
};
