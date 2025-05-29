#include "pch.h"
#include "Program.h"

Program::Program(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	Shader vertexShader{ GL_VERTEX_SHADER };

	vertexShader.SetSource(vertexShaderSource);
	vertexShader.Compile();

	Shader fragmentShader{ GL_FRAGMENT_SHADER };
	fragmentShader.SetSource(fragmentShaderSource);
	fragmentShader.Compile();

	if (!vertexShader.IsCompiled())
	{
		throw std::runtime_error("Failed to compile vertex shader: " + vertexShader.GetInfoLog());
	}
	if (!fragmentShader.IsCompiled())
	{
		throw std::runtime_error("Failed to compile fragment shader: " + fragmentShader.GetInfoLog());
	}
	m_program.AttachShader(vertexShader);
	m_program.AttachShader(fragmentShader);
	m_program.Link();
	m_program.Validate();
	if (!m_program.IsLinked())
	{
		throw std::runtime_error("Failed to link program: " + m_program.GetInfoLog());
	}
	if (!m_program.IsValid())
	{
		throw std::runtime_error("Program is not valid");
	}
}
