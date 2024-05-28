#include "StdAfx.h"
#include "MyApplication.h"

// ���������� �� ������� ��������� ��������� ������������� ������
const double CMyApplication::ZNEAR = 0.5;
// ���������� �� ������ ��������� ��������� ������������� ������
const double CMyApplication::ZFAR = 10;
// ���� ������ �� ���������
const double CMyApplication::FIELD_OF_VIEW = 60;

// ��������� - ���������� ������
struct Vec3
{
	GLfloat x, y, z;
};

// ��������� - ������������������� ����
struct Color4
{
	GLubyte r, g, b, a;
};

// ��������� - �������
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

	// ������ ����� ������� ������
	glVertexPointer(
		3,			// ���������� �������� � ���� ����������������� �������
		GL_FLOAT,	// ���� float
		sizeof(Vec3), // ���������� ���� ����� ������� i-�� � i+1-�� ���������
		&coords[0]); // ����� ������ �������

	// ���������� ������ ��������� ������� ������
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Color4), &colors[0]);

	// ��������� ������������� ������� ��������� ������
	glEnableClientState(GL_VERTEX_ARRAY);
	// � ������� ������
	glEnableClientState(GL_COLOR_ARRAY);

	// ������ ������ ������������� ������� � 0-�������, ����� 3 �������
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// ��������� ������������� ������� ������
	glDisableClientState(GL_COLOR_ARRAY);
	// ��������� ������������� ������� ��������� ������
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
	// ������ ��������� ������
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

	// ������ ������, � ������, �������� ������������ �� ������.
	// ������� ������ ������ ������������� � ������� �� ������
	// ������ ������� ������� (���� �������� �� ����� �������)
	static const unsigned char faces[6][4] = 
	{
		{4, 7, 3, 0},	// ����� x<0
		{5, 1, 2, 6},	// ����� x>0
		{4, 0, 1, 5},	// ����� y<0
		{7, 6, 2, 3},	// ����� y>0
		{0, 3, 2, 1},	// ����� z<0
		{4, 5, 6, 7},	// ����� z>0
	};

	// �������� ���������� � ������� ������
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].pos);
	// � ������� ������
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &vertices[1].color);

	// ��������� ������������� ������� ��������� ������ � ������
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, faces);

	// ��������� ������������� ������� ������
	glDisableClientState(GL_COLOR_ARRAY);
	// ��������� ������������� ������� ��������� ������
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

	// ��������� ����������� ������ ���������� ������� ����
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, aspect, ZNEAR, ZFAR);
	glMatrixMode(GL_MODELVIEW);
}
