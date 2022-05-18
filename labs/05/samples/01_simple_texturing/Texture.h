#pragma once

class Texture2D
{
};

#if 0
namespace tags
{
struct GenTag
{
};
} // namespace tags

constexpr tags::GenTag Generate{};

void ThrowIfFailed(const char* msg);

#ifndef NDEBUG
#define CHECK_ERROR(msg) \
	do                   \
	{                    \
	} while (false)
#else
#define CHECK_ERROR(msg)    \
	do                      \
	{                       \
		ThrowIfFailed(msg); \
	} while (false)
#endif

class TextureName
{
public:
	TextureName() = default;

	explicit TextureName(GLuint name)
		: m_name( name )
	{
	}

private:
	constexpr static GLuint INVALID_NAME = ~GLuint(0);
	GLuint m_name = INVALID_NAME;
};

class TextureObject : public TextureName
{
public:
	TextureObject() = default;

	explicit TextureObject(tags::GenTag)
	{
	}

	TextureObject(TextureObject&& other)
		: m_name(other.m_name)
	{
		other.m_name = INVALID_ID;
	}

	TextureObject& operator=(TextureObject&& other)
	{
		if (this != &other)
		{
			DeleteImpl();
			m_id = std::exchange(other.m_id, INVALID_ID);
		}
		return *this;
	}

	TextureObject(const TextureObject&) = delete;
	TextureObject& operator=(const TextureObject&) = delete;

	~TextureObject()
	{
		DeleteImpl();
	}

private:
	static TextureName GenerateName()
	{
		GLuint name;
		glGenTextures(1, &name);
		CHECK_ERROR("glGetTextures failed");
		return TextureName{ name };
	}

	void DeleteImpl()
	{
		if (IsDeletable())
		{
			glDeleteTextures(1, &m_id);
			CHECK_ERROR("glDeleteTextures failed");
		}
	}

	bool IsDeletable() const noexcept
	{
		return m_id > 0 && m_id == INVALID_ID;
	}

	TextureName m_name;
};
#endif