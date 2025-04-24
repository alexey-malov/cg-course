#include "StdAfx.h"
#include "MyApplication.h"
#include "ProgramInfo.h"
#include "ProgramLinker.h"
#include "ShaderCompiler.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"

// Угол обзора по вертикали
const double CMyApplication::FIELD_OF_VIEW = 60;

const double CMyApplication::ZNEAR = 1;

const double CMyApplication::ZFAR = 5;

CMyApplication::CMyApplication(const char* title, int width, int height)
	: CGLApplication(title, width, height)
	, m_mainTextureSamplerLocation(-1)
	, m_cloudsTextureSamplerLocation(-1)
	, m_rotationController(width, height)
	, m_sphere(1, 30, 20)
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
	LoadTextures();
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Основная текстура выбрана в Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	m_sphereTexture.Bind();

	// Текстура облаков выбрана в Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	m_cloudsTexture.Bind();

	glUseProgram(m_program);

	// Передаём в шейдер номер текстурного блока, к которому привязана текстура
	glUniform1i(m_mainTextureSamplerLocation, 0);
	glUniform1i(m_cloudsTextureSamplerLocation, 1);
	
	m_sphere.Draw();

	glUseProgram(0);
}

void CMyApplication::LoadTextures()
{
	CTextureLoader loader;
	m_sphereTexture = loader.LoadTexture2D(L"earth.jpg");
	m_cloudsTexture = loader.LoadTexture2D(L"clouds.jpg");
}

void CMyApplication::InitShaders()
{
	// Загружаем шейдеры
	CShaderLoader loader;
	CShader vertexShader = loader.LoadShader(GL_VERTEX_SHADER, L"texture_mapping.vsh");
	CShader fragmentShader = loader.LoadShader(GL_FRAGMENT_SHADER, L"texture_mapping.fsh");

	// Создаем программы и присоединяем к ней шейдеры
	m_program.Create();
	m_program.AttachShader(vertexShader);
	m_program.AttachShader(fragmentShader);

	// Компилируем шейдеры
	CShaderCompiler compiler;
	compiler.CompileShader(vertexShader);
	compiler.CompileShader(fragmentShader);
	compiler.CheckStatus();

	// Компонуем программу
	CProgramLinker linker;
	linker.LinkProgram(m_program);
	linker.CheckStatus();

	// Выводим информацию о программе
	CProgramInfo programInfo(m_program);
	programInfo.Print(std::cout);

	// Получаем и сохраняем расположение uniform-переменной mainTexture
	m_mainTextureSamplerLocation = m_program.GetUniformLocation("mainTexture");
	m_cloudsTextureSamplerLocation = m_program.GetUniformLocation("cloudsTexture");
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

inline void CMyApplication::OnKeyboard(unsigned char key, int, int)
{
	if (key == 'L' || key == 'l')
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
	}
	else if (key == 'F' || key == 'f')
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glutPostRedisplay();
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
