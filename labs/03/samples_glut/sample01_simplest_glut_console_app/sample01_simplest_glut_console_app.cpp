// sample01_simplest_glut_console_app.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _USE_MATH_DEFINES
#include <tchar.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGTH = 600;
const char WINDOW_TITLE[] = "My first OpenGL application";

void OnDrawScene();
void OnResizeWindow(int width, int height);
int g_windowWidth = WINDOW_WIDTH;
int g_windowHeight = WINDOW_HEIGTH;

int main(int argc, char* argv[])
{
	// Инициализируем библиотеку GLUT
	glutInit(&argc, argv);

	/*
	Инициализируем формат буфера кадра, используемый OpenGL:
		- 32-битный буфер цвета,
		- наличие буфера глубины
		- двойная буферизация
	*/
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// Задаем начальные размеры окна
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGTH);

	glutCreateWindow(WINDOW_TITLE);

	// Задаем функцию перерисовки кадра
	glutDisplayFunc(&OnDrawScene);

	// Задаем обработчик события изменения размеров окна
	glutReshapeFunc(&OnResizeWindow);

	// Запускаем главный цикл обработки
	glutMainLoop();

	// Сюда мы, к сожалению, уже не вернемся,
	// т.к. glutMainLoop() не возвращает управления и при закрытии окна
	// сразу вызывает функцию exit(), которая завершает работу приложения
	return 0;
}

// Рисуем пятиконечную звезду
void Draw5PointStar(float xCenter, float yCenter, float radius)
{
	static const float STEP = (float)(M_PI * 4 / 5);

	// Начинаем новую группу примитивов (замкнутая ломаная линия)
	glBegin(GL_LINE_LOOP);

	float angle = (float)(- M_PI / 2);

	// Соединяем отрезками прямой линии точки, расположенные на окружности
	// в точках, с углами: -90, 54, 198, 342, 486 (126) градусов
	for (int i = 0; i < 5; ++i, angle += STEP)
	{
		float x = xCenter + radius * cosf(angle);
		float y = yCenter + radius * sinf(angle);

		// Если раскомметировать следующий вызов glColor3f,
		// каждая вершина звезды будет иметь свой цвет
		// glColor3f(i / 5.0f, 1.0f - i / 10.0f, (i + 2) / 7.0f);

		// функция glVertex2f добавляет в текущую группу примитивов
		// точку, лежащую на плоскости z = 0
		// суффикс 2f в названии функции обозначает, что задаются 2 координаты
		// x и y типа GLfloat
		glVertex2f(x, y);
	}

	// Заканчиваем группу примитивов
	glEnd();
}

void OnDrawScene()
{
	/*
	Задаем цвет очистки буфера кадра (желтый)
	*/
	glClearColor(1, 1, 0, 0);

	// Очищаем буфер цвета и буфер глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Делаем текущий цвет вершины синим
	// суффикс 3f в названии функции обозначает, что будут указаны
	// 3 составляющих цвета (r, g, b) в формате float (от 0 до 1).
	glColor3f(0, 0, 1);

	// Рисуем пятиконечную звезду
	Draw5PointStar(300, 200, 120);

	// Обмениваем лицевой и нелицевой буферы кадра местами
	glutSwapBuffers();
}

// Данная функция вызывается при изменении пользователем размеров окна
// а также непосредственно после создания окна
void OnResizeWindow(int width, int height)
{
	g_windowWidth = width;
	g_windowHeight = height;

	/*
	Задаем положение и размеры видового порта (порта просмотра) в буфере кадра
	Вывод примитивов не будет осуществляться за его пределы
	*/
	glViewport(0, 0, width, height);

	// Делаем текущей матрицей OpenGL матрицу проецирования
	glMatrixMode(GL_PROJECTION);

	// Загружаем в нее единичную матрицу
	glLoadIdentity();

	/*
	И умножаем ее на матрицу ортографического проецирования такую,
	что левому верхнему углу просмотра будет соответствовать
	точка (0, 0), правому нижнему - (width, height),
	а сцена будет включать в себя объект, расположенные по глубине в диапазоне
	от -1 (сзади) до +1 (спереди)
	*/
	glOrtho(0, width, height, 0, -1, 1);

	// Делаем текущей матрицей матрицу моделирования-вида
	glMatrixMode(GL_MODELVIEW);
}
