#include "pch.h"
#include "GLApplication.h"

CGLApplication* CGLApplication::m_pApplication = nullptr;

CGLApplication::CGLApplication(
	const char* title,
	int width,
	int height,
	bool needDepth,
	bool needStencil)
	: m_width(0)
	, m_height(0)
{
	// ������ ���� ��������� ���������� ��������� �������
	if (m_pApplication)
	{
		assert(!"Only one instance of the application is permitted");
		return;
	}
	m_pApplication = this;

	// ��������� ���� � ��������� ����� �� �������, ��� ��������
	// ��������� ����������� � ��� ��������-������������
	AddEventListener(this);

	// �������������� ���������� GLUT, ���������� ��
	// ��������� ��������� �������� ������
	int argc = 1;
	char const* argv[] = { "" };
	glutInit(&argc, const_cast<char**>(argv));

	// ������ ����� ������ �����, ����������� �������������
	glutInitDisplayMode(
		GLUT_RGBA | GLUT_DOUBLE | (needDepth ? GLUT_DEPTH : 0) | (needStencil ? GLUT_STENCIL : 0));

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
	glutSpecialFunc(&SpecialKeyHandler);
	glutMouseFunc(&MouseHandler);
	glutMotionFunc(&MotionHandler);
	glutIdleFunc(&IdleHandler);

	// ��� ������� ����� �������� ����������� ���������
	// �������, �������������� glut
}

void CGLApplication::MotionHandler(int x, int y)
{
	// ������� � ����������� ������ ���� ����
	class CMotionEvent
	{
	public:
		CMotionEvent(int x, int y)
			: m_x(x)
			, m_y(y)
		{
		}

		// �������� () �������������� ������� � ����� OnMotion ���������
		void operator()(IApplicationListener* pListener) const
		{
			pListener->OnMotion(m_x, m_y);
		}

	private:
		int m_x, m_y;
	};

	// ��������� ������� ���� ����������
	m_pApplication->DispatchEvent(CMotionEvent(x, y));
}

void CGLApplication::MouseHandler(int button, int state, int x, int y)
{
	// ������� � �������/���������� ������ ���� ����
	class CMouseEvent
	{
	public:
		CMouseEvent(int button, int state, int x, int y)
			: m_button(button)
			, m_state(state)
			, m_x(x)
			, m_y(y)
		{
		}

		// �������� () �������������� ������� � ����� OnMouse ���������
		void operator()(IApplicationListener* pListener) const
		{
			pListener->OnMouse(m_button, m_state, m_x, m_y);
		}

	private:
		int m_button, m_state, m_x, m_y;
	};

	// ��������� ������� ���� ����������
	m_pApplication->DispatchEvent(CMouseEvent(button, state, x, y));
}

void CGLApplication::DisplayHandler()
{
	// ������� � ������������� ����������� ����
	class CDisplayEvent
	{
	public:
		// �������� () �������������� ������� � ����� OnDisplay ���������
		void operator()(IApplicationListener* pListener) const
		{
			pListener->OnDisplay();
		}
	};

	// ��������� ������� ���� ����������
	m_pApplication->DispatchEvent(CDisplayEvent());
	// ���������� ������� ������� � ������� ������ �����
	glutSwapBuffers();
}

void CGLApplication::IdleHandler()
{
	Sleep(1);

	// ������� � ���, ��� ���������� ����� ��������
	class CIdleEvent
	{
	public:
		// �������� () �������������� ������� � ����� OnIdle ���������
		void operator()(IApplicationListener* pListener) const
		{
			pListener->OnIdle();
		}
	};

	// ��������� ������� ���� ����������
	m_pApplication->DispatchEvent(CIdleEvent());
}

void CGLApplication::ReshapeHandler(int width, int height)
{
	m_pApplication->UpdateWindowSize(width, height);
}

void CGLApplication::UpdateWindowSize(int width, int height)
{
	m_width = width;
	m_height = height;

	// ������� �� ��������� �������� ����
	class CReshapeEvent
	{
	public:
		CReshapeEvent(int width, int height)
			: m_width(width)
			, m_height(height)
		{
		}

		// �������� () �������������� ������� � ����� OnReshape ���������
		void operator()(IApplicationListener* pListener) const
		{
			pListener->OnReshape(m_width, m_height);
		}

	private:
		int m_width, m_height;
	};

	// ��������� ������� ���� ����������
	m_pApplication->DispatchEvent(CReshapeEvent(width, height));
}

void CGLApplication::SpecialKeyHandler(int key, int x, int y)
{
	// ������� � ������ ����������� �������
	class CSpecialKeyEvent
	{
	public:
		CSpecialKeyEvent(int key, int x, int y)
			: m_key(key)
			, m_x(x)
			, m_y(y)
		{
		}
		// �������� () �������������� ������� � ����� OnSpecialKey ���������
		void operator()(IApplicationListener* pListener) const
		{
			pListener->OnSpecialKey(m_key, m_x, m_y);
		}

	private:
		int m_key;
		unsigned m_x, m_y;
	};

	// ��������� ������� ���� ����������
	m_pApplication->DispatchEvent(CSpecialKeyEvent(key, x, y));
}

void CGLApplication::KeyboardHandler(unsigned char key, int x, int y)
{
	// ������� � ������� ������� �� ����������
	class CKeyboardEvent
	{
	public:
		CKeyboardEvent(unsigned char key, int x, int y)
			: m_key(key)
			, m_x(x)
			, m_y(y)
		{
		}
		// �������� () �������������� ������� � ����� OnKeyboard ���������
		void operator()(IApplicationListener* pListener) const
		{
			pListener->OnKeyboard(m_key, m_x, m_y);
		}

	private:
		unsigned char m_key;
		unsigned m_x, m_y;
	};

	// ��������� ������� ���� ����������
	m_pApplication->DispatchEvent(CKeyboardEvent(key, x, y));
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

// ������ ����� ����� ���� ���������� ��������-������������
void CGLApplication::OnInit()
{
}

CGLApplication::~CGLApplication(void)
{
}
