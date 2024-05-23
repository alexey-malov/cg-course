#pragma once

#include "Vector3.h"

class CCube
{
public:
	enum CubeSide
	{
		CS_NEGATIVE_X,
		CS_POSITIVE_X,
		CS_NEGATIVE_Y,
		CS_POSITIVE_Y,
		CS_NEGATIVE_Z,
		CS_POSITIVE_Z,

		CS_MIN_CUBE_SIDE_INDEX = CS_NEGATIVE_X,
		CS_MAX_CUBE_SIDE_INDEX = CS_POSITIVE_Z,
	};
public:
	CCube(float size = 1);
	void Draw()const;
	void DrawShadowVolume(CVector3f const& lightPosition, float extrusionFactor)const;
	void SetSideColor(CubeSide side, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);
private:
	// Рисуем боковые стороны теневого объема
	void DrawShadowVolumeSides(CVector3f const& lightPosition, float extrusionFactor)const;

	// Рисуем верх и низ теневого объема
	void DrawShadowVolumeCaps(CVector3f const& lightPosition, float extrusionFactor)const;

	// выполняем инициализицию информации о ребрах
	static void InitEdges();

	// Размер куба
	float m_size;

	// Цвета сторон куба
	GLubyte m_sideColors[6][4];

	// Массив вершин куба
	static const float m_vertices[8][3];

	// Массив граней губа
	static const unsigned short m_faces[6][4];
	static const size_t m_faceCount;

	// Информация о ребре полигональной сетки
	struct Edge
	{
		// индексы вершин, составляющих ребро
		short vStart, vEnd;

		// нормали прилегающих к ребру граней
		// frontFaceNormal - нормаль грани, в состав которой вершины 
		// ребра входят в последовательности vStart, vEnd
		// backFaceNormal - нормаль грани, в состав которой вершины
		// ребра входя в последовательности vEnd, vStart
		CVector3f frontFaceNormal;
		CVector3f backFaceNormal;

		// была ли задана нелицевая прилегающая грань
		// (используется при сборе информации о ребрах)
		bool backFaceNormalIsDefined;
	};

	// Массив ребер куба
	static std::vector<Edge> m_edges;
};
