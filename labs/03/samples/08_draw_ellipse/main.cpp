#define _USE_MATH_DEFINES
#include <cmath>
//
#include <GLFW/glfw3.h>
#include <chrono>
#include <stdexcept>

class GLFWInitializer final
{
public:
	GLFWInitializer()
	{
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initialize GLFW");
		}
	}

	GLFWInitializer(const GLFWInitializer&) = delete;
	GLFWInitializer& operator=(const GLFWInitializer&) = delete;

	~GLFWInitializer()
	{
		glfwTerminate();
	}
};

class BaseWindow
{
public:
	BaseWindow(int w, int h, const char* title)
		: m_window{ CreateWindow(w, h, title) }
	{
		if (!m_window)
		{
			throw std::runtime_error("Failed to create window");
		}
	}
	BaseWindow(const BaseWindow&) = delete;
	BaseWindow& operator=(const BaseWindow&) = delete;

	virtual ~BaseWindow()
	{
		glfwDestroyWindow(m_window);
	}

	void Run()
	{
		glfwMakeContextCurrent(m_window);
		OnRunStart();
		while (!glfwWindowShouldClose(m_window))
		{
			int w, h;
			glfwGetFramebufferSize(m_window, &w, &h);
			Draw(w, h);
			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}
		OnRunEnd();
	}

private:
	virtual void Draw(int width, int height) = 0;

	virtual void OnRunStart() {}
	virtual void OnRunEnd() {}

	static GLFWwindow* CreateWindow(int w, int h, const char* title)
	{
		return glfwCreateWindow(w, h, title, nullptr, nullptr);
	}
	GLFWwindow* m_window;
};

class Window : public BaseWindow
{
public:
	using BaseWindow::BaseWindow;

private:
	void OnRunStart() override
	{
		// Задаем цвет очистки буфера кадра
		glClearColor(1, 1, 0, 1);
	}

	void Draw(int width, int height) override
	{
		// Настраиваем порт просмотра
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);

		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0, 0, 1);

		FillEllipse(150, 120, 100, 90);
	}

	static void FillEllipse(float xCenter, float yCenter, float rx, float ry, int points = 360)
	{
		const float step = static_cast<float>(2 * M_PI / points);

		// Эллипс представлет в виде "веера" из треугольников
		glBegin(GL_TRIANGLE_FAN);
		// Начальная точка веера располагается в центре эллипса
		glVertex2d(xCenter, yCenter);
		// Остальные точки - равномерно по его границе
		for (float angle = 0; angle <= 2 * M_PI; angle += step)
		{
			float a = (fabsf(static_cast<float>(angle - 2 * M_PI)) < 1e-5) ? 0 : angle;
			const float dx = rx * cosf(a);
			const float dy = ry * sinf(a);
			glVertex2f(dx + xCenter, dy + yCenter);
		}
		glEnd();
	}

	static void SetupProjectionMatrix(int width, int height)
	{
		// Вычисляет матрицу ортографического преобразования такую, чтобы вписать квадратную область
		// [-1;+1] по обеим осям внутрь видового порта размером width*height пикселей
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		const double aspectRatio = double(width) / double(height);
		double viewWidth = 2.0;
		double viewHeight = viewWidth;
		if (aspectRatio > 1.0)
		{
			viewWidth = viewHeight * aspectRatio;
		}
		else
		{
			viewHeight = viewWidth / aspectRatio;
		}
		glOrtho(-viewWidth * 0.5, viewWidth * 0.5, -viewHeight * 0.5, viewHeight * 0.5, -1.0, 1.0);
	}

	static void SetupModelViewMatrix(double phase)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotated(phase * 360, 0.0, 0.0, 1.0);
	}

	std::chrono::steady_clock::time_point m_startTime;
	// Период вращения треугольника (за какое время треугольник делает полный оборот)
	const std::chrono::milliseconds ANIMATION_PERIOD = std::chrono::milliseconds{ 8000 };
};

int main()
{
	GLFWInitializer initGLFW;
	Window window{ 800, 600, "Hello, ellipse" };
	window.Run();
}
