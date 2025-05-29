#pragma once
#include "Shader.h"
#include "ShaderProgram.h"

class Program
{
public:
	Program(const char* vertexShaderSource, const char* fragmentShaderSource);

	GLuint GetId() const noexcept { return m_program; }

	const ShaderProgram& GetProgram() const noexcept
	{
		return m_program;
	}

private:
	ShaderProgram m_program;
};

static_assert(std::is_move_constructible_v<Program>);
