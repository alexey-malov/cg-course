#pragma once

#include "Vertex.h"
#include "BoundingBox.h"

class CMesh : public boost::noncopyable
{
public:
	// ���������, ����������� ��������
	struct SubMesh
	{
		// ������ �������� � ������� �������� �����
		unsigned startIndex;
		// ���������� �������� � ��������
		unsigned indexCount;
	};

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

	// ��������� ��������, �������� �������� ������ �����, ������������
	// �� ���������� �������� (��������, �� ������ ���������)
	// ������������ ������ ����������� ��������
	unsigned AddSubMesh(unsigned startIndex, unsigned subMeshIndexCount);

	// ���������� ��������
	unsigned GetSubMeshCount()const;

	// ���������� ���������� � �������� � �������� ��������
	SubMesh GetSubMesh(unsigned index)const;

	// ������ ������ ��������, ����� ������� ����� �������� materialIndex
	void SetMaterialSubMesh(int materialIndex, unsigned subMeshIndex);

	// �������� ������ ��������, ����� ������� ����� �������� materialIndex
	int GetMaterialSubMesh(int materialIndex)const;

	// �������� ���������� ����������, ������������ � �����
	unsigned GetMaterialsCount()const;

	// �������� ����� ��������� �� ��� ����������� ������ � �����
	int GetMaterial(unsigned index)const;

	// �������� ���������� ����, ��������� ��� �������� ������ �������
	unsigned GetIndexSize()const;
private:
	// ������ �������� ������ ������ �����
	std::vector<SubMesh> m_subMeshes;
	// ������ �������� ����������, ������������ ������ ������
	std::vector<int> m_materials;
	// ������, �������� ������ �������� ��� ������� ���������
	std::vector<int> m_materialSubMesh;

	// �������� � ��������� ������ � �������� ������ �����
	unsigned int m_vertexBufferOffset;
	// �������� � ��������� ������ � �������� ������ �����
	unsigned int m_indexBufferOffset;
	// ���������� ������ � �����
	unsigned int m_vertexCount;
	// ���������� �������� � �����
	unsigned int m_indexCount;
	// ������� �� ��������� ���������� � ��������� �����
	bool m_hasTexture;
	// �������������� ���� ������������� �����
	CBoundingBox const m_boundingBox;
	// ��� ���������� ������������� ����� (GL_TRIANGLES � �.�.)
	GLenum m_primitiveType;
	// ��� �������� ������������� ����� (GL_UNSIGNED_SHORT � �.�.)
	GLenum m_indexType;
};

