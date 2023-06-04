#define _USE_MATH_DEFINES
#include "MyApplication.h"

CMyApplication::CMyApplication(const char* title)
	: CGLApplication(title)
{
}

CMyApplication::~CMyApplication(void)
{
}

void CMyApplication::OnInit()
{
	// Задаем цвет очистки буфера кадра
	glClearColor(1, 1, 0, 1);
}

void CMyApplication::OnDisplay(void)
{
	// Очищаем буфер цвета и буфер глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Настраиваем цвет вершин
	glColor3f(0, 0, 1);

	// Рисуем закрашенный эллипс
	FillEllipse(150, 120, 100, 90);
}

void CMyApplication::OnReshape(int width, int height)
{
	// Настраиваем порт просмотра
	glViewport(0, 0, width, height);

	// Делаем текущей матрицу проецирования и настраиваем ее параметры
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);

	// Делаем текущей матрицей матрицу моделирования-вида
	glMatrixMode(GL_MODELVIEW);
}

// Рисуем закрашенный эллипс
void CMyApplication::FillEllipse(float xCenter, float yCenter, float rx, float ry, int points)
{
	const float step = float(2 * M_PI / points);

	// Эллипс представлет в виде "веера" из треугольников
	glBegin(GL_TRIANGLE_FAN);
	// Начальная точка веера располагается в центре эллипса
	glVertex2d(xCenter, yCenter);
	// Остальные точки - равномерно по его границе
	for (float angle = 0; angle <= 2 * M_PI; angle += step)
	{
		float a = (fabsf(angle - (float)(2 * M_PI)) < 1e-5) ? 0 : angle;
		const float dx = rx * cosf(a);
		const float dy = ry * sinf(a);
		glVertex2f(dx + xCenter, dy + yCenter);
	}
	glEnd();
}
