#pragma once

class CBufferObjectBase
{
public:
	// Генерируем имя для буферного объекта
	void Create()
	{
		if (m_bufferId == 0)
		{
			glGenBuffers(1, &m_bufferId);
		}
	}

	// Возвращаем идентификатор буферного объекта
	operator GLuint()const
	{
		return m_bufferId;
	}

	// "Отсоединяем" буферный объект от текущего экземпляра класса
	GLuint Detach()
	{
		GLuint bufferId = m_bufferId;
		m_bufferId = 0;
		return bufferId;
	}

	// Удаляем буферный объект
	void Delete()
	{
		if (m_bufferId != 0)
		{
			glDeleteBuffers(1, &m_bufferId);
			m_bufferId = 0;
		}
	}

protected:
	CBufferObjectBase(GLuint bufferId)
		:m_bufferId(bufferId)
	{
	}

	// Делаем буферный объект активным
	void BindTo(GLenum target)const
	{
		glBindBuffer(target, m_bufferId);
	}

	// Задаем идентификатор буферного объекта
	void SetBuffer(GLuint bufferId)
	{
		m_bufferId = bufferId;
	}
private:
	GLuint m_bufferId;

	CBufferObjectBase(CBufferObjectBase const&);
	CBufferObjectBase& operator=(CBufferObjectBase const&);
};

template <bool t_managed, GLenum target>
class CBufferObjectImpl : public CBufferObjectBase
{
public:
	CBufferObjectImpl<t_managed, target>(GLuint bufferId = 0)
		:CBufferObjectBase(bufferId)
	{
	}

	~CBufferObjectImpl()
	{
		if (t_managed && (*this != 0))
		{
			Delete();
		}
	}

	void Bind()const
	{
		BindTo(target);
	}

	void Attach(GLuint bufferId)
	{
		if (t_managed && (*this != 0) && (bufferId != *this))
		{
			Delete();
		}
		SetBuffer(bufferId);
	}

	static GLvoid* MapBuffer(GLenum access)
	{
		return glMapBuffer(target, access);
	}

	static GLboolean UnmapBuffer()
	{
		return glUnmapBuffer(target);
	}

	static void BufferData(GLsizeiptr size, GLvoid const* data, GLenum usage)
	{
		glBufferData(target, size, data, usage);
	}

	static void BufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data)
	{
		glBufferSubData(target, offset, size, data)
	}

	CBufferObjectImpl<t_managed, target> & operator=(GLuint bufferId)
	{
		Attach(bufferId);
		return *this;
	}
};


typedef CBufferObjectImpl<true, GL_ARRAY_BUFFER> CArrayBuffer;
typedef CBufferObjectImpl<false, GL_ARRAY_BUFFER> CArrayBufferHandle;
typedef CBufferObjectImpl<true, GL_ELEMENT_ARRAY_BUFFER> CElementArrayBuffer;
typedef CBufferObjectImpl<false, GL_ELEMENT_ARRAY_BUFFER> CElementArrayBufferHandle;
