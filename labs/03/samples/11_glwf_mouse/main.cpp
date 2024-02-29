#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/mat4x4.hpp>
#include <iostream>
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

struct SizeI
{
	int width = 0;
	int height = 0;
};

struct PointD
{
	double x = 0;
	double y = 0;
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
		glfwSetWindowUserPointer(m_window, this);
		glfwSetWindowRefreshCallback(m_window, &BaseWindow::RefreshCallback);
		glfwSetCursorPosCallback(m_window, &BaseWindow::CursorPosCallback);
	}
	BaseWindow(const BaseWindow&) = delete;
	BaseWindow& operator=(const BaseWindow&) = delete;

	virtual ~BaseWindow()
	{
		glfwDestroyWindow(m_window);
	}

	SizeI GetFramebufferSize() const
	{
		SizeI size;
		glfwGetFramebufferSize(m_window, &size.width, &size.height);
		return size;
	}

	void Run()
	{
		glfwMakeContextCurrent(m_window);
		OnRunStart();
		while (!glfwWindowShouldClose(m_window))
		{
			int w, h;
			glfwGetFramebufferSize(m_window, &w, &h);
			//Draw(w, h);
			//glfwPollEvents();
			glfwWaitEvents();
		}
		OnRunEnd();
	}

	void SwapBuffers()
	{
		glfwSwapBuffers(m_window);
	}

private:
	virtual void Draw(int width, int height) = 0;

	virtual void OnRunStart() {}
	virtual void OnRunEnd() {}
	virtual void OnRefresh() {}
	virtual void OnCursorPos([[maybe_unused]] double x, [[maybe_unused]] double y)
	{
	}

	static BaseWindow* GetInstance(GLFWwindow* window)
	{
		return static_cast<BaseWindow*>(glfwGetWindowUserPointer(window));
	}

	static void RefreshCallback(GLFWwindow* window)
	{
		if (auto instance = GetInstance(window))
		{
			instance->OnRefresh();
		}
	}

	static void CursorPosCallback(GLFWwindow* window, double x, double y)
	{
		if (auto instance = GetInstance(window))
		{
			instance->OnCursorPos(x, y);
		}
	}

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
#if 0
		SetupModelViewMatrix(phase);

		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.8f, -0.8f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.8f, -0.8f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.0f, 0.8f);
		glEnd();
#endif
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

#if 0
	static void SetupModelViewMatrix(double phase)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotated(phase * 360, 0.0, 0.0, 1.0);
	}
#endif

	std::chrono::steady_clock::time_point m_startTime;
	// ѕериод вращени€ треугольника (за какое врем€ треугольник делает полный оборот)
	const std::chrono::milliseconds ANIMATION_PERIOD = std::chrono::milliseconds{ 8000 };

private:
	void OnRefresh() override
	{
		auto size = GetFramebufferSize();
		Draw(size.width, size.height);
		std::cout << "On Refresh" << std::endl;
		SwapBuffers();
	}

	void OnCursorPos(double x, double y) override
	{
		double mv[4][4];
		glm::dmat4x4 proj, modelView;
		glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
		glGetDoublev(GL_MODELVIEW_MATRIX, &modelView[0][0]);

		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		auto wndSize = GetFramebufferSize();

		glm::dvec4 cursorNorm = {
			(x - viewport[0]) / (viewport[2] / 2) - 1.0,
			(wndSize.height - y - viewport[1]) / (viewport[3] / 2) - 1.0,
			0.0,
			1.0
		};

		auto mvp = proj * modelView;
		auto mvpInvertse = inverse(mvp);
		glm::dvec4 cursorPoint{ x, y, 0, 1 };
		auto cursorInModelSpace = mvpInvertse * cursorNorm;
		cursorInModelSpace.x /= cursorInModelSpace.w;
		cursorInModelSpace.y /= cursorInModelSpace.w;
		std::cout << "On cursor pos: " << x << ", " << y
				  << " ( " << cursorInModelSpace.x << ","
				  << cursorInModelSpace.y << ")" << std::endl;
	}
};

int main()
{
	GLFWInitializer initGLFW;
	Window window{ 800, 600, "Hello, triangle" };
	window.Run();
}
