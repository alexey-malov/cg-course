// Application.cpp: implementation of the CApplication class.
//
//////////////////////////////////////////////////////////////////////
#include "Application.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplication* CApplication::m_pApplication = NULL;

void CApplication::Run()
{
	glutMainLoop();
}

CApplication::~CApplication()
{
}

CApplication::CApplication(int* pArgc, char** argv, int width, int height, const char* title)
	: m_argc(*pArgc)
	, m_argv(argv)
{
	// initialize glut
	glutInit(pArgc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutCreateWindow(title);
	glutDisplayFunc(&DisplayHandler);
	m_pApplication = this;
}

void CApplication::EnableIdleHandler()
{
	glutIdleFunc(&IdleHandler);
}

// can be overriden
void CApplication::OnDraw()
{
	glutSwapBuffers();
}

// can be overriden
void CApplication::OnIdle()
{
}

void CApplication::DisplayHandler()
{
	m_pApplication->OnDraw();
}

void CApplication::IdleHandler()
{
	m_pApplication->OnIdle();
}