#pragma once

#include "Vertex.h"
#include "BoundingBox.h"

class CMesh : public boost::noncopyable
{
public:
	CMesh(
		unsigned int vertexBufferOffset,	// �������� � ������ ������
		unsigned int indexBufferOffset,		// �������� � ������ ��������
		unsigned vertexCount,				// ���������� ������
		unsigned indexCount,				// ���������� ��������
		bool hasTextureCoords,				// ������� ���������� ���������
		CBoundingBox const& boundingBox,	// �������������� ����
		GLenum primitiveType,				// ��� ���������� �����
		GLenum indexType					// ��� �������� �����
		);

	// ���� �� � ����� ���������� ����������?
	bool HasTextureCoords()const;

	// ���������� �������� ������������ ������ ������ ������
	unsigned int GetVertexBufferOffset()const;

	// ���������� �������� ������������ ������ ������ ��������
	unsigned int GetIndexBufferOffset()const;

	// ���������� ���������� ��������
	unsigned int GetIndexCount()const;

	// ���������� ���������� ������
	unsigned int GetVertexCount()const;

	// ���������� �������������� ����
	CBoundingBox const& GetBoundingBox()const;

	// ���������� ��� ���������� (GL_TRIANGLES, GL_TRIANGLE_STRIP � �.�.)
	GLenum GetPrimitiveType()const;

	// ���������� ��� ������ ��� �������� �������� (GL_UNSIGNED_SHORT � �.�.)
	GLenum GetIndexType()const;
private:
	unsigned int m_vertexBufferOffset;
	unsigned int m_indexBufferOffset;
	unsigned int m_vertexCount;
	unsigned int m_indexCount;
	bool m_hasTexture;
	CBoundingBox const m_boundingBox;
	GLenum m_primitiveType;
	GLenum m_indexType;
};

