#include "StdAfx.h"
#include "GLApplication.h"

CGLApplication * CGLApplication::m_pApplication = NULL;

CGLApplication::CGLApplication(
   const char * title, 
   int width, 
   int height, 
   bool needDepth, 
   bool needStencil
   )
{
	// ������ ���� ��������� ���������� ��������� �������
	if (m_pApplication)
	{
		assert(!"Only one instance of the application is permitted");
		return;
	}
	m_pApplication = this;

	// �������������� ���������� GLUT, ���������� ��
	// ��������� ��������� �������� ������
	int argc = 1;
	char *argv[] = {"", };
	glutInit(&argc, argv);

	// ������ ����� ������ �����, ����������� �������������
	glutInitDisplayMode(
		GLUT_RGBA |
		GLUT_DOUBLE |
		(needDepth ? GLUT_DEPTH : 0) |
		(needStencil ? GLUT_STENCIL : 0)
		);

	// ���� ������������ ������ ������� ����, �������� � ��� GLUT
	if (width > 0 && height > 0)
	{
		glutInitWindowSize(width, height);
	}

	// ������� ���� ����������
	glutCreateWindow(title);

	// ������ ����������� �������
	InitEventHandlers();
}

void CGLApplication::InitEventHandlers()
{
	// �������������� ����������� ����� ������������ �������
	glutDisplayFunc(&DisplayHandler);
	glutReshapeFunc(&ReshapeHandler);
	glutKeyboardFunc(&KeyboardHandler);
	glutMouseFunc(&MouseHandler);
	glutMotionFunc(&MotionHandler);
	glutIdleFunc(&IdleHandler);

#if (GLUT_API_VERSION >= 2)
	glutSpaceballMotionFunc(&SpaceballMotionHandler);
	glutSpaceballRotateFunc(&SpaceballRotateHandler);
	glutSpaceballButtonFunc(&SpaceballButtonHandler);
#endif
	// ��� ������� ����� �������� ����������� ���������
	// �������, �������������� glut
}

void CGLApplication::SpaceballMotionHandler(int x, int y, int z)
{
	m_pApplication->OnSpaceballMotion(x, y, z);
}

void CGLApplication::SpaceballRotateHandler(int x, int y, int z)
{
	m_pApplication->OnSpaceballRotation(x, y, z);
}

void CGLApplication::SpaceballButtonHandler(int button, int state)
{
	m_pApplication->OnSpaceballButton(button, state);
}

void CGLApplication::MotionHandler(int x, int y)
{
	// ���������������� ����� ����������� ������� ���������� ������ ����������
	m_pApplication->OnMotion(x, y);
}

void CGLApplication::MouseHandler(int button, int state, int x, int y)
{
	// ���������������� ����� ����������� ������� ���������� ������ ����������
	m_pApplication->OnMouse(button, state, x, y);
}


void CGLApplication::DisplayHandler()
{
	// ���������������� ����� ����������� ������� ���������� ������ ����������
	m_pApplication->OnDisplay();
	// ���������� ������� ������� � ������� ������ �����
	glutSwapBuffers();
}

void CGLApplication::IdleHandler()
{
	Sleep(1);
	// ������ �� ������
	m_pApplication->OnIdle();
}

void CGLApplication::ReshapeHandler(int width, int height)
{
	// ���������������� ����� ����������� ������� ���������� ������ ����������
	m_pApplication->OnReshape(width, height);
}

void CGLApplication::KeyboardHandler(unsigned char key, int x, int y)
{
	// ���������������� ����� ����������� ������� ���������� ������ ����������
	m_pApplication->OnKeyboard(key, x, y);
}

// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnReshape(int width, int height)
{
	// ���������� �� ��������� ����� ���� ������
	// ���� ��������� �������� � ���������� ������� ����
	glViewport(0, 0, width, height);
}

// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnKeyboard(unsigned char /*key*/, int /*x*/, int /*y*/)
{

}

// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnMouse(int /*button*/, int /*state*/, int /*x*/, int /*y*/)
{

}

// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnMotion(int /*x*/, int /*y*/)
{

}

// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnIdle()
{

}

// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnSpaceballMotion(int /*x*/, int /*y*/, int /*z*/)
{

}

// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnSpaceballRotation(int /*x*/, int /*y*/, int /*z*/)
{

}


// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnSpaceballButton(int /*button*/, int /*state*/)
{

}

// ����� ����� ���� ���������� � ������-����������
void CGLApplication::OnInit()
{

}


void CGLApplication::PostRedisplay()
{
	// ���������� ����������� �����
	glutPostRedisplay();
}

void CGLApplication::SetTimer(int milliseconds, TimerProc proc, int value)
{
	// ������������� ������
	glutTimerFunc(milliseconds, proc, value);
}

void CGLApplication::MainLoop(void)
{
	// �������� ���������� OnInit, ������� ����� ���� 
	// ���������� � ������-����������
	OnInit();
	// � ��������� � ���� ��������� ��������� GLUT
	glutMainLoop();
}


CGLApplication::~CGLApplication(void)
{
}

