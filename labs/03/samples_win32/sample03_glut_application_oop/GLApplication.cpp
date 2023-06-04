#include "GLApplication.h"

CGLApplication* CGLApplication::m_pApplication = NULL;

CGLApplication::CGLApplication(
	const char* title,
	int width,
	int height,
	bool needDepth,
	bool needStencil)
{
	// Только один экземпляр приложения разрешено создать
	if (m_pApplication)
	{
		assert(!"Only one instance of the application is permitted");
		return;
	}
	m_pApplication = this;

	// Инициализируем библиотеку GLUT, подсовывая ей
	// фиктивные параметры командой строки
	int argc = 1;
	char** argv = {};
	glutInit(&argc, argv);

	// Задаем режим буфера кадра, запрошенный пользователем
	glutInitDisplayMode(
		GLUT_RGBA | GLUT_DOUBLE | (needDepth ? GLUT_DEPTH : 0) | (needStencil ? GLUT_STENCIL : 0));

	// Если пользователь указал размеры окна, сообщаем о них GLUT
	if (width > 0 && height > 0)
	{
		glutInitWindowSize(width, height);
	}

	// Создаем окно приложения
	glutCreateWindow(title);

	// Задаем обработчики событий
	InitEventHandlers();
}

void CGLApplication::InitEventHandlers()
{
	// Инициализируем обработчики часто используемых событий
	glutDisplayFunc(&DisplayHandler);
	glutReshapeFunc(&ReshapeHandler);
	glutKeyboardFunc(&KeyboardHandler);
	glutMouseFunc(&MouseHandler);
	glutMotionFunc(&MotionHandler);
	glutIdleFunc(&IdleHandler);
	// При желании можно добавить обработчики остальных
	// событий, поддерживаемых glut
}

void CGLApplication::MotionHandler(int x, int y)
{
	// Переадресовываем вызов обработчика события экземпляру класса приложения
	m_pApplication->OnMotion(x, y);
}

void CGLApplication::MouseHandler(int button, int state, int x, int y)
{
	// Переадресовываем вызов обработчика события экземпляру класса приложения
	m_pApplication->OnMouse(button, state, x, y);
}

void CGLApplication::DisplayHandler()
{
	// Переадресовываем вызов обработчика события экземпляру класса приложения
	m_pApplication->OnDisplay();
	// Обмениваем местами теневой и лицевой буферы кадра
	glutSwapBuffers();
}

void CGLApplication::IdleHandler()
{
	// ничего не делаем
	m_pApplication->OnIdle();
}

void CGLApplication::ReshapeHandler(int width, int height)
{
	// Переадресовываем вызов обработчика события экземпляру класса приложения
	m_pApplication->OnReshape(width, height);
}

void CGLApplication::KeyboardHandler(unsigned char key, int x, int y)
{
	// Переадресовываем вызов обработчика события экземпляру класса приложения
	m_pApplication->OnKeyboard(key, x, y);
}

// Метод может быть перегружен в классе-наследнике
void CGLApplication::OnReshape(int width, int height)
{
	// Обработчик по умолчанию всего лишь задает
	// порт просмотра размером с клиентскую область окна
	glViewport(0, 0, width, height);
}

// Метод может быть перегружен в классе-наследнике
void CGLApplication::OnKeyboard(unsigned char /*key*/, int /*x*/, int /*y*/)
{
}

// Метод может быть перегружен в классе-наследнике
void CGLApplication::OnMouse(int /*button*/, int /*state*/, int /*x*/, int /*y*/)
{
}

// Метод может быть перегружен в классе-наследнике
void CGLApplication::OnMotion(int /*x*/, int /*y*/)
{
}

// Метод может быть перегружен в классе-наследнике
void CGLApplication::OnIdle()
{
}

// Метод может быть перегружен в классе-наследнике
void CGLApplication::OnInit()
{
}

void CGLApplication::PostRedisplay()
{
	// Инициируем перерисовку калпа
	glutPostRedisplay();
}

void CGLApplication::MainLoop(void)
{
	// Вызываем обработчик OnInit, который может быть
	// перегружен в классе-наследнике
	OnInit();
	// И переходим в цикл обработки сообщений GLUT
	glutMainLoop();
}

CGLApplication::~CGLApplication(void)
{
}
