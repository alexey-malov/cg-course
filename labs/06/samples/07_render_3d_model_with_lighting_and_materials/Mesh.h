#pragma once

#include "Vertex.h"
#include "BoundingBox.h"

class CMesh : public boost::noncopyable
{
public:
	// Структура, описывающая подсетку
	struct SubMesh
	{
		// Начало подсетки в массиве индексов сетки
		unsigned startIndex;
		// Количество индексов в подсетке
		unsigned indexCount;
	};

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

	// Добавляем подсетку, диапазон индексов внутри сетки, объединенных
	// по некоторому признаку (например, по общему материалу)
	// Возвращается индекс добавленной подсетки
	unsigned AddSubMesh(unsigned startIndex, unsigned subMeshIndexCount);

	// Количество подсеток
	unsigned GetSubMeshCount()const;

	// Возвращаем информацию о подсетке с заданным индексом
	SubMesh GetSubMesh(unsigned index)const;

	// Задаем индекс подсетки, грани которой имеют материал materialIndex
	void SetMaterialSubMesh(int materialIndex, unsigned subMeshIndex);

	// Получаем индекс подсетки, грани которой имеют материал materialIndex
	int GetMaterialSubMesh(int materialIndex)const;

	// Получаем количество материалов, используемых в сетке
	unsigned GetMaterialsCount()const;

	// Получаем индес материала по его порядковому номеру в сетке
	int GetMaterial(unsigned index)const;

	// Получаем количество байт, требуемых для хранения одного индекса
	unsigned GetIndexSize()const;
private:
	// Массив подсеток внутри данной сетки
	std::vector<SubMesh> m_subMeshes;
	// Массив индексов материалов, используемых данной сеткой
	std::vector<int> m_materials;
	// Массив, задающий индекс подсетки для каждого материала
	std::vector<int> m_materialSubMesh;

	// Смещение в вершинном буфере к вершинам данной сетки
	unsigned int m_vertexBufferOffset;
	// Смещение в индексном буфере к индексам данной сетки
	unsigned int m_indexBufferOffset;
	// Количество вершин в сетке
	unsigned int m_vertexCount;
	// Количество индексов в сетке
	unsigned int m_indexCount;
	// Связаны ли вершинные координаты с вершинами сетки
	bool m_hasTexture;
	// Ограничивающий блок полигональной сетки
	CBoundingBox const m_boundingBox;
	// Тип примитивов полигональной сетки (GL_TRIANGLES и т.п.)
	GLenum m_primitiveType;
	// Тип индексов полигональной сетки (GL_UNSIGNED_SHORT и т.п.)
	GLenum m_indexType;
};

