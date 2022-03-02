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
		m_startTime = std::chrono::steady_clock::now();
	}

	void Draw(int width, int height) override
	{
		using namespace std::chrono;
		const auto currentTime = steady_clock::now();
		const auto timeSinceStart = currentTime - m_startTime;
		const auto phase = duration_cast<duration<double>>(timeSinceStart % ANIMATION_PERIOD) / ANIMATION_PERIOD;

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, width, height);
		SetupProjectionMatrix(width, height);
		SetupModelViewMatrix(phase);

		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.8f, -0.8f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.8f, -0.8f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.0f, 0.8f);
		glEnd();
	}

	static void SetupProjectionMatrix(int width, int height)
	{
		// ¬ычисл€ет матрицу ортографического преобразовани€ такую, чтобы вписать квадратную область
		// [-1;+1] по обеим ос€м внутрь видового порта размером width*height пикселей
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
	// ѕериод вращени€ треугольника (за какое врем€ треугольник делает полный оборот)
	const std::chrono::milliseconds ANIMATION_PERIOD = std::chrono::milliseconds{ 8000 };
};

int main()
{
	GLFWInitializer initGLFW;
	Window window{ 800, 600, "Hello, triangle" };
	window.Run();
}
