#include "pch.h"
#include "Cube.h"

Cube::Cube(float size)
	: m_size(size)
{
	constexpr glm::vec4 defaultColor{ 1, 1, 1, 1 };
	SetSideColor(CubeSide::NEGATIVE_X, defaultColor);
	SetSideColor(CubeSide::POSITIVE_X, defaultColor);
	SetSideColor(CubeSide::NEGATIVE_Y, defaultColor);
	SetSideColor(CubeSide::POSITIVE_Y, defaultColor);
	SetSideColor(CubeSide::NEGATIVE_Z, defaultColor);
	SetSideColor(CubeSide::POSITIVE_Z, defaultColor);
}

void Cube::Draw() const
{

	/*
	   Y
	   |
	   |
	   |
	   +---X
	  /
	 /
	Z
	   3----2
	  /    /|
	 /    / |
	7----6  |
	|  0 |  1
	|    | /
    |    |/
	4----5
	*/
	// Массив координат вершин
	static constexpr float vertices[8][3] = {
		{ -1, -1, -1 }, // 0
		{ +1, -1, -1 }, // 1
		{ +1, +1, -1 }, // 2
		{ -1, +1, -1 }, // 3
		{ -1, -1, +1 }, // 4
		{ +1, -1, +1 }, // 5
		{ +1, +1, +1 }, // 6
		{ -1, +1, +1 }, // 7
	};

	// Массив координат граней (в порядке, совпадающем с
	// порядком объявления их в массиве цветов)
	// индексы вершин граней перечисляются в порядке их обхода
	// против часовой стрелки (если смотреть на грань снаружи)
	static constexpr unsigned char faces[6][4] = {
		{ 4, 7, 3, 0 }, // грань x<0
		{ 5, 1, 2, 6 }, // грань x>0
		{ 4, 0, 1, 5 }, // грань y<0
		{ 7, 6, 2, 3 }, // грань y>0
		{ 0, 3, 2, 1 }, // грань z<0
		{ 4, 5, 6, 7 }, // грань z>0
	};
	static size_t const faceCount = sizeof(faces) / sizeof(*faces);

	glBegin(GL_QUADS);
	{
		for (size_t face = 0; face < faceCount; ++face)
		{
			// устанавливаем диффузный и фоновый цвет грани
			glColor4fv(glm::value_ptr(m_sideColors[face]));
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, glm::value_ptr(m_sideColors[face]));
			const unsigned char* facePoints = faces[face];

			// получаем вершины очередной грани куба
			auto p0 = glm::make_vec3(vertices[facePoints[0]]);
			auto p1 = glm::make_vec3(vertices[facePoints[1]]);
			auto p2 = glm::make_vec3(vertices[facePoints[2]]);
			auto p3 = glm::make_vec3(vertices[facePoints[3]]);
			// Вычисляем координаты вершин куба с учетом его размера
			p0 *= m_size * 0.5f;
			p1 *= m_size * 0.5f;
			p2 *= m_size * 0.5f;
			p3 *= m_size * 0.5f;

			// Вычисляем нормаль к грани куба через
			// векторное произведение его смежных сторон
			auto v01 = p1 - p0;
			auto v02 = p2 - p0;
			auto normal = glm::normalize(glm::cross(v01, v02));

			glNormal3fv(glm::value_ptr(normal));

			glVertex3fv(glm::value_ptr(p0));
			glVertex3fv(glm::value_ptr(p1));
			glVertex3fv(glm::value_ptr(p2));
			glVertex3fv(glm::value_ptr(p3));
		}
	}
	glEnd();
}

void Cube::SetSideColor(CubeSide side, const glm::vec4& color)
{
	unsigned index = static_cast<unsigned>(side);
	m_sideColors[index] = color;
}

void Cube::SetSpecularColor(glm::vec4 color)
{
	m_specularColor = color;
}

void Cube::SetShininess(float shininess)
{
	m_shininess = shininess;
}
