#include "pch.h"
#include "MyApplication.h"

const double CMyApplication::FIELD_OF_VIEW = 60;
const double CMyApplication::ZNEAR = 1;
const double CMyApplication::ZFAR = 10;
const double CMyApplication::X_ROTATION_SPEED = 43;
const double CMyApplication::Y_ROTATION_SPEED = -37;
const double CMyApplication::Z_ROTATION_SPEED = 29;

const GLuint CMyApplication::FRAME_BUFFER_WIDTH = 800;
const GLuint CMyApplication::FRAME_BUFFER_HEIGHT = 600;

CMyApplication::CMyApplication(const char* title, int width, int height)
	: CGLApplication(title, width, height)
	, m_frameBuffer(0)
	, m_depthBuffer(0)
	, m_windowWidth(width)
	, m_windowHeight(height)
	, m_rotationController(width, height)
{
	// Добавляем контроллер вращения к списку слушателей приложения
	AddEventListener(&m_rotationController);

	// В то же время, будем слушать события от контроллера вращения
	m_rotationController.AddEventListener(this);
}

CMyApplication::~CMyApplication()
{
	// Удаляем буфер глубины вспомогательного буфера кадра
	if (m_depthBuffer)
	{
		glDeleteRenderbuffersEXT(1, &m_depthBuffer);
	}

	// Удаляем вспомогательный буфер кадра
	if (m_frameBuffer)
	{
		glDeleteFramebuffersEXT(1, &m_frameBuffer);
	}
}

void CMyApplication::OnRotationControllerUpdate()
{
	// Перерисовываем сцену при обновлении
	// состояния контроллера вращения
	PostRedisplay();
}

void CMyApplication::OnInit()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 1);
	glEnable(GL_CULL_FACE);

	// Загружаем матрицу камеры в матрицу моделирования-вида
	// вспомогательного буфера кадра
	m_modelViewFrameBuffer.LoadLookAtRH(
		0, 0, 5,
		0, 0, 0,
		0, 1, 0);

	// Инициализируем матрицу моделирования вида основного буфера кадра
	CMatrix4d modelView;
	modelView.LoadLookAtRH(
		0, 0, 4,
		0, 0, 0,
		0, 1, 0);
	// и загружаем ее в контроллер вращения
	m_rotationController.SetModelViewMatrix(modelView);

	// Инициализируем вспомогательный буфер кадра
	InitFrameBuffer();
}

void CMyApplication::OnDisplay()
{
	// Рисуем объект в текстуру
	DrawObjectToTexture();

	// Привязываемся к основному буферу кадра
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// Очищаем основной буфер кадра
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Обновляем матрицу проецирования и видовой порт
	SetupViewportAndProjectionMatrix(
		m_windowWidth, m_windowHeight,
		FIELD_OF_VIEW, ZNEAR, ZFAR);

	// Загружаем матрицу моделирования-вида, получив ее
	// у контроллера вращения
	glLoadMatrixd(m_rotationController.GetModelViewMatrix());

	// Выбираем текстуру, используемую в качестве буфера цвета
	// во вспомогательном буфере кадра
	glEnable(GL_TEXTURE_2D);
	m_colorBufferTexture.Bind();

	// Рисуем прямоугольник
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex2f(-2, -1.5);

		glTexCoord2f(1, 0);
		glVertex2f(2, -1.5);

		glTexCoord2f(1, 1);
		glVertex2f(2, 1.5);

		glTexCoord2f(0, 1);
		glVertex2f(-2, 1.5);
	}
	glEnd();
}

void CMyApplication::DrawObjectToTexture() const
{
	// Привязываемся к текстурному объекту с идентификатором 0
	// и запрещаем наложение текстуры
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	// Делаем вспомогательный буфер активным
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBuffer);

	// Очищаем связанные с ним буферы цвета и глубины
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Настраиваем матрицу проецирования для рендеринга
	// во вспомогательный буфер кадра
	SetupViewportAndProjectionMatrix(
		FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT,
		FIELD_OF_VIEW,
		ZNEAR, ZFAR);

	// Загружаем матрицу моделирования вида для
	// рисования во вспомогательный буфер
	glLoadMatrixd(m_modelViewFrameBuffer);

	// Рисуем тор во вспомогательный буфер
	glutWireTorus(0.2, 2, 20, 50);

	// Привязываемся к текстуре, присоединенной к буферу цвета
	// вспомогательного буфера кадра
	m_colorBufferTexture.Bind();
	// И генерируем mip-уровни
	glGenerateMipmapEXT(GL_TEXTURE_2D);
}

void CMyApplication::OnIdle()
{
	m_animationController.Tick();
	double delta = m_animationController.GetTimeDelta() * 0.001;

	// Осуществляем вращение объекта, выводимого во вспомогательный
	// буфер кадра, вокруг осей X, Y и Z
	m_modelViewFrameBuffer.Rotate(delta * X_ROTATION_SPEED, 1, 0, 0);
	m_modelViewFrameBuffer.Rotate(delta * Y_ROTATION_SPEED, 0, 1, 0);
	m_modelViewFrameBuffer.Rotate(delta * Z_ROTATION_SPEED, 0, 0, 1);
	m_modelViewFrameBuffer.Normalize();

	// Иниициируем перерисовку кадра
	PostRedisplay();
}

void CMyApplication::InitFrameBuffer()
{
	// Инициализируем текстуру, используемую для хранения
	// буфера цвета вспомогательного буфера кадра
	m_colorBufferTexture.Create();
	m_colorBufferTexture.Bind();
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);

	// Задаем уровень анизотропной фильтрации для текстуры
	// (при поддержке со стороны OpenGL)
	if (GL_EXT_texture_filter_anisotropic)
	{
		GLint maxAnisotropy = 0;
		glGetIntegerv(
			GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
			&maxAnisotropy);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAX_ANISOTROPY_EXT,
			maxAnisotropy);
	}

	// Создаем текстуру заданного размера, не инициализируя ее содержимое
	// (параметр pData) равен NULL
	m_colorBufferTexture.TexImage(
		0,
		GL_RGBA,
		FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT,
		0, GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);
	// Автоматически генерируем уровни детализации для текстуры
	// (данная функция вошла в состав расширения GL_EXT_framebuffer_object)
	glGenerateMipmapEXT(GL_TEXTURE_2D);

	// Генерируем имя для буфера кадра
	glGenFramebuffersEXT(1, &m_frameBuffer);
	// Выполняем привязку ко вспомогательному буферу кадра
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBuffer);
	// Присоединяем 0 уровень детализации текстуры в качестве буфера цвета №0
	glFramebufferTexture2DEXT(
		GL_FRAMEBUFFER_EXT, // тип буфера кадра
		GL_COLOR_ATTACHMENT0_EXT, // точка присоединения
		GL_TEXTURE_2D, // тип текстуры
		m_colorBufferTexture, // текстурный объект
		0); // mip-уровень

	// Генерируем имя для буфера визуализации
	glGenRenderbuffersEXT(1, &m_depthBuffer);
	// Выполняем привязку к буферу визуализации
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuffer);
	// Выделяем в буфере визуализации память для хранения
	// двухмерного массива пикселей глубины
	glRenderbufferStorageEXT(
		GL_RENDERBUFFER_EXT,
		GL_DEPTH_COMPONENT,
		FRAME_BUFFER_WIDTH,
		FRAME_BUFFER_HEIGHT);
	// Присоединяем буфер визуализации в качестве буфера глубины
	glFramebufferRenderbufferEXT(
		GL_FRAMEBUFFER_EXT, // цель использования буфера кадра
		GL_DEPTH_ATTACHMENT_EXT, // точка присоединения
		GL_RENDERBUFFER_EXT, // цель использования буфера визуализации
		m_depthBuffer); // буфер визуализации

	// Проверяем статус буфера кадра
	GLenum frameBufferStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	// Буфер кадра должен быть полным
	// (содержать необходимый набор изображений)
	if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		throw std::logic_error("Framebuffer is incomplete");
	}

	// Переключаемся на использование основного буфера кадра
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CMyApplication::OnReshape(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;
}

void CMyApplication::SetupViewportAndProjectionMatrix(
	int width, int height,
	double fov, double znear, double zfar)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, znear, zfar);
	glMatrixMode(GL_MODELVIEW);
}
