#include "MyApplication.h"
#include "../libgl/TextureLoader.h"

// Расстояние до ближней плоскости отсечения отображаемого объема
const double CMyApplication::ZNEAR = 0.5;
// Расстояние до дальей плоскости отсечения отображаемого объема
const double CMyApplication::ZFAR = 10;
// Угол обзора по вертикали
const double CMyApplication::FIELD_OF_VIEW = 60;

const float CMyApplication::ROTATION_SPEED = 40;

CMyApplication::CMyApplication(const char* title, int width, int height)
	: CGLApplication(title, width, height)
	, m_rotationAngle(0)
{
}

void CMyApplication::OnInit()
{
	glewInit();
	// Проверяем наличие расширения GL_ARB_multitexture
	if (!GLEW_ARB_multitexture)
	{
		throw std::runtime_error("GL_ARB_multitexture extension is not supported");
	}

	CTextureLoader textureLoader;
	m_car.Attach(textureLoader.LoadTexture2D(L"assets/car.jpg"));
	m_smile.Attach(textureLoader.LoadTexture2D(L"assets/smile.png"));

	textureLoader.SetWrapMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	m_sun.Attach(textureLoader.LoadTexture2D(L"assets/sun.png"));

	m_medved.Attach(textureLoader.LoadTexture2D(L"assets/medved.png"));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1);
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0, -2, 4, 0, 0, 0, 0, 1, 0);

	/*
	Задаем текстуры для текстурного блока №0
	*/
	glActiveTexture(GL_TEXTURE0);
	m_car.Bind();

	// Включаем текстурирование только в первом текстурном блоке
	// и рисуем прямоугольник
	{
		glActiveTexture(GL_TEXTURE0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);

		glPushMatrix();

		glTranslatef(-1.5f, 1.5f, 0);
		glColor3f(0.5, 1, 1);

		DrawRectangle();

		glPopMatrix();
	}

	// Включаем текстурирование в первом и втором текстурных блоках
	// и рисуем прямоугольник
	// Второй текстурный блок работает в режиме GL_DECAL
	{
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		m_medved.Bind();
		glPushMatrix();
		glTranslatef(+1.5f, 1.5f, 0);
		DrawRectangle();
		glPopMatrix();
	}

	// Включаем текстурирование в обоих текстурных блоках
	// и рисуем прямоугольник
	// Второй текстурный блок работает в режиме GL_MODULATE
	{
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1);
		m_smile.Bind();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(-1.5f, -1.0f, 0);
		DrawRectangle();
		glPopMatrix();
	}

	/*
	Рисуем прямоугольник, применяя к одному из текстурных блоков
	трансформацию текстурных координат
	*/
	{
		glActiveTexture(GL_TEXTURE1);
		m_sun.Bind();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		// сохраняем матрицу ModelView в стеке матриц и переносим
		// прямоугольник вдоль вектора 1.5, -1.0
		glPushMatrix();
		glTranslatef(1.5f, -1.0f, 0);

		// Активируем текстурный блок №1 и задаем для него матрицу
		// трансформации текстурных координат
		glActiveTexture(GL_TEXTURE1);
		glMatrixMode(GL_TEXTURE);
		// сохраняем текущую матрицу в стеке матриц
		// (в каждом текстурном блоке - свой стек матриц)
		glPushMatrix();
		glLoadIdentity();
		// вращаем вокруг оси Z, проходящей через точку (0.5, 0.5, 0)
		glTranslatef(0.5, 0.5, 0);
		glRotatef(m_rotationAngle, 0, 0, 1);
		glTranslatef(-0.5, -0.5, 0);

		DrawRectangle();

		// восстанавливаем матрицы
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
}

void CMyApplication::OnIdle()
{
	m_animationController.Tick();

	m_rotationAngle += static_cast<float>(m_animationController.GetTimeDelta() * 0.001 * ROTATION_SPEED);
	m_rotationAngle = fmodf(m_rotationAngle, 360);

	Sleep(10);
	PostRedisplay();
}

void CMyApplication::DrawRectangle() const
{
	/*
	Рисуем прямоугольник, указывая для каждой из его вершин текстурные координаты
	Y
	|
	0---|---3
	|   |   |
	|   |   |
	-----+------X
	|   |   |
	|   |   |
	1---|---2
	|
	*/
	glBegin(GL_QUADS);
	{
		// верхний левый угол
		glMultiTexCoord2f(GL_TEXTURE0, 0, 0);
		glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
		glVertex3f(-1, 1, 0);

		// нижний левый угол
		glMultiTexCoord2f(GL_TEXTURE0, 0, 1);
		glMultiTexCoord2f(GL_TEXTURE1, 0, 1);
		glVertex3f(-1, -1, 0);

		// нижний правый угол
		glMultiTexCoord2f(GL_TEXTURE0, 1, 1);
		glMultiTexCoord2f(GL_TEXTURE1, 1, 1);
		glVertex3f(1, -1, 0);

		// верхний правый угол
		glMultiTexCoord2f(GL_TEXTURE0, 1, 0);
		glMultiTexCoord2f(GL_TEXTURE1, 1, 0);
		glVertex3f(1, 1, 0);
	}
	glEnd();
}

void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, aspect, ZNEAR, ZFAR);
	glMatrixMode(GL_MODELVIEW);
}
