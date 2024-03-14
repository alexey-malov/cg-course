#pragma once

// Базовый класс "Буфер кадра"
class CFrameBufferBase
{
public:
	// Создание нового буфера кадра
	void Create()
	{
		assert(!m_frameBuffer);
		glGenRenderbuffersEXT(1, &m_frameBuffer);
	}

	// Привязка буфера кадра к указанной цели
	static void Bind(GLuint target, GLuint framebuffer)
	{
		glBindFramebufferEXT(target, framebuffer);
	}

	// Привязка текущего буфера кадра
	void Bind(GLenum target = GL_FRAMEBUFFER_EXT) const
	{
		assert(m_frameBuffer != 0);
		glBindFramebufferEXT(target, m_frameBuffer);
	}

	// Присоединение буфера визуализации к буферу кадра
	static void SetRenderBuffer(
		GLenum target, GLenum attachment,
		GLenum renderbuffertarget, GLuint renderbuffer)
	{
		glFramebufferRenderbufferEXT(
			target, attachment, renderbuffertarget, renderbuffer);
	}

	// Присоединение двухмерной текстуры к буферу кадра
	static void SetTexture2D(
		GLenum target, GLenum attachment,
		GLenum textarget, GLuint texture, GLint level)
	{
		glFramebufferTexture2DEXT(
			target, attachment, textarget, texture, level);
	}

	// Проверка статуса буфера кадра
	static GLenum CheckStatus(GLenum target)
	{
		return glCheckFramebufferStatusEXT(target);
	}

	// Удаление буфера кадра
	void Delete()
	{
		assert(m_frameBuffer != 0);
		glDeleteFramebuffersEXT(1, &m_frameBuffer);
		m_frameBuffer = 0;
	}

	// Получение идентификатора буфера кадра
	GLuint Get() const
	{
		return m_frameBuffer;
	}

	// Получение идентификатора буфера кадра
	operator GLuint() const
	{
		return m_frameBuffer;
	}

protected:
	CFrameBufferBase(GLuint frameBuffer = 0)
		: m_frameBuffer(0)
	{
	}

	~CFrameBufferBase()
	{
	}

	// Задание идентификатора буфера кадра
	void SetFrameBuffer(GLuint frameBuffer)
	{
		m_frameBuffer = frameBuffer;
	}

private:
	// Запрещаем копирование и присваивание
	// экземпляров класса CFrameBufferBase
	CFrameBufferBase(CFrameBufferBase const&);
	CFrameBufferBase& operator=(CFrameBufferBase const&);

	GLuint m_frameBuffer;
};

// Управляемая или неуправляемая реализация буфера кадра
template <bool t_managed>
class CFrameBufferImpl : public CFrameBufferBase
{
public:
	CFrameBufferImpl(GLuint frameBuffer = 0)
		: CFrameBufferBase(frameBuffer)
	{
	}

	~CFrameBufferImpl()
	{
		// Удаляем буфер кадра, если класс управляет временем
		// его жизни
		if (t_managed && (Get() != 0))
		{
			Delete();
		}
	}

private:
};

// Буфер кадра (класс управляет временем жизни объекта OpenGL)
typedef CFrameBufferImpl<true> CFrameBuffer;

// Дескриптор буфера кадра (класс не управляет временем
// жизни объекта OpenGL)
typedef CFrameBufferImpl<false> CFrameBufferHandle;

// Базовый буфер физуализации
class CRenderBufferBase
{
public:
	// Создание буфера визуализации
	void Create()
	{
		assert(!m_renderBuffer);
		glGenRenderbuffersEXT(1, &m_renderBuffer);
	}

	// Привязка к текущему буферу визуализации
	void Bind(GLenum target = GL_RENDERBUFFER_EXT) const
	{
		assert(m_renderBuffer != 0);
		glBindRenderbufferEXT(target, m_renderBuffer);
	}

	// Привязка к заданному буферу визуализации
	static void Bind(GLenum target, GLenum renderBuffer)
	{
		glBindRenderbufferEXT(target, renderBuffer);
	}

	// Удаление буфера визуализации
	void Delete()
	{
		assert(m_renderBuffer != 0);
		glDeleteRenderbuffersEXT(1, &m_renderBuffer);
		m_renderBuffer = 0;
	}

	// Получение идентификатора буфера визуализации
	GLuint Get() const
	{
		return m_renderBuffer;
	}

	// Получение идентификатора буфера визуализации
	operator GLuint() const
	{
		return m_renderBuffer;
	}

	// Выделение памяти под хранение буфера кадра заданного размера
	static void SetStorage(
		GLenum target, GLenum internalformat,
		GLsizei width, GLsizei height)
	{
		glRenderbufferStorageEXT(target, internalformat, width, height);
	}

protected:
	CRenderBufferBase(GLuint renderBuffer = 0)
		: m_renderBuffer(renderBuffer)
	{
	}
	void SetRenderBuffer(GLuint renderBuffer)
	{
		m_renderBuffer = renderBuffer;
	}
	~CRenderBufferBase()
	{
	}

private:
	// Запрещаем конструктор копирования и оператор присваивания
	CRenderBufferBase(CRenderBufferBase const&);
	CRenderBufferBase& operator=(CRenderBufferBase const&);

	GLuint m_renderBuffer;
};

// Управляемая, либо неуправляемая реализация
// класса "Буфер визуализации"
template <bool t_managed>
class CRenderBufferImpl : public CRenderBufferBase
{
public:
	CRenderBufferImpl(GLuint renderBuffer = 0)
		: CRenderBufferBase(renderBuffer)
	{
	}

	~CRenderBufferImpl()
	{
		// Удаляем буфер визуализации
		if (t_managed && (Get() != 0))
		{
			Delete();
		}
	}
};

// Буфер визуализации (класс управляем временем жизни объекта OpenGL)
typedef CRenderBufferImpl<true> CRenderBuffer;

// Дескриптор буфера визуализации
// (класс не управляет временем жизни объекта OpenGL)
typedef CRenderBufferImpl<false> CRenderBufferHandle;
