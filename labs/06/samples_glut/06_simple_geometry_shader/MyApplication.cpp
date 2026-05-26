#include "StdAfx.h"
#include "MyApplication.h"
#include "../libgl/ShaderLoader.h"
#include "../libgl/ShaderCompiler.h"
#include "../libgl/ProgramLinker.h"
#include "../libgl/TextureLoader.h"

CMyApplication::CMyApplication(const char * title, int width, int height)
:CGLApplication(title, width, height)
{
}

CMyApplication::~CMyApplication(void)
{
}

void CMyApplication::OnInit()
{
	InitShaders();
	InitTextures();
}

void CMyApplication::InitTextures()
{
	// Загружаем текстуру
	CTextureLoader loader;
	loader.BuildMipmaps(true);
	loader.SetMagFilter(GL_LINEAR);
	loader.SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	m_texture = loader.LoadTexture2D(L"car.jpg");
}

void CMyApplication::InitShaders()
{
	// Проверяем поддержку геометрических шейдеров видеокартой
	if (!GLEW_EXT_geometry_shader4)
	{
		throw std::runtime_error(
			"The OpenGL implementation does not support geometry shaders");
	}


	CShaderLoader loader;
	// Загружаем шейдеры
	m_vertexShader = 
		loader.LoadShader(GL_VERTEX_SHADER, "vertex_shader.vsh");
	m_fragmentShader = 
		loader.LoadShader(GL_FRAGMENT_SHADER, "fragment_shader.fsh");
	m_geometryShader = 
		loader.LoadShader(GL_GEOMETRY_SHADER, "geometry_shader.gsh");

	// Создаем программный объект и присоединяем шейдеры к нему
	m_program.Create();
	m_program.AttachShader(m_vertexShader);
	m_program.AttachShader(m_fragmentShader);
	m_program.AttachShader(m_geometryShader);

	// Компилируем шейдеры
	CShaderCompiler compiler;
	compiler.CompileShader(m_vertexShader);
	compiler.CompileShader(m_geometryShader);
	compiler.CompileShader(m_fragmentShader);
	compiler.CheckStatus();

	// Задаем параметры шейдерной программы
	// тип входных примитивов: точки
	m_program.SetParameter(GL_GEOMETRY_INPUT_TYPE_ARB, GL_POINTS);
	// типв выходных примитивов: лента из треугольникков
	m_program.SetParameter(GL_GEOMETRY_OUTPUT_TYPE_ARB, GL_TRIANGLE_STRIP);
	// Максимальное количество вершин, порождаемых геометрическим шейдером
	// за один вызов. Для ленты из двух треугольников оно равно 4
	m_program.SetParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 4);

	// Компонуем программу и проверяем ее статус
	CProgramLinker linker;
	linker.LinkProgram(m_program);
	linker.CheckStatus();

	// Получаем расположение uniform-переменных, используемых в 
	// шейдерной программе
	m_sizeLocation = m_program.GetUniformLocation("Size");
	m_textureMapLocation = m_program.GetUniformLocation("TextureMap");
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Активируем шейдерную программу и задаем значения ее uniform-переменных
	glUseProgram(m_program);
	if (m_sizeLocation >= 0)
	{
		// размеры точки в нормализованных координатах
		glUniform2f(m_sizeLocation, 0.6f, 0.6f);
	}
	if (m_textureMapLocation >= 0)
	{
		// задаем номер текстурного модуля 0 для использования дискретизатором
		// TextureMap
		glUniform1i(m_textureMapLocation, 0);
	}

	// Привязываемся к загруженной текстуре в 0 текстурном модуле
	glBindTexture(GL_TEXTURE_2D, m_texture);

	// Рисуем две точки в виде текстурированных прямоугольников
	glBegin(GL_POINTS);
		glVertex2f(-0.5, -0.5);
		glVertex2f(0.5, 0.5);
	glEnd();

	// Переключаемся на стандартный конвейер
	glUseProgram(0);
}

void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);
}