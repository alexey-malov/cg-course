#pragma once

/*
Базовый класс для работы с шейдерный объектом OpenGL
*/
class CShaderBase
{
	// Защищенные конструктор и деструктор сделают
	// возможным использование данного класса только
	// для его потомков
protected:
	CShaderBase(GLuint shader = 0)
		: m_shader(shader)
	{
	}

	// Объявляем деструктор защищенным, т.к. в противном
	// случае компилятор сделал бы его публичным
	// Объявлять деструктор виртуальным нет необходимости
	// т.к. деструктор защищен от вызова извне, а значит
	// удалить CShaderBase напрямую не получится (только удалив
	// его наследника)
	~CShaderBase()
	{
		// Данный деструктор не делает ничего
		// Освобождением ресурсов будет заниматься
		// класс-потомок
	}

public:
	// Делаем недоступными конструктор копирования
	// и оператор присваивания
	CShaderBase(CShaderBase const&) = delete;
	CShaderBase& operator=(CShaderBase const&) = delete;

	// Задаем исходный код шейдера
	void SetSource(GLsizei count, const GLchar** strings, const GLint* lengths)
	{
		assert(m_shader != 0);

		glShaderSource(m_shader, count, strings, lengths);
	}

	// Задаем исходный код шейдера (в одной строке)
	void SetSource(const GLchar* source, GLint length)
	{
		const GLchar** ppSource = &source;
		SetSource(1, ppSource, &length);
	}

	// Задаем исходный код шейдера (одна ASCIIZ-строка)
	void SetSource(const GLchar* source)
	{
		const GLchar** ppSource = &source;
		SetSource(1, ppSource, NULL);
	}

	// Получаем значение некоторого параметра шейдера
	void GetParameter(GLenum pname, GLint* param) const
	{
		assert(m_shader != 0);
		glGetShaderiv(m_shader, pname, param);
	}

	// Упрощенный вариант предыдущего метода
	GLint GetParameter(GLenum pname) const
	{
		GLint value;
		GetParameter(pname, &value);
		return value;
	}

	// Получаем информационный лог от шейдера
	void GetInfoLog(GLsizei bufSize, GLsizei* length, GLchar* infoLog) const
	{
		assert(m_shader != 0);
		glGetShaderInfoLog(m_shader, bufSize, length, infoLog);
	}

	// Получаем информационный лог от шейдера
	// (упрощенный вариант)
	std::string GetInfoLog() const
	{
		GLint length = GetParameter(GL_INFO_LOG_LENGTH);
		if (length > 0)
		{
			std::vector<char> buffer(length);
			GetInfoLog(length, &length, &buffer[0]);
			std::string log(&buffer[0]);
			return log;
		}
		else
		{
			return std::string();
		}
	}

	// Получаем идентификатор шейдерного объекта
	operator GLuint() const
	{
		return m_shader;
	}

	// Получаем идентификатор шейдерного объекта (альтернативный вариант)
	GLuint Get() const
	{
		return m_shader;
	}

	// Компилируем ешйдер
	void Compile()
	{
		assert(m_shader != 0);
		glCompileShader(m_shader);
	}

	// Присоединяем шейдерный объект к классу и возвращаем
	// дескриптор ранее присоединенного
	GLuint Attach(GLuint shader)
	{
		GLuint tmp = m_shader;
		m_shader = shader;
		return tmp;
	}

	// Отсоединяем шейдерный объект
	GLuint Detach()
	{
		return Attach(0);
	}

	// Удаляем шейдерный объект
	void Delete()
	{
		assert(m_shader != 0);
		if (m_shader != 0)
		{
			glDeleteShader(Detach());
			m_shader = 0;
		}
	}

private:
	GLuint m_shader;
};

/*
Шаблонная реализация класса "Шейдерный объект OpenGL"
Параметр t_managed определяет, будет ли время жизни
шейдерного объекта управляться классом CShaderImpl или нет
*/
template <bool t_managed>
class CShaderImpl : public CShaderBase
{
public:
	CShaderImpl(GLuint shader = 0)
		: CShaderBase(shader)
	{
	}

	// Оператор присваивания выполняет
	CShaderImpl& operator=(GLuint shader)
	{
		// Удаляем текущий шейдерный объект только если:
		if (
			t_managed && // Класс управляет шейдерным объектом
			(Get() != 0) && // Задан текущий шейдерный объект
			(Get() != shader) // И он не совпадает с переданным
		)
		{
			Delete();
		}
		Attach(shader);
		return *this;
	}

	// Создаем шейдер заданного типа
	GLuint Create(GLenum type)
	{
		// Удаляем старый шейдерный объект в случае необходимости
		// (если он задан и класс управляет шейдером)
		if ((Get() != 0) && t_managed)
		{
			Delete();
		}
		GLuint shader = glCreateShader(type);
		Attach(shader);
		return shader;
	}

	// Деструктор удалит текущий шейдерный объект (если класс управляет
	// шейдерным объектом)
	~CShaderImpl(void)
	{
		if (t_managed && (Get() != 0))
		{
			Delete();
		}
	}
};

// Тип "Шейдерный объект"
// (с автоматическим управлением времем жизни
// шейдерного объекта OpenGL)
typedef CShaderImpl<true> CShader;
// Тип "Дескриптор шейдерного объекта"
// (без автоматического управления временем
// жизни шейдерного объекта OpenGL)
typedef CShaderImpl<false> CShaderHandle;

/*
Класс "Базовый программный объект OpenGL"
*/
class CProgramBase
{
protected:
	CProgramBase(GLuint program = 0)
		: m_program(program)
	{
	}

	// Объявляем деструктор защищенным, чтобы нельзя было удалить
	// экземпляр класса через CPrograBase
	~CProgramBase()
	{
	}

public:
	CProgramBase(CProgramBase const&) = delete;
	CProgramBase& operator=(CProgramBase const&) = delete;

	// Получаем целочисленный идентификатор программного объекта
	operator GLuint() const
	{
		return m_program;
	}

	// Получае целочисленный идентификатор программного объекта
	GLuint Get() const
	{
		return m_program;
	}

	// Удаляем программный объект
	void Delete()
	{
		if (m_program != 0)
		{
			glDeleteProgram(m_program);
			m_program = 0;
		}
	}

	// Присоединям к объекту класса дескриптор программного объекта,
	// возвращая ранее существующий
	GLuint Attach(GLuint program)
	{
		GLuint tmp = m_program;
		m_program = program;
		return tmp;
	}

	/*
	Присоединяем к программе шейдер с указанным идентификатором
	*/
	void AttachShader(GLuint shader)
	{
		assert(m_program != 0);
		glAttachShader(m_program, shader);
	}

	// Отсоединяем шейдер с указанным идентификатором
	void DetachShader(GLuint shader)
	{
		assert(m_program != 0);
		glDetachShader(m_program, shader);
	}

	// Выполняем компоновку программы
	void Link() const
	{
		assert(m_program);
		glLinkProgram(m_program);
	}

	// Проверяем, возможна ли корректная работа шейдерной программы
	// с текущим состоянием OpenGL
	void Validate() const
	{
		assert(m_program);
		glValidateProgram(m_program);
	}

	// Получаем информацию о программе в текстовом виде
	void GetInfoLog(GLsizei bufSize, GLsizei* length, GLchar* infoLog) const
	{
		assert(m_program != 0);
		glGetProgramInfoLog(m_program, bufSize, length, infoLog);
	}

	// Получаем информацию о программе в текстовом виде
	// (упрощенный вариант)
	std::string GetInfoLog() const
	{
		GLint length = GetParameter(GL_INFO_LOG_LENGTH);
		if (length > 0)
		{
			std::vector<char> buffer(length);
			GetInfoLog(length, &length, &buffer[0]);
			std::string log(&buffer[0], static_cast<size_t>(length) - 1);
			return log;
		}
		else
		{
			return std::string();
		}
	}

	// Задаем параметр программного объекта
	void GetParameter(GLenum pname, GLint* param) const
	{
		assert(m_program != 0);
		glGetProgramiv(m_program, pname, param);
	}

	// Получаем расположение uniform-переменной с
	// указанным именем
	GLint GetUniformLocation(const GLchar* name) const
	{
		assert(m_program);
		return glGetUniformLocation(m_program, name);
	}

	// Получаем расположение атрибутивной переменной
	// с заданным именем.
	GLint GetAttribLocation(const GLchar* name) const
	{
		assert(m_program);
		return glGetAttribLocation(m_program, name);
	}

	// Возвращаем количество активных uniform-переменных
	GLuint GetActiveUniforms() const
	{
		return GetParameter(GL_ACTIVE_UNIFORMS);
	}

	// Возвращаем количество активных attribute-переменных
	GLuint GetActiveAttributes() const
	{
		return GetParameter(GL_ACTIVE_ATTRIBUTES);
	}

	// Возвращаем максимальную длину для хранения имени
	// uniform-переменной программы
	GLuint GetActiveUniformMaxLength() const
	{
		return GetParameter(GL_ACTIVE_UNIFORM_MAX_LENGTH);
	}

	// Получаем информацию об имени, типе и размере uniform-переменной
	void GetActiveUniform(
		GLuint index, GLsizei maxLength,
		GLsizei* length, GLint* size,
		GLenum* type, GLchar* name) const
	{
		assert(m_program);
		assert(index < GetActiveUniforms());
		glGetActiveUniform(m_program, index, maxLength, length, size, type, name);
	}

	// Получаем информацию об имени, типе и размере uniform-переменной
	// более простым способом
	std::string GetActiveUniform(GLuint index, GLint* size, GLenum* type) const
	{
		// Узнаем размер буфера для хранения имени
		GLuint bufferLength = GetActiveUniformMaxLength();
		if (!bufferLength)
		{
			// Нет активных uniform переменных
			return std::string();
		}
		std::vector<char> buffer(bufferLength);
		GLsizei nameLength = 0;

		// Получаем имя, тип и размер переменной
		GetActiveUniform(
			index, bufferLength,
			&nameLength, size,
			type, &buffer[0]);

		// Переводим их в строковое представление
		return std::string(&buffer[0], &buffer[nameLength]);
	}

	// Возвращаем максимальную длину для хранения имени
	// attribute-переменной программы
	GLuint GetActiveAttributeMaxLength() const
	{
		return GetParameter(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
	}

	// Получаем информацию об имени, типе, длине и размере активной
	// attribute-переменной
	void GetActiveAttrib(GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLchar* name) const
	{
		assert(m_program);
		assert(index < GetActiveAttributes());
		glGetActiveAttrib(m_program, index, maxLength, length, size, type, name);
	}

	// Получаем информацию об имени, размере и типе активной
	// attribute-переменной более простым способом
	std::string GetActiveAttrib(GLuint index, GLint* size, GLenum* type) const
	{
		GLuint bufferLength = GetActiveAttributeMaxLength();
		if (!bufferLength)
		{
			// Нет активных атрибутивных переменных
			return std::string();
		}
		std::vector<char> buffer(bufferLength);
		GLsizei nameLength = 0;

		// Получаем имя, тип и размер переменной
		GetActiveAttrib(index, bufferLength, &nameLength, size, type, &buffer[0]);

		// Переводим их в строковое представление
		return std::string(&buffer[0], &buffer[nameLength]);
	}

	// Получаем параметр программного объекта
	GLint GetParameter(GLenum pname) const
	{
		GLint value = 0;
		GetParameter(pname, &value);
		return value;
	}

	// Задаем параметр программного объекта
	void SetParameter(GLenum pname, GLint value)
	{
		assert(m_program);
		glProgramParameteri(m_program, pname, value);
	}

private:
	GLuint m_program;
};

/*
Управляемая либо неуправляемая реализация программного объекта
*/
template <bool t_managed>
class CProgramImpl : public CProgramBase
{
public:
	CProgramImpl(GLuint program = 0)
		: CProgramBase(program)
	{
	}

	// Создаем программный объект и возвращаем его идентификатор
	GLuint Create()
	{
		if (t_managed && (Get() != 0))
		{
			Delete();
		}
		GLuint program = glCreateProgram();
		Attach(program);
		return program;
	}

	// Выполняем замену текущего программного объекта
	CProgramImpl& operator=(GLuint program)
	{
		// Удаляем ранее присоединенную программу
		if (t_managed && (Get() != 0) && (Get() != program))
		{
			Delete();
		}

		Attach(program);

		return *this;
	}

	// Деструктор программного объекта
	~CProgramImpl()
	{
		// удаляем объект OpenGL (если он управляется классом)
		if (t_managed && Get() != 0)
		{
			Delete();
		}
	}
};

// Тип "Программа"
// (с автоматическим управлением времем жизни
// программного объекта OpenGL)
typedef CProgramImpl<true> CProgram;

// Тип "Дескриптор программы"
// (без автоматического управления временем жизни
// программного объекта OpenGL)
typedef CProgramImpl<false> CProgramHandle;
