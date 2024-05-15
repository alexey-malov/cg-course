#pragma once
#include <GL/glew.h>
#include <cassert>
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

private:
	GLuint m_frameBuffer = 0u;
};

using FrameBufferHandle = FrameBufferBase;

class FrameBuffer : public FrameBufferBase
{
public:
	FrameBuffer() = default;

	FrameBuffer(FrameBuffer&& other) noexcept
	{
		Attach(other.Detach());
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