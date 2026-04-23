#pragma once

#include "BufferObject.h"

/************************************************************************/
/* Реализация класса "Буфер", инкапсулирующая хранение данных           */
/* и использующая буферные объекты в случае их поддержки OpenGL         */
/************************************************************************/
template <GLenum target>
class CBufferImpl
{
public:
	// Параметр useVBO является пожеланием клиента об использовании
	// Buffer object для хранения данных
	CBufferImpl(bool useVBO = true)
		:m_hasData(false)
		, m_useVBO(useVBO)
		, m_created(false)
	{
	}

	// Создаем буффер для хранения данных, по возможности используя
	// буферный объект
	void Create()
	{
		if (!m_created) // создаем буфер только при первом вызове Create()
		{
			// Совпадает ли желание клиента с возможностями реализации?
			m_useVBO = m_useVBO && (GLEW_ARB_vertex_buffer_object == GL_TRUE);

			if (m_useVBO && (m_buffer == 0))
			{
				// Если совпадает, инициализируем Buffer Object
				m_buffer.Create();
			}
			m_created = true;
		}
	}

	// Привязываем буфер для использования по назначению класса
	void Bind()const
	{
		if (!m_created)
		{
			throw std::logic_error("The buffer has not been created");
		}

		// Если используется буферный объект, выполняем его привязку
		if (m_useVBO)
		{
			m_buffer.Bind();
		}
		else if (GLEW_ARB_vertex_buffer_object)
		{
			// иначе – отключаем привязку к буферному объекту
			glBindBuffer(target, 0);
		}

		// Обновляем указатель на текущий буферный объект (параметр this)
		// Первый параметр сообщает методу о том, что вызывать метод Bind
		// еще раз (из метода UpdateCurrentBufferPointer) не нужно
		UpdateCurrentBufferPointer(false, this);
	}

	// Выполняем отвязку от использования буферного объекта
	static void Unbind()
	{
		if (GLEW_ARB_vertex_buffer_object)
		{
			glBindBuffer(target, 0);
		}

		UpdateCurrentBufferPointer(false, NULL);
	}

	// Задаем содержимое буфера (параметры как в функции glBufferData)
	void BufferData(GLsizeiptr size, GLvoid const* data, GLenum usage)
	{
		// Обновляем указатель на текущий буфер и, возможно,
		// вызываем Bind()
		UpdateCurrentBufferPointer(true, this);

		if (m_useVBO)
		{
			// Если используется buffer object, заносим данные в него
			m_buffer.BufferData(size, data, usage);
		}
		else
		{
			// Если buffer object не используется, то заносим данные в массив
			if (data != NULL)
			{
				GLubyte const* pDataStart = reinterpret_cast<GLubyte const*>(data);
				GLubyte const* pDataEnd = pDataStart + size;
				m_data.assign(pDataStart, pDataEnd);
			}
			else
			{
				// Если в качестве адреса данных передали NULL, очищаем массив
				// Вместо метода std::vector.clear() используем обмен 
				// с пустым вектором, т.к. std::vector.clear() не освобождает
				// память, а лишь обнуляет количество его элементов
				m_data.swap(std::vector<GLubyte>());
			}
		}

		m_hasData = (data != NULL);
	}

	// Возвращаем адрес начала буфера
	GLvoid const* GetBufferPointer()const
	{
		// Буферный объект должен быть заполнен данными
		if (!m_hasData)
		{
			throw std::logic_error("The buffer contains no data");
		}

		// Обновляем указатель на текущий буфер и, возможно,
		// вызываем Bind()
		UpdateCurrentBufferPointer(true, this);

		if (m_useVBO)
		{
			// Если используется Vertex Buffer Object, то возвращаем NULL
			return NULL;
		}
		else
		{
			// В противном случае - адрес первого элемента массива
			return &m_data[0];
		}
	}

	~CBufferImpl()
	{
		// При уничтожении буфера хранимый классом указатель на 
		// текущий привязанный буфер может стать невалидным, 
		// что может вызвать проблемы с корректной работой метода 
		// UpdateCurrentBufferPointer (метод Bind() вызван из него не будет,
		// когда это нужно).
		// Поэтому мы явным образом обнуляем указатель на текущий буфер
		// при уничтожении экземпляра
		UpdateCurrentBufferPointer(false, NULL);
	}

private:
	// Обновляем указатель на текущий буфер (статическая переменная метода)
	// В случае, если произошла смена буфера, вызываем Bind (если нужно)
	// В ряде случаев данный метод может не суметь определить необходимость 
	// вызова Bind(), поэтому лучше всегда вызывать его явно
	static void UpdateCurrentBufferPointer(bool callBind, CBufferImpl const* thisPtr)
	{
		// инициализация данной статической переменной выполнится один раз
		// при первом вызове метода UpdateCurrentBufferPointer()
		static const CBufferImpl* pCurrentBuffer = NULL;

		// Если ранее привязанный и текущий буферы не совпадают
		if (pCurrentBuffer != thisPtr)
		{
			// обновляем указатель на текущий буфер
			pCurrentBuffer = thisPtr;

			// и в случае необходимости вызываем метод Bind
			if (callBind && (thisPtr != NULL))
			{
				thisPtr->Bind();
			}
		}
	}


private:
	// Запрещаем копирование и присваивание экземпляров класса
	CBufferImpl(CBufferImpl const&);
	CBufferImpl& operator=(CBufferImpl const&);

	// буферный объект для хранения данных
	CBufferObjectImpl<true, target> m_buffer;
	// обычный вектор для хранения данных. Применяется, если реализация
	// не поддерживает буферные объекты
	std::vector<BYTE>	m_data;

	bool m_hasData;	// сигнализируем о наличии данных
	bool m_useVBO;	// сигнализирум об использовании буферных объектов
	bool m_created; // сигнализируем о том, что буфер был создан	
};

typedef CBufferImpl<GL_ARRAY_BUFFER> CVertexBuffer;
typedef CBufferImpl<GL_ELEMENT_ARRAY_BUFFER> CIndexBuffer;
