#pragma once
#include <GL/glew.h>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <string>

class ShaderProgram
{
public:
	ShaderProgram()
		: m_program{ glCreateProgram() }
	{
		if (!m_program)
		{
			throw std::runtime_error("Failed to create program");
		}
	}

	ShaderProgram(ShaderProgram&& other) noexcept
		: m_program{std::exchange(other.m_program, 0)}
	{
	}

	~ShaderProgram()
	{
		glDeleteProgram(m_program);
	}

	ShaderProgram& operator=(ShaderProgram&& rhs) noexcept
	{
		std::swap(m_program, rhs.m_program);
		return *this;
	}

	void AttachShader(GLuint shader) noexcept
	{
		assert(m_program);
		assert(shader);
		glAttachShader(m_program, shader);
	}

	void Link() noexcept
	{
		assert(m_program);
		glLinkProgram(m_program);
	}

	void Validate() noexcept
	{
		assert(m_program);
		glValidateProgram(m_program);
	}

	bool IsValid() const noexcept
	{
		GLint status = GL_FALSE;
		GetParameter(GL_VALIDATE_STATUS, &status);
		return status == GL_TRUE;
	}

	bool IsLinked() const noexcept
	{
		GLint linkStatus = GL_FALSE;
		GetParameter(GL_LINK_STATUS, &linkStatus);
		return linkStatus == GL_TRUE;
	}

	void GetParameter(GLenum paramName, GLint* param) const noexcept
	{
		assert(m_program);
		assert(param);
		glGetProgramiv(m_program, paramName, param);
	}

	std::string GetInfoLog() const
	{
		GLint bufSize = 0;
		GetParameter(GL_INFO_LOG_LENGTH, &bufSize);
		std::string log(static_cast<size_t>(bufSize), ' ');

		GLsizei actualLength;
		glGetProgramInfoLog(m_program, static_cast<GLsizei>(bufSize), &actualLength, log.data());
		log.resize(static_cast<size_t>(actualLength));

		return log;
	}

	[[nodiscard]] GLint GetUniformLocation(const GLchar* name) const
	{
		return glGetUniformLocation(m_program, name);
	}

	operator GLuint() const noexcept
	{
		return m_program;
	}

private:
	GLuint m_program;
};
