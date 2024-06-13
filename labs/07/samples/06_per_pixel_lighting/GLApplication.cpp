#include "StdAfx.h"
#include "GLApplication.h"
#include "IApplicationListener.h"

CGLApplication * CGLApplication::m_pApplication = NULL;

CGLApplication::CGLApplication(
   const char * title, 
   int width, 
   int height, 
   bool needDepth, 
   bool needStencil
   )
{
	// Только один экземпляр приложения разрешено создать
	if (m_pApplication)
	{
		assert(!"Only one instance of the application is permitted");
		return;
	}
	m_pApplication = this;

	// Добавляем себя в слушатели своих же событий, что облегчит
	// получение уведомлений о них классами-наследниками
	AddEventListener(this);

	// Инициализируем библиотеку GLUT, подсовывая ей
	// фиктивные параметры командой строки
	int argc = 1;
	char* argv[] = {
		(char*)"",
	};
	glutInit(&argc, argv);

	// Задаем режим буфера кадра, запрошенный пользователем
	glutInitDisplayMode(
		GLUT_RGBA |
		GLUT_DOUBLE |
		(needDepth ? GLUT_DEPTH : 0) |
		(needStencil ? GLUT_STENCIL : 0)
		);

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
	glutSpecialFunc(&SpecialKeyHandler);
	glutMouseFunc(&MouseHandler);
	glutMotionFunc(&MotionHandler);
	glutIdleFunc(&IdleHandler);

	// При желании можно добавить обработчики остальных
	// событий, поддерживаемых glut
}

void CGLApplication::MotionHandler(int x, int y)
{
	// Событие о перемещении кнопки мыши мыши
	class CMotionEvent
	{
	public:
		CMotionEvent(int x, int y)
			:m_x(x), m_y(y)
		{
		}

		// Оператор () перенаправляет событие в метод OnMotion слушателя
		void operator()(IApplicationListener * pListener)const
		{
			pListener->OnMotion(m_x, m_y);
		}
	private:
		int m_x, m_y;
	};

	// Рассылаем событие всем слушателям
	m_pApplication->DispatchEvent(CMotionEvent(x, y));
}

void CGLApplication::MouseHandler(int button, int state, int x, int y)
{
	// Событие о нажатии/отпускании кнопки мыши мыши
	class CMouseEvent
	{
	public:
		CMouseEvent(int button, int state, int x, int y)
			:m_button(button), m_state(state), m_x(x), m_y(y)
		{
		}

		// Оператор () перенаправляет событие в метод OnMouse слушателя
		void operator()(IApplicationListener * pListener)const
		{
			pListener->OnMouse(m_button, m_state, m_x, m_y);
		}
	private:
		int m_button, m_state, m_x, m_y;
	};

	// Рассылаем событие всем слушателям
	m_pApplication->DispatchEvent(CMouseEvent(button, state, x, y));
}


void CGLApplication::DisplayHandler()
{
	// Событие о необходимости перерисовки окна
	class CDisplayEvent
	{
	public:
		// Оператор () перенаправляет событие в метод OnDisplay слушателя
		void operator()(IApplicationListener * pListener)const
		{
			pListener->OnDisplay();
		}
	};


	// Рассылаем событие всем слушателям
	m_pApplication->DispatchEvent(CDisplayEvent());
	// Обмениваем местами теневой и лицевой буферы кадра
	glutSwapBuffers();
}

void CGLApplication::IdleHandler()
{
	Sleep(1);

	// Событие о том, что приложению нечем заняться
	class CIdleEvent
	{
	public:
		// Оператор () перенаправляет событие в метод OnIdle слушателя
		void operator()(IApplicationListener * pListener)const
		{
			pListener->OnIdle();
		}
	};

	// Рассылаем событие всем слушателям
	m_pApplication->DispatchEvent(CIdleEvent());
}

void CGLApplication::ReshapeHandler(int width, int height)
{
	// Событие об изменении размеров окна
	class CReshapeEvent
	{
	public:
		CReshapeEvent(int width, int height)
			:m_width(width), m_height(height)
		{
		}

		// Оператор () перенаправляет событие в метод OnReshape слушателя
		void operator()(IApplicationListener * pListener)const
		{
			pListener->OnReshape(m_width, m_height);
		}
	private:
		int m_width, m_height;
	};

	// Рассылаем событие всем слушателям
	m_pApplication->DispatchEvent(CReshapeEvent(width, height));
}

void CGLApplication::SpecialKeyHandler(int key, int x, int y)
{
	// Событие о нажатии специальной клавиши
	class CSpecialKeyEvent
	{
	public:
		CSpecialKeyEvent(int key, int x, int y)
			:m_key(key), m_x(x), m_y(y)
		{
		}
		// Оператор () перенаправляет событие в метод OnSpecialKey слушателя
		void operator()(IApplicationListener * pListener)const
		{
			pListener->OnSpecialKey(m_key, m_x, m_y);
		}
	private:
		int m_key;
		unsigned m_x, m_y;
	};	

	// Рассылаем событие всем слушателям
	m_pApplication->DispatchEvent(CSpecialKeyEvent(key, x, y));
}

void CGLApplication::KeyboardHandler(unsigned char key, int x, int y)
{
	// Событие о нажатии клавиши на клавиатуре
	class CKeyboardEvent
	{
	public:
		CKeyboardEvent(unsigned char key, int x, int y)
			:m_key(key), m_x(x), m_y(y)
		{
		}
		// Оператор () перенаправляет событие в метод OnKeyboard слушателя
		void operator()(IApplicationListener * pListener)const
		{
			pListener->OnKeyboard(m_key, m_x, m_y);
		}
	private:
		unsigned char m_key;
		unsigned m_x, m_y;
	};

	// Рассылаем событие всем слушателям
	m_pApplication->DispatchEvent(CKeyboardEvent(key, x, y));
}

void CGLApplication::PostRedisplay()
{
	// Инициируем перерисовку калпа
	glutPostRedisplay();
}

void CGLApplication::SetTimer(int milliseconds, TimerProc proc, int value)
{
	// Устанавливаем таймер
	glutTimerFunc(milliseconds, proc, value);
}

void CGLApplication::MainLoop(void)
{
	// Вызываем обработчик OnInit, который может быть 
	// перегружен в классе-наследнике
	OnInit();
	// И переходим в цикл обработки сообщений GLUT
	glutMainLoop();
}

// Данный метод может быть перегружен классами-наследниками
void CGLApplication::OnInit()
{

}

CGLApplication::~CGLApplication(void)
{
}
