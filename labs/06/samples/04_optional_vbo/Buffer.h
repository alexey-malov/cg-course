#pragma once

#include "BufferObject.h"

/************************************************************************/
/* ���������� ������ "�����", ��������������� �������� ������           */
/* � ������������ �������� ������� � ������ �� ��������� OpenGL         */
/************************************************************************/
template <GLenum target>
class CBufferImpl
{
public:
	// �������� useVBO �������� ���������� ������� �� �������������
	// Buffer object ��� �������� ������
	CBufferImpl(bool useVBO = true)
		:m_hasData(false)
		,m_useVBO(useVBO)
		,m_created(false)
	{
	}

	// ������� ������ ��� �������� ������, �� ����������� ���������
	// �������� ������
	void Create()
	{
		if (!m_created) // ������� ����� ������ ��� ������ ������ Create()
		{
			// ��������� �� ������� ������� � ������������� ����������?
			m_useVBO = m_useVBO && (GLEW_ARB_vertex_buffer_object == GL_TRUE);

			if (m_useVBO && (m_buffer == 0))
			{
				// ���� ���������, �������������� Buffer Object
				m_buffer.Create();
			}
			m_created = true;
		}
	}

	// ����������� ����� ��� ������������� �� ���������� ������
	void Bind()const
	{
		if (!m_created)
		{
			throw std::logic_error("The buffer has not been created");
		}

		// ���� ������������ �������� ������, ��������� ��� ��������
		if (m_useVBO)
		{
			m_buffer.Bind();
		}
		else if (GLEW_ARB_vertex_buffer_object)
		{
			// ����� � ��������� �������� � ��������� �������
			glBindBuffer(target, 0);
		}

		// ��������� ��������� �� ������� �������� ������ (�������� this)
		// ������ �������� �������� ������ � ���, ��� �������� ����� Bind
		// ��� ��� (�� ������ UpdateCurrentBufferPointer) �� �����
		UpdateCurrentBufferPointer(false, this);
	}

	// ��������� ������� �� ������������� ��������� �������
	static void Unbind()
	{
		if (GLEW_ARB_vertex_buffer_object)
		{
			glBindBuffer(target, 0);
		}

		UpdateCurrentBufferPointer(false, NULL);
	}

	// ������ ���������� ������ (��������� ��� � ������� glBufferData)
	void BufferData(GLsizeiptr size, GLvoid const* data, GLenum usage)
	{
		// ��������� ��������� �� ������� ����� �, ��������,
		// �������� Bind()
		UpdateCurrentBufferPointer(true, this);

		if (m_useVBO)
		{
			// ���� ������������ buffer object, ������� ������ � ����
			m_buffer.BufferData(size, data, usage);
		}
		else
		{
			// ���� buffer object �� ������������, �� ������� ������ � ������
			if (data != NULL)
			{
				GLubyte const * pDataStart = reinterpret_cast<GLubyte const*>(data);
				GLubyte const * pDataEnd = pDataStart + size;
				m_data.assign(pDataStart, pDataEnd);
			}
			else
			{
				// ���� � �������� ������ ������ �������� NULL, ������� ������
				// ������ ������ std::vector.clear() ���������� ����� 
				// � ������ ��������, �.�. std::vector.clear() �� �����������
				// ������, � ���� �������� ���������� ��� ���������
				m_data.clear();
			}
		}
		
		m_hasData = (data != NULL);
	}

	// ���������� ����� ������ ������
	GLvoid const* GetBufferPointer()const
	{
		// �������� ������ ������ ���� �������� �������
		if (!m_hasData)
		{
			throw std::logic_error("The buffer contains no data");
		}

		// ��������� ��������� �� ������� ����� �, ��������,
		// �������� Bind()
		UpdateCurrentBufferPointer(true, this);

		if (m_useVBO)
		{
			// ���� ������������ Vertex Buffer Object, �� ���������� NULL
			return NULL;
		}
		else
		{
			// � ��������� ������ - ����� ������� �������� �������
			return &m_data[0];
		}
	}

	~CBufferImpl()
	{
		// ��� ����������� ������ �������� ������� ��������� �� 
		// ������� ����������� ����� ����� ����� ����������, 
		// ��� ����� ������� �������� � ���������� ������� ������ 
		// UpdateCurrentBufferPointer (����� Bind() ������ �� ���� �� �����,
		// ����� ��� �����).
		// ������� �� ����� ������� �������� ��������� �� ������� �����
		// ��� ����������� ����������
		UpdateCurrentBufferPointer(false, NULL);
	}

private:
	// ��������� ��������� �� ������� ����� (����������� ���������� ������)
	// � ������, ���� ��������� ����� ������, �������� Bind (���� �����)
	// � ���� ������� ������ ����� ����� �� ������ ���������� ������������� 
	// ������ Bind(), ������� ����� ������ �������� ��� ����
	static void UpdateCurrentBufferPointer(bool callBind, CBufferImpl const* thisPtr)
	{
		// ������������� ������ ����������� ���������� ���������� ���� ���
		// ��� ������ ������ ������ UpdateCurrentBufferPointer()
		static const CBufferImpl * pCurrentBuffer = NULL;

		// ���� ����� ����������� � ������� ������ �� ���������
		if (pCurrentBuffer != thisPtr)
		{
			// ��������� ��������� �� ������� �����
			pCurrentBuffer = thisPtr;

			// � � ������ ������������� �������� ����� Bind
			if (callBind && (thisPtr != NULL))
			{
				thisPtr->Bind();
			}
		}
	}


private:
	// ��������� ����������� � ������������ ����������� ������
	CBufferImpl(CBufferImpl const&);
	CBufferImpl& operator=(CBufferImpl const&);

	// �������� ������ ��� �������� ������
	CBufferObjectImpl<true, target> m_buffer;
	// ������� ������ ��� �������� ������. �����������, ���� ����������
	// �� ������������ �������� �������
	std::vector<BYTE>	m_data;

	bool m_hasData;	// ������������� � ������� ������
	bool m_useVBO;	// ������������ �� ������������� �������� ��������
	bool m_created; // ������������� � ���, ��� ����� ��� ������	
};

typedef CBufferImpl<GL_ARRAY_BUFFER> CVertexBuffer;
typedef CBufferImpl<GL_ELEMENT_ARRAY_BUFFER> CIndexBuffer;
