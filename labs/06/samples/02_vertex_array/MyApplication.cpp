#include "StdAfx.h"
#include "MyApplication.h"

// Расстояние до ближней плоскости отсечения отображаемого объема
const double CMyApplication::ZNEAR = 0.5;
// Расстояние до дальей плоскости отсечения отображаемого объема
const double CMyApplication::ZFAR = 10;
// Угол обзора по вертикали
const double CMyApplication::FIELD_OF_VIEW = 60;

// Структура - трехмерный вектор
struct Vec3
{
	GLfloat x, y, z;
};

// Структура - четырехкомпонентный цвет
struct Color4
{
	GLubyte r, g, b, a;
};

// Структура - вершина
struct Vertex
{
	Vec3 pos;
	Color4 color;
};

CMyApplication::CMyApplication(const char * title, int width, int height)
:CGLApplication(title, width, height)
,m_rotationController(width, height)
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
}

void CMyApplication::DrawTriangle()const
{
	static const Vec3 coords[3] = 
	{
		{-1, -1, -3},
		{1, -1, -3},
		{0, 1, -3},
	};

	static const Color4 colors[3] = 
	{
		{255, 0, 0, 255},
		{0, 255, 0, 255},
		{0, 0, 255, 255},
	};

	// Задаем адрес массива вершин
	glVertexPointer(
		3,			// Координаты задаются в виде трехкомпонентного вектора
		GL_FLOAT,	// типа float
		sizeof(Vec3), // Количество байт между началом i-ой и i+1-ой вершинами
		&coords[0]); // Адрес первой вершины

	// Аналогично задаем параметры массива цветов
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Color4), &colors[0]);

	// Разрешаем использование массива координат вершин
	glEnableClientState(GL_VERTEX_ARRAY);
	// И массива цветов
	glEnableClientState(GL_COLOR_ARRAY);

	// Рисуем массив треугольников начиная с 0-вершины, всего 3 вершины
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Выключаем использовнием массива цветов
	glDisableClientState(GL_COLOR_ARRAY);
	// Выключаем использование массива координат вершин
	glDisableClientState(GL_VERTEX_ARRAY);
}

void CMyApplication::DrawCube()const
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

	// Массив граней, а точнее, индексов составляющих их вершин.
	// Индексы вершин граней перечисляются в порядке их обхода
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

	// Передаем информацию о массиве вершин
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].pos);
	// и массиве цветов
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &vertices[1].color);

	// Разрешаем использование массива координат вершин и цветов
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, faces);

	// Выключаем использовнием массива цветов
	glDisableClientState(GL_COLOR_ARRAY);
	// Выключаем использование массива координат вершин
	glDisableClientState(GL_VERTEX_ARRAY);
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawTriangle();
	DrawCube();
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


void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	m_rotationController.ResizeWindow(width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, aspect, ZNEAR, ZFAR);
	glMatrixMode(GL_MODELVIEW);
}
