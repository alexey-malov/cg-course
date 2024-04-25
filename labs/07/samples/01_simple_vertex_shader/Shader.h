#pragma once

class Shader
{
public:
	explicit Shader(GLenum type)
		: m_shader{ glCreateShader(type) }
	{
		if (!m_shader)
		{
			throw std::runtime_error("Failed to create shader");
		}
	}
	Shader(Shader&& other) noexcept
		: m_shader(other.m_shader)
	{
		other.m_shader = 0;
	}
	Shader& operator=(Shader&& rhs) noexcept
	{
		std::swap(m_shader, rhs.m_shader);
		return *this;
	}
	~Shader()
	{
		glDeleteShader(m_shader);
	}

private:
	GLuint m_shader = 0u;
};

static_assert(!std::is_copy_constructible_v<Shader> && !std::is_copy_assignable_v<Shader>);