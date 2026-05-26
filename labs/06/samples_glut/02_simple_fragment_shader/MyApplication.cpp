#include "StdAfx.h"
#include "MyApplication.h"
#include "../libgl/ShaderLoader.h"
#include "../libgl/ShaderCompiler.h"
#include "../libgl/ProgramLinker.h"

CMyApplication::CMyApplication(const char * title, int width, int height)
:CGLApplication(title, width, height)
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
	
	// Задаем параметры камеры
	glLoadIdentity();
	gluLookAt(
		0, 0, 3, 
		0, 0, 0, 
		0, 1, 0);

	InitShaders();
}

void CMyApplication::InitShaders()
{
	// Создаем загрузчик шейдеров
	CShaderLoader loader;
	// И загружаем с его помощью вершинный и фрагментный шейдеры
	CShader vertexShader = 
		loader.LoadShader(GL_VERTEX_SHADER, "checker.vsh");
	CShader fragmentShader = 
		loader.LoadShader(GL_FRAGMENT_SHADER, "checker.fsh");

	Sleep(1);
	LARGE_INTEGER tick0;
	QueryPerformanceCounter(&tick0);

	// Создаем компилятор
	CShaderCompiler compiler;
	// и запускаем компиляцию шейдеров
	compiler.CompileShader(vertexShader);
	compiler.CompileShader(fragmentShader);

	LARGE_INTEGER tick1;
	QueryPerformanceCounter(&tick1);

	std::cout << "Compilation duration: " << tick1.QuadPart - tick0.QuadPart << "\n";

	// Создаем программу и присоединяем к ней шейдеры
	m_program.Create();
	m_program.AttachShader(vertexShader);
	m_program.AttachShader(fragmentShader);

	LARGE_INTEGER tick2;
	QueryPerformanceCounter(&tick2);
	std::cout << "Attach shaders duration: " << tick2.QuadPart - tick1.QuadPart << "\n";

	// Проверяем состояние скомпилированности шейдеров.
	// Если хотя бы один из шейдеров скомпилировался с ошибкой
	// будет выброшено исключение
	compiler.CheckStatus();
	// Все нормально, шейдеры скомпилировались без проблем

	LARGE_INTEGER tick3;
	QueryPerformanceCounter(&tick3);
	std::cout << "Compilation checking status duration: " << tick3.QuadPart - tick2.QuadPart << "\n";

	// Создаем компоновщик,
	CProgramLinker linker;
	// компонуем программу с его помощью
	linker.LinkProgram(m_program);

	LARGE_INTEGER tick4;
	QueryPerformanceCounter(&tick4);
	std::cout << "Link duration: " << tick4.QuadPart - tick3.QuadPart << "\n";

	// Проверяем состояние скомпонованности программ
	// Если при компоновке возникла ошибка, то
	// будет выброшено исключение
	linker.CheckStatus();
	// Все нормально

	LARGE_INTEGER tick5;
	QueryPerformanceCounter(&tick5);
	std::cout << "Link checking status duration: " << tick5.QuadPart - tick4.QuadPart << "\n";
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);

	// Рисуем квадрат
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex2f((float)-0.8, (float)-0.8);

		glTexCoord2f(4, 0);
		glVertex2f((float)0.8, (float)-0.8);

		glTexCoord2f(4, 4);
		glVertex2f((float)0.8, (float)0.8);

		glTexCoord2f(0, 4);
		glVertex2f((float)-0.8, (float)0.8);
	}
	glEnd();

	glUseProgram(0);
}

void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-aspect, +aspect, -1, 1, 0, 10);
	glMatrixMode(GL_MODELVIEW);
}

