#pragma once
#include <GL/glew.h>
#include <cassert>
#include <type_traits>
#include <utility>

class FrameBufferBase
{
public:
	void Delete() noexcept
	{
		if (m_frameBuffer)
		{
			glDeleteBuffers(1, &m_frameBuffer);
			m_frameBuffer = 0;
		}
	}

	GLuint Create()
	{
		assert(!m_frameBuffer);
		glGenFramebuffers(1, &m_frameBuffer);
		return m_frameBuffer;
	}

	GLuint Detach() noexcept
	{
		return std::exchange(m_frameBuffer, 0u);
	}

	void Attach(GLuint frameBuffer) noexcept
	{
		assert(!m_frameBuffer);
		m_frameBuffer = frameBuffer;
	}

	operator GLuint() const noexcept
	{
		return m_frameBuffer;
	}

	void Bind(GLenum target)
	{
		glBindFramebuffer(target, m_frameBuffer);
	}

protected:
	FrameBufferBase() = default;
	explicit FrameBufferBase(GLuint frameBuffer)
		: m_frameBuffer{ frameBuffer }
	{
	}

private:
	GLuint m_frameBuffer = 0u;
};

class FrameBufferHandle : public FrameBufferBase
{
public:
	FrameBufferHandle() = default;

	explicit FrameBufferHandle(GLuint frameBuffer) noexcept
		: FrameBufferBase(frameBuffer)
	{
	}
};

class FrameBuffer : public FrameBufferBase
{
public:
	FrameBuffer() = default;

	explicit FrameBuffer(GLuint frameBuffer) noexcept
		: FrameBufferBase(frameBuffer)
	{
	}

	FrameBuffer(FrameBuffer&& other) noexcept
		: FrameBufferBase{ other.Detach() }
	{
	}

	FrameBuffer& operator=(FrameBuffer&& other) noexcept
	{
		if (this != &other)
		{
			Delete();
			Attach(other.Detach());
		}
		return *this;
	}

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	~FrameBuffer()
	{
		Delete();
	}
};

