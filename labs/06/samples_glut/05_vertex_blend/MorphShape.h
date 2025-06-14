#pragma once

#include "../libgl/Buffer.h"
#include "../libgl/Vector3.h"
#include "../libgl/Shaders.h"

struct Vertex
{
	CVector3f startPos;
	CVector3f endPos;
};

class CMorphShape : public boost::noncopyable
{
public:
	CMorphShape(size_t numberOfVertices, float maxRadius, float minRadius);

	// Сообщаем классу о программе и расположении ее переменных
	void SetProgram(GLuint program, GLuint phaseLocation, GLuint endPosLocation);

	// Рисуем фазу
	void Draw(float phase)const;
private:
	// Инициализация фигуры
	void Create()const;

	// Количество вершин в фигуре
	size_t const m_numberOfVertices;

	// Минимальный и максимальный радиус лепестков
	float const m_maxRadius;
	float const m_minRadius;

	// Вершинный буфер и указатели на находящиеся в нем массивы
	mutable bool m_initialized;
	mutable CVertexBuffer m_vertexBuffer;
	mutable GLubyte const * m_pStartPosArray;
	mutable GLubyte const * m_pEndPosArray;

	CProgramHandle m_program;
	GLuint m_phaseLocation;
	GLuint m_endPosLocation;
};
