#include <GLFW/glfw3.h>
#include <memory>
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

using GLFWwindowPtr = std::unique_ptr<GLFWwindow, decltype([](GLFWwindow* w) {
	if (w)
	{
		glfwDestroyWindow(w);
	}
})>;

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

	virtual ~BaseWindow() = default;

	void Run()
	{
		glfwMakeContextCurrent(m_window.get());
		while (!glfwWindowShouldClose(m_window.get()))
		{
			int w, h;
			glfwGetFramebufferSize(m_window.get(), &w, &h);
			Draw(w, h);
			glfwSwapBuffers(m_window.get());
			glfwPollEvents();
		}
	}

private:
	virtual void Draw(int width, int height) = 0;

	static GLFWwindowPtr CreateWindow(int w, int h, const char* title)
	{
		return GLFWwindowPtr{ glfwCreateWindow(w, h, title, nullptr, nullptr) };
	}
	GLFWwindowPtr m_window;
};

class Window : public BaseWindow
{
public:
	using BaseWindow::BaseWindow;

private:
	void Draw(int width, int height) override
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, width, height);

		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.8f, -0.8f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.8f, -0.8f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.0f, 0.8f);
		glEnd();
	}
};

int main()
{
	GLFWInitializer initGLFW;
	Window window{ 800, 600, "Hello, triangle" };
	window.Run();
}
