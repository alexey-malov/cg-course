#include "pch.h"
#include "Program.h"

Program::Program(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	m_vertexShader.SetSource(vertexShaderSource);
	m_vertexShader.Compile();
	m_fragmentShader.SetSource(fragmentShaderSource);
	m_fragmentShader.Compile();
	if (!m_vertexShader.IsCompiled())
	{
		throw std::runtime_error("Failed to compile vertex shader: " + m_vertexShader.GetInfoLog());
	}
	if (!m_fragmentShader.IsCompiled())
	{
		throw std::runtime_error("Failed to compile fragment shader: " + m_fragmentShader.GetInfoLog());
	}
	m_program.AttachShader(m_vertexShader);
	m_program.AttachShader(m_fragmentShader);
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
