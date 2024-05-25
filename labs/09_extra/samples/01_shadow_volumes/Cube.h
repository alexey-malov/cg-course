#pragma once

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
	void DrawShadowVolume(glm::vec3 const& lightPosition, float extrusionFactor) const;
	void SetSideColor(CubeSide side, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);
private:
	// Рисуем боковые стороны теневого объема
	void DrawShadowVolumeSides(glm::vec3 const& lightPosition, float extrusionFactor) const;

	// Рисуем верх и низ теневого объема
	void DrawShadowVolumeCaps(glm::vec3 const& lightPosition, float extrusionFactor) const;

	// выполняем инициализацию информации о ребрах
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
		short vStart = 0, vEnd = 0;

		// нормали прилегающих к ребру граней
		// frontFaceNormal - нормаль грани, в состав которой вершины
		// ребра входят в последовательности vStart, vEnd
		// backFaceNormal - нормаль грани, в состав которой вершины
		// ребра входя в последовательности vEnd, vStart
		glm::vec3 frontFaceNormal = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 backFaceNormal = glm::vec3(0.0f, 0.0f, 0.0f);

		// была ли задана нелицевая прилегающая грань
		// (используется при сборе информации о ребрах)
		bool backFaceNormalIsDefined = false;
	};

	// Массив ребер куба
	static std::vector<Edge> m_edges;
};
