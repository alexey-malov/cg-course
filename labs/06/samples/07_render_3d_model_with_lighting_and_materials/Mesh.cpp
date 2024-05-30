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
	// Проверяем, что подсетка начинается с существующего индекса
	if (startIndex >= m_indexCount)
	{
		throw std::out_of_range("Start vertex index is out of range");
	}

	// Проверка на допустимость количества индексов в подсетке
	if (
		(subMeshIndexCount == 0) || 
		(startIndex + subMeshIndexCount < startIndex) ||
		(startIndex + subMeshIndexCount > m_indexCount)
		)
	{
		throw std::out_of_range("SubMesh index count is out of range");
	}

	// Создаем подсетку и добавляем в массив подсеток
	SubMesh subMesh = {startIndex, subMeshIndexCount};
	m_subMeshes.push_back(subMesh);

	// Возвращаем индекс вставленной подсетки
	return static_cast<unsigned>(m_subMeshes.size() - 1);
}

CMesh::SubMesh CMesh::GetSubMesh(unsigned index)const
{
	return m_subMeshes.at(index);
}

unsigned CMesh::GetSubMeshCount()const
{
	return static_cast<unsigned>(m_subMeshes.size());
}

void CMesh::SetMaterialSubMesh(int materialIndex, unsigned subMeshIndex)
{
	// Проверка индекса подсетки на допустимость
	if (subMeshIndex >= GetSubMeshCount())
	{
		throw std::out_of_range("Sub mesh index is out of range");
	}

	// Индекс материала, равный -1, задается в подсетках,
	// использующих материал по умолчанию.
	// Т.к. нулевой элемент массива m_materialSubMesh хранит индекс
	// подсетки с материалом равным -1, увеличиваем индекс материала на 1
	++materialIndex;

	// проверяем его на допустимость (-1 - единственное отрицательное
	// значение допустимое для использования в качестве индекса материала)
	if (materialIndex < 0)
	{
		throw std::out_of_range("Invalid material index");
	}

	// Если мы пытаемся задать подсетку для материала, отсутствующего
	// в полигональной сетке, нужно выделить для него место в массиве
	// m_materialSubMesh
	if (static_cast<unsigned>(materialIndex) >= m_materialSubMesh.size())
	{
		m_materialSubMesh.insert(
			m_materialSubMesh.end(), 
			materialIndex - m_materialSubMesh.size() + 1, 
			-1);
	}

	// Если для данного материала подсетка уже была задана,
	// то не даем переопределить ее
	if (m_materialSubMesh[materialIndex] >= 0)
	{
		throw std::logic_error("Material sub mesh has already specified");
	}

	// добавляем индекс материала в массив используемых материалов,
	// предварительно уменьшив его на 1 (компенсируем инкремент индекса)
	m_materials.push_back(materialIndex - 1);

	// Задаем для данного материала индекс использующей его подсетки
	m_materialSubMesh[materialIndex] = subMeshIndex;
}

int CMesh::GetMaterialSubMesh(int materialIndex)const
{
	// Т.к. индексация элементов массива m_materialSubMesh "виртуально"
	// начинается с -1, увеличиваем индекс материала на 1, не забывая проверить
	// на допустимость значений
	++materialIndex;
	if (materialIndex < 0)
	{
		throw std::out_of_range("Invalid material index");
	}

	// Если индекс материала превышает количество материалов, для которых были
	// указаны индексы подсеток, то возвращаем значение -1, сигнализируя
	// об отсутствии подсетки для данного материала
	if (static_cast<unsigned>(materialIndex) >= m_materialSubMesh.size())
	{
		// нет подсетки, ассоциированной с данным материалом
		return -1;
	}

	// Иначе возвращаем индекс подсетки, соответствующей запрошенному материалу
	return m_materialSubMesh[materialIndex];
}

unsigned CMesh::GetMaterialsCount()const
{
	return static_cast<unsigned>(m_materials.size());
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