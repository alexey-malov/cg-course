#pragma once
#include "gl.h"
#include <cassert>
#include <utility>

// Базовый класс текстур
class TextureBase
{
public:
	operator GLuint() const noexcept
	{
		return m_texture;
	}

	GLuint Create() noexcept
	{
		assert(!m_texture);
		glGenTextures(1, &m_texture);
		return m_texture;
	}

	void Delete() noexcept
	{
		if (m_texture)
		{
			glDeleteTextures(1, &m_texture);
			m_texture = 0;
		}
	}

	GLuint Detach() noexcept
	{
		return std::exchange(m_texture, 0u);
	}

	void Attach(GLuint texture) noexcept
	{
		assert(!m_texture);
		m_texture = texture;
	}

protected:
	TextureBase() = default;

	explicit TextureBase(GLuint texture)
		: m_texture{ texture }
	{
	}

private:
	GLuint m_texture = 0;
};

class TextureHandle : public TextureBase
{
public:
	TextureHandle() = default;
	explicit TextureHandle(GLuint texture)
		: TextureBase{ texture }
	{
	}
};

class Texture : public TextureBase
{
public:
	Texture() = default;

	explicit Texture(GLuint texture) noexcept
		: TextureBase{ texture }
	{
	}

	Texture(Texture&& other) noexcept
		: TextureBase{ other.Detach() }
	{
	}

	Texture& operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			Delete();
			Attach(other.Detach());
		}
		return *this;
	}

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	~Texture()
	{
		Delete();
	}
};

