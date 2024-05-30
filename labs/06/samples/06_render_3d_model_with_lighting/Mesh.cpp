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
