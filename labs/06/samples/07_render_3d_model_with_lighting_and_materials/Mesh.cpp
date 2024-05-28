#include "StdAfx.h"
#include "Mesh.h"

CMesh::CMesh(
	unsigned int vertexBufferOffset, 
	unsigned int indexBufferOffset, 
	unsigned vertexCount, 
	unsigned indexCount, 
	bool hasTexture,
	CBoundingBox const& boundingBox,
	GLenum primitiveType,
	GLenum indexType
	)
:m_indexBufferOffset(indexBufferOffset)
,m_vertexBufferOffset(vertexBufferOffset)
,m_vertexCount(vertexCount)
,m_indexCount(indexCount)
,m_hasTexture(hasTexture)
,m_boundingBox(boundingBox)
,m_primitiveType(primitiveType)
,m_indexType(indexType)
{
}

bool CMesh::HasTextureCoords()const
{
	return m_hasTexture;
}

unsigned int CMesh::GetVertexBufferOffset()const
{
	return m_vertexBufferOffset;
}

unsigned int CMesh::GetIndexBufferOffset()const
{
	return m_indexBufferOffset;
}

unsigned int CMesh::GetIndexCount()const
{
	return m_indexCount;
}

CBoundingBox const& CMesh::GetBoundingBox()const
{
	return m_boundingBox;
}

GLenum CMesh::GetPrimitiveType()const
{
	return m_primitiveType;
}

GLenum CMesh::GetIndexType()const
{
	return m_indexType;
}

unsigned int CMesh::GetVertexCount()const
{
	return m_vertexCount;
}

unsigned CMesh::AddSubMesh(unsigned startIndex, unsigned subMeshIndexCount)
{
	// ���������, ��� �������� ���������� � ������������� �������
	if (startIndex >= m_indexCount)
	{
		throw std::out_of_range("Start vertex index is out of range");
	}

	// �������� �� ������������ ���������� �������� � ��������
	if (
		(subMeshIndexCount == 0) || 
		(startIndex + subMeshIndexCount < startIndex) ||
		(startIndex + subMeshIndexCount > m_indexCount)
		)
	{
		throw std::out_of_range("SubMesh index count is out of range");
	}

	// ������� �������� � ��������� � ������ ��������
	SubMesh subMesh = {startIndex, subMeshIndexCount};
	m_subMeshes.push_back(subMesh);

	// ���������� ������ ����������� ��������
	return m_subMeshes.size() - 1;
}

CMesh::SubMesh CMesh::GetSubMesh(unsigned index)const
{
	return m_subMeshes.at(index);
}

unsigned CMesh::GetSubMeshCount()const
{
	return m_subMeshes.size();
}

void CMesh::SetMaterialSubMesh(int materialIndex, unsigned subMeshIndex)
{
	// �������� ������� �������� �� ������������
	if (subMeshIndex >= GetSubMeshCount())
	{
		throw std::out_of_range("Sub mesh index is out of range");
	}

	// ������ ���������, ������ -1, �������� � ���������,
	// ������������ �������� �� ���������.
	// �.�. ������� ������� ������� m_materialSubMesh ������ ������
	// �������� � ���������� ������ -1, ����������� ������ ��������� �� 1
	++materialIndex;

	// ��������� ��� �� ������������ (-1 - ������������ �������������
	// �������� ���������� ��� ������������� � �������� ������� ���������)
	if (materialIndex < 0)
	{
		throw std::out_of_range("Invalid material index");
	}

	// ���� �� �������� ������ �������� ��� ���������, ��������������
	// � ������������� �����, ����� �������� ��� ���� ����� � �������
	// m_materialSubMesh
	if (static_cast<unsigned>(materialIndex) >= m_materialSubMesh.size())
	{
		m_materialSubMesh.insert(
			m_materialSubMesh.end(), 
			materialIndex - m_materialSubMesh.size() + 1, 
			-1);
	}

	// ���� ��� ������� ��������� �������� ��� ���� ������,
	// �� �� ���� �������������� ��
	if (m_materialSubMesh[materialIndex] >= 0)
	{
		throw std::logic_error("Material sub mesh has already specified");
	}

	// ��������� ������ ��������� � ������ ������������ ����������,
	// �������������� �������� ��� �� 1 (������������ ��������� �������)
	m_materials.push_back(materialIndex - 1);

	// ������ ��� ������� ��������� ������ ������������ ��� ��������
	m_materialSubMesh[materialIndex] = subMeshIndex;
}

int CMesh::GetMaterialSubMesh(int materialIndex)const
{
	// �.�. ���������� ��������� ������� m_materialSubMesh "����������"
	// ���������� � -1, ����������� ������ ��������� �� 1, �� ������� ���������
	// �� ������������ ��������
	++materialIndex;
	if (materialIndex < 0)
	{
		throw std::out_of_range("Invalid material index");
	}

	// ���� ������ ��������� ��������� ���������� ����������, ��� ������� ����
	// ������� ������� ��������, �� ���������� �������� -1, ������������
	// �� ���������� �������� ��� ������� ���������
	if (static_cast<unsigned>(materialIndex) >= m_materialSubMesh.size())
	{
		// ��� ��������, ��������������� � ������ ����������
		return -1;
	}

	// ����� ���������� ������ ��������, ��������������� ������������ ���������
	return m_materialSubMesh[materialIndex];
}

unsigned CMesh::GetMaterialsCount()const
{
	return m_materials.size();
}

int CMesh::GetMaterial(unsigned index)const
{
	return m_materials.at(index);
}

unsigned CMesh::GetIndexSize()const
{
	return
		(m_indexType == GL_UNSIGNED_BYTE) ? sizeof(GLubyte) :
		(m_indexType == GL_UNSIGNED_SHORT) ? sizeof(GLshort) :
		(m_indexType == GL_UNSIGNED_INT) ? sizeof(GLuint) : 0;

}