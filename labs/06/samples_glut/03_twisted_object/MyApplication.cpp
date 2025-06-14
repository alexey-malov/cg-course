#include "StdAfx.h"
#include "MyApplication.h"
#include "../libgl/ShaderLoader.h"
#include "../libgl/ShaderCompiler.h"
#include "../libgl/ProgramLinker.h"
#include "../libgl/ProgramInfo.h"



// Угол обзора по вертикали
const double CMyApplication::FIELD_OF_VIEW = 60;

const double CMyApplication::ZNEAR = 1;

const double CMyApplication::ZFAR = 5;

CMyApplication::CMyApplication(const char * title, int width, int height)
:CGLApplication(title, width, height)
,m_twistLocation(-1)
,m_rotationController(width, height)
{
}

CMyApplication::~CMyApplication(void)
{
}

void CMyApplication::OnInit()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1);
	glColor3f(1, 1, 1);
	
	glLoadIdentity();
	gluLookAt(
		0, 0, 3, 
		0, 0, 0, 
		0, 1, 0);

	InitShaders();

}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Подключаем шейдерную программу
	glUseProgram(m_program);

	// Устанавливаем значение uniform-переменной TWIST
	glUniform1f(m_twistLocation, 1.0f);

	glutWireTorus(0.3, 1, 20, 50);

	glUseProgram(0);
}

void CMyApplication::InitShaders()
{
	CShaderLoader loader;

	// Загружаем вершинный шейдер
	CShader vertexShader = loader.LoadShader(GL_VERTEX_SHADER, L"twist.vsh");

	// Создаем программный объект и присоединяем к нему шейдер
	m_program.Create();
	m_program.AttachShader(vertexShader);

	// Запускаем компиляцию шейдеров
	CShaderCompiler compiler;
	compiler.CompileShader(vertexShader);
	compiler.CheckStatus();

	// Запускаем компоновку программы
	CProgramLinker linker;
	linker.LinkProgram(m_program);
	linker.CheckStatus();

	// Выводим информацию о программе
	CProgramInfo programInfo(m_program);
	programInfo.Print(std::cout);

	// Получаем и сохраняем расположение uniform-переменной TWIST
	m_twistLocation = m_program.GetUniformLocation("TWIST");
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

