#include "StdAfx.h"
#include "MyApplication.h"

// Расстояние до ближней плоскости отсечения отображаемого объема
const double CMyApplication::ZNEAR = 0.5;
// Расстояние до дальей плоскости отсечения отображаемого объема
const double CMyApplication::ZFAR = 10;
// Угол обзора по вертикали
const double CMyApplication::FIELD_OF_VIEW = 60;

struct Vec3
{
	GLfloat x, y, z;
};

struct Color4
{
	GLubyte r, g, b, a;
};

struct Vertex
{
	Vec3 pos;
	Color4 color;
};

CMyApplication::CMyApplication(const char * title, int width, int height)
:CGLApplication(title, width, height)
,m_rotationController(width, height)
,m_pVertexArray(NULL)
,m_pIndexArray(NULL)
,m_pColorArray(NULL)
{
}

CMyApplication::~CMyApplication(void)
{
}

void CMyApplication::OnInit()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 1);
	
	glLoadIdentity();
	gluLookAt(0, 3, 4, 0, 0, 0, 0, 1, 0);

	InitArrays();

}

void CMyApplication::InitArrays()
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
	static const Vertex vertices[8] = 
	{
		{{-1, -1, -1}, {255, 0, 0, 255}},		// 0
		{{+1, -1, -1}, {255, 255, 0, 255}},		// 1
		{{+1, +1, -1}, {0, 255, 0, 255}},		// 2
		{{-1, +1, -1}, {0, 0, 0, 255}},			// 3
		{{-1, -1, +1}, {255, 0, 255, 255}},		// 4
		{{+1, -1, +1}, {255, 255, 255, 255}},	// 5
		{{+1, +1, +1}, {0, 255, 255, 255}},		// 6
		{{-1, +1, +1}, {0, 0, 255, 255}},		// 7
	};

	// Массив координат граней (в порядке, совпадающем с 
	// порядком объявления их в массиве цветов)
	// индексы вершин граней перечисляются в порядке их обхода
	// против часовой стрелки (если смотреть на грань снаружи)
	static const unsigned char faces[6][4] = 
	{
		{4, 7, 3, 0},	// грань x<0
		{5, 1, 2, 6},	// грань x>0
		{4, 0, 1, 5},	// грань y<0
		{7, 6, 2, 3},	// грань y>0
		{0, 3, 2, 1},	// грань z<0
		{4, 5, 6, 7},	// грань z>0
	};

	m_vertices.Create();
	m_vertices.BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);

	m_pVertexArray = 
		reinterpret_cast<GLubyte const*>(m_vertices.GetBufferPointer()) + 
		offsetof(Vertex, pos);
	m_pColorArray = 
		reinterpret_cast<GLubyte const*>(m_vertices.GetBufferPointer()) + 
		offsetof(Vertex, color);

	m_indices.Create();
	m_indices.BufferData(sizeof(faces), faces, GL_STATIC_DRAW);

	m_pIndexArray = reinterpret_cast<GLubyte const*>(m_indices.GetBufferPointer());
}

void CMyApplication::DrawCube()const
{
	// Разрешаем использование массива координат вершин
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// Включаем привязку для вершинного и индексного буферов
	m_vertices.Bind();
	m_indices.Bind();

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), m_pVertexArray);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), m_pColorArray);

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, m_pIndexArray);

	// Отключаем использование вершинного и индексного буферов
	CVertexBuffer::Unbind();
	CIndexBuffer::Unbind();

	// Выключаем использовнием массива цветов
	glDisableClientState(GL_COLOR_ARRAY);
	// Выключаем использование массива координат вершин
	glDisableClientState(GL_VERTEX_ARRAY);
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawCube();
}

void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, aspect, ZNEAR, ZFAR);
	glMatrixMode(GL_MODELVIEW);
}

void CMyApplication::OnMouse(int button, int state, int x, int y)
{
	m_rotationController.OnMouse(button, state, x, y);
}

void CMyApplication::OnMotion(int x, int y)
{
	if (m_rotationController.LeftButtonIsPressed())
	{
		m_rotationController.OnMotion(x, y);
		PostRedisplay();
	}
}
