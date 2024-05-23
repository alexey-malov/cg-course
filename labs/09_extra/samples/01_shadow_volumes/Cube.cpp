#include "StdAfx.h"
#include "Cube.h"
#include "Vector3.h"

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
const float CCube::m_vertices[8][3] = 
{
	{-1, -1, -1},	// 0
	{+1, -1, -1},	// 1
	{+1, +1, -1},	// 2
	{-1, +1, -1},	// 3
	{-1, -1, +1},	// 4
	{+1, -1, +1},	// 5
	{+1, +1, +1},	// 6
	{-1, +1, +1},	// 7
};

// Массив координат граней (в порядке, совпадающем с 
// порядком объявления их в массиве цветов)
// индексы вершин граней перечисляются в порядке их обхода
// против часовой стрелки (если смотреть на грань снаружи)
const unsigned short CCube::m_faces[6][4] = 
{
	{4, 7, 3, 0},	// грань x<0
	{5, 1, 2, 6},	// грань x>0
	{4, 0, 1, 5},	// грань y<0
	{7, 6, 2, 3},	// грань y>0
	{0, 3, 2, 1},	// грань z<0
	{4, 5, 6, 7},	// грань z>0
};
const size_t CCube::m_faceCount = sizeof(m_faces) / sizeof(*m_faces);

std::vector<CCube::Edge> CCube::m_edges;

void CCube::InitEdges()
{
	// пробегаем по всем граням
	for (size_t faceIndex = 0; faceIndex < m_faceCount; ++faceIndex)
	{
		const unsigned short * face = m_faces[faceIndex];

		// Вычисляем нормаль к грани
		CVector3f v0 = CVector3f(m_vertices[face[0]]);
		CVector3f v1 = CVector3f(m_vertices[face[1]]);
		CVector3f v2 = CVector3f(m_vertices[face[2]]);
		CVector3f normal = Cross(v1 - v0, v2 - v0);

		// пробегаем по всем вершинам грани
		for (size_t vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
		{
			const unsigned short startVertex = face[vertexIndex];
			const unsigned short endVertex = face[(vertexIndex + 1) % 4];

			// Ищем среди существующих ребер ребро, содержащее вершины startVertex и endVertex
			size_t edgeIndex;
			for (edgeIndex = 0; edgeIndex < m_edges.size(); ++edgeIndex)
			{
				Edge & edge = m_edges[edgeIndex];
				if (
					(edge.vStart == startVertex && edge.vEnd == endVertex) ||
					(edge.vStart == endVertex && edge.vEnd == startVertex)				
					)
				{
					break;
				}
			}
			if (edgeIndex == m_edges.size()) // такого ребра нет
			{
				// добавляем новое ребро
				Edge newEdge;
				newEdge.vStart = startVertex;
				newEdge.vEnd = endVertex;
				newEdge.frontFaceNormal = normal;
				newEdge.backFaceNormalIsDefined = false;
				m_edges.push_back(newEdge);
			}
			else
			{
				// такое ребро уже есть
				Edge & edge = m_edges[edgeIndex];
				if (!edge.backFaceNormalIsDefined &&
					(edge.vStart == endVertex && edge.vEnd == startVertex)
					)
				{
					// Добавляем нормаль нелицевой грани ребра
					edge.backFaceNormalIsDefined = true;
					edge.backFaceNormal = normal;
				}
			}
		}
	}
	// Удаляем ребра, для которых не задана нормаль нелицевой грани
	for (int edgeIndex = m_edges.size() - 1; edgeIndex >=0; --edgeIndex)
	{
		if (!m_edges[edgeIndex].backFaceNormalIsDefined)
		{
			m_edges.erase(m_edges.begin() + edgeIndex);
		}
	}
}


CCube::CCube(float size)
:m_size(size * 0.5f)
{
	SetSideColor(CS_NEGATIVE_X, 255, 255, 255);
	SetSideColor(CS_POSITIVE_X, 255, 255, 255);
	SetSideColor(CS_NEGATIVE_Y, 255, 255, 255);
	SetSideColor(CS_POSITIVE_Y, 255, 255, 255);
	SetSideColor(CS_NEGATIVE_Z, 255, 255, 255);
	SetSideColor(CS_POSITIVE_Z, 255, 255, 255);
}

void CCube::DrawShadowVolumeSides(CVector3f const& lightPosition, float extrusionFactor)const
{
	// Среди всех ребер ищем те, которые являются силуэтными
	for (size_t edgeIndex = 0; edgeIndex < m_edges.size(); ++edgeIndex)
	{
		Edge const& edge = m_edges[edgeIndex];

		// Вектор направления на источник света
		CVector3f lightDirection = 
			lightPosition - CVector3f(m_vertices[edge.vStart]) * m_size;

		// Определяем освещенность прилегающих к ребру граней
		bool frontFaceIsLit = Dot(edge.frontFaceNormal, lightDirection) > 0;

		bool backFaceIsLit = Dot(edge.backFaceNormal, lightDirection) > 0;

		if (frontFaceIsLit != backFaceIsLit) // Это силуэтное ребро?
		{
			// Вытягиваем вершину от источника света через ребро
			// на заданный фактор вытягивания
			CVector3f v0 = CVector3f(m_vertices[edge.vStart]) * m_size;
			CVector3f v0e = 
				v0 + (v0 - lightPosition) * extrusionFactor;

			CVector3f v1 = CVector3f(m_vertices[edge.vEnd]) * m_size;
			CVector3f v1e = 
				v1 + (v1 - lightPosition) * extrusionFactor;

			// Задаем вершины четырехугольника в зависимости от того,
			// какая из прилегающих граней освещена
			if (frontFaceIsLit)
			{
				glVertex3fv(v1);
				glVertex3fv(v0);
				// четвертая координата, равная 0, вытягивает вершину
				// в бесконечность
				glVertex4f(v0e.x, v0e.y, v0e.z, 0);
				glVertex4f(v1e.x, v1e.y, v1e.z, 0);
			}
			else
			{
				glVertex3fv(v0);
				glVertex3fv(v1);
				// четвертая координата, равная 0, вытягивает вершину
				// в бесконечность
				glVertex4f(v1e.x, v1e.y, v1e.z, 0);
				glVertex4f(v0e.x, v0e.y, v0e.z, 0);
			}
		}	
	}
}

void CCube::DrawShadowVolumeCaps(CVector3f const& lightPosition, float extrusionFactor)const
{
	for (size_t faceIndex = 0; faceIndex < m_faceCount; ++faceIndex)
	{
		const unsigned short * face = m_faces[faceIndex];

		CVector3f v0 = CVector3f(m_vertices[face[0]]) * m_size;
		CVector3f v1 = CVector3f(m_vertices[face[1]]) * m_size;
		CVector3f v2 = CVector3f(m_vertices[face[2]]) * m_size;
		CVector3f v3 = CVector3f(m_vertices[face[3]]) * m_size;

		// нормаль к грани
		CVector3f faceNormal = Cross(v1 - v0, v2 - v0);

		// Вычисляем направление на источник света
		CVector3f lightDirection = lightPosition - v0;

		bool faceIsLit = Dot(faceNormal, lightDirection) > 0;

		float w = 1;
		if (!faceIsLit)	// Освещенная грань?
		{
			w = 0;	// вершины будут "вытягиваться" в бесконечность
			v0 += (v0 - lightPosition) * extrusionFactor;
			v1 += (v1 - lightPosition) * extrusionFactor;
			v2 += (v2 - lightPosition) * extrusionFactor;
			v3 += (v3 - lightPosition) * extrusionFactor;
		}

		glVertex4f(v0.x, v0.y, v0.z, w);
		glVertex4f(v1.x, v1.y, v1.z, w);
		glVertex4f(v2.x, v2.y, v2.z, w);
		glVertex4f(v3.x, v3.y, v3.z, w);
	}
}

void CCube::DrawShadowVolume(
	CVector3f const& lightPosition, float extrusionFactor)const
{
	// Инициализируем информацию о ребрах при первом вызове
	if (m_edges.empty())
	{
		InitEdges();
	}
	glBegin(GL_QUADS);
	// Рисуем боковые грани теневого объема, вытягивая силуэтные ребра
	DrawShadowVolumeSides(lightPosition, extrusionFactor);

	// Рисуем "верхнюю" и "нижнюю" части теневого объема,
	// вытягивая неосвещенные грани объекта в бесконечность
	DrawShadowVolumeCaps(lightPosition, extrusionFactor);
	glEnd();
}

void CCube::Draw()const
{
	glBegin(GL_QUADS);
	{
		for (size_t face = 0; face < m_faceCount; ++face)
		{
			// устанавливаем цвет грани
			glColor4ubv(m_sideColors[face]);
			const unsigned short * facePoints = m_faces[face];

			// получаем вершины очередной грани куба
			CVector3f p0(m_vertices[facePoints[0]]);
			CVector3f p1(m_vertices[facePoints[1]]);
			CVector3f p2(m_vertices[facePoints[2]]);
			CVector3f p3(m_vertices[facePoints[3]]);
			// Вычисляем координаты вершин куба с учетом его размера
			p0 *= m_size;// * 0.5f;
			p1 *= m_size;// * 0.5f;
			p2 *= m_size;// * 0.5f;
			p3 *= m_size;// * 0.5f;

			// Вычисляем нормаль к грани куба через
			// векторное произведение его смежных сторон
			CVector3f v01 = p1 - p0;
			CVector3f v02 = p2 - p0;
			CVector3f normal = Cross(v01, v02);
			normal.Normalize();

			glNormal3fv(normal);

			// В классе CVector3f перегружен оператор приведения к типу cosnt float*
			// поэтому фактически следующая строка эквивалентна
			// glVertex3fv(&p0.x);
			glVertex3fv(p0);
			glVertex3fv(p1);
			glVertex3fv(p2);
			glVertex3fv(p3);
		}
	}
	glEnd();
}


void CCube::SetSideColor(CubeSide side, GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	if (side < CS_MIN_CUBE_SIDE_INDEX || side > CS_MAX_CUBE_SIDE_INDEX)
	{
		throw std::invalid_argument("Invalid cube side");
	}

	m_sideColors[side][0] = r;
	m_sideColors[side][1] = g;
	m_sideColors[side][2] = b;
	m_sideColors[side][3] = a;
}
