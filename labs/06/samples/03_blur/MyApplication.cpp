#include "pch.h"
#include "MyApplication.h"
#include "../libgl/ProgramInfo.h"
#include "../libgl/ProgramLinker.h"
#include "../libgl/ShaderCompiler.h"
#include "../libgl/ShaderLoader.h"
#include "../libgl/TextureLoader.h"

// Радиус фильтра (должен совпадать с радиусом фильтра в шейдере)
const int CMyApplication::BLUR_RADIUS = 10;

CMyApplication::CMyApplication(const char* title, int width, int height)
	: CGLApplication(title, width, height)
	, m_blur(true)
	, m_sigma(3)
	, m_textureWidth(0)
	, m_textureHeight(0)
{
}

void CMyApplication::OnInit()
{
	if (!GLEW_EXT_framebuffer_object)
	{
		throw std::runtime_error("GL_EXT_framebuffer_object extension is not available");
	}

	if (!GLEW_ARB_shader_objects)
	{
		throw std::runtime_error("GLEW_ARB_shader_objects extension is not available");
	}

	LoadTextures();
	LoadShaders();
}

void CMyApplication::OnDisplay()
{
	// Очищаем буфер цвета
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	CTexture2DHandle image;

	// Опционально применяем фильтр размытия к текстуре
	if (m_blur)
	{
		image = m_blurFilter.Blur(
			m_texture,
			m_sigma,
			m_textureWidth,
			m_textureHeight,
			false);
	}
	else
	{
		image = m_texture;
	}

	// Выводим прямоугольник с текстурой размытого либо оригинального
	// изображения
	image.Bind();
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex2i(0, 0);

		glTexCoord2f(0, 1);
		glVertex2i(0, m_textureHeight);

		glTexCoord2f(1, 1);
		glVertex2i(m_textureWidth, m_textureHeight);

		glTexCoord2f(1, 0);
		glVertex2i(m_textureWidth, 0);
	}
	glEnd();
}

void CMyApplication::LoadShaders()
{
	CShaderLoader loader;
	CShader vertexShader = loader.LoadShader(GL_VERTEX_SHADER, "blur.vsh");
	CShader fragmentShader = loader.LoadShader(GL_FRAGMENT_SHADER, "blur.fsh");

	m_blurProgram.Create();
	m_blurProgram.AttachShader(vertexShader);
	m_blurProgram.AttachShader(fragmentShader);

	CShaderCompiler compiler;
	compiler.CompileShader(vertexShader);
	compiler.CompileShader(fragmentShader);

	compiler.CheckStatus();

	CProgramLinker linker;
	linker.LinkProgram(m_blurProgram);
	linker.CheckStatus();

	CProgramInfo info(m_blurProgram);
	info.Print(std::cout);

	// Получаем расположение uniform-переменных шейдерной программы
	GLint imageLocation = m_blurProgram.GetUniformLocation("image");
	GLint stepLocation = m_blurProgram.GetUniformLocation("step");
	GLint coefficientsLocation = m_blurProgram.GetUniformLocation("coefficients");
	// Выполняем инициализацию фильтра размытия
	m_blurFilter.SetProgram(
		m_blurProgram,
		stepLocation,
		imageLocation,
		coefficientsLocation,
		BLUR_RADIUS);
}

void CMyApplication::LoadTextures()
{
	CTextureLoader loader;
	loader.SetMagFilter(GL_NEAREST);
	loader.SetMinFilter(GL_NEAREST);
	loader.SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	m_texture = loader.LoadTexture2D(L"car.jpg", 0, 0, &m_textureWidth, &m_textureHeight);
}

void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	// Устанавливаем матрицу ортографического проекцирования
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	glMatrixMode(GL_MODELVIEW);
}

void CMyApplication::OnMouse(int button, int state, int x, int y)
{
	// Включаем/выключаем применение фильтра по щелчку
	// левой кнопкой мыши
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		m_blur = !m_blur;
		PostRedisplay();
	}
}

void CMyApplication::OnSpecialKey(int key, int x, int y)
{
	// Управляем величиной дисперсии фильтра Гаусса
	// при помощи клавиш управления курсором
	bool refresh = false;
	switch (key)
	{
	case GLUT_KEY_UP:
		m_blur = true;
		m_sigma = std::min(m_sigma + 0.1f, static_cast<float>(BLUR_RADIUS));
		refresh = true;
		break;
	case GLUT_KEY_DOWN:
		m_blur = true;
		m_sigma = std::max(m_sigma - 0.1f, 0.1f);
		refresh = true;
		break;
	}

	// Обновляем экран при изменении величины дисперсии
	if (refresh)
	{
		PostRedisplay();
	}
}
