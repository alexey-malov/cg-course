#pragma once

#include "Vertex.h"
#include "BoundingBox.h"

class CMesh : public boost::noncopyable
{
public:
	CMesh(
		unsigned int vertexBufferOffset,	// смещение в буфере вершин
		unsigned int indexBufferOffset,		// смещение в буфере индексов
		unsigned vertexCount,				// количество вершин
		unsigned indexCount,				// количество индексов
		bool hasTextureCoords,				// наличие текстурных координат
		CBoundingBox const& boundingBox,	// ограничивающий блок
		GLenum primitiveType,				// тип примитивов сетки
		GLenum indexType					// тип индексов сетки
		);

	// Есть ли в сетке текстурные координаты?
	bool HasTextureCoords()const;

	// Возвращаем смещение относительно начала буфера вершин
	unsigned int GetVertexBufferOffset()const;

	// Возвращаем смещение относительно начала буфера индексов
	unsigned int GetIndexBufferOffset()const;

	// Возвращаем количество индексов
	unsigned int GetIndexCount()const;

	// Возвращаем количество вершин
	unsigned int GetVertexCount()const;

	// Возвращаем ограничивающий блок
	CBoundingBox const& GetBoundingBox()const;

	// Возвращаем тип примитивов (GL_TRIANGLES, GL_TRIANGLE_STRIP и т.п.)
	GLenum GetPrimitiveType()const;

	// Возвращаем тип данных для хранения индексов (GL_UNSIGNED_SHORT и т.п.)
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

