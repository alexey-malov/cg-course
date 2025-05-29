#pragma once
#include "gl.h"

class BaseWindow
{
public:
	BaseWindow(int w, int h, const char* title);

	BaseWindow(const BaseWindow&) = delete;
	BaseWindow& operator=(const BaseWindow&) = delete;

	virtual ~BaseWindow();

	glm::ivec2 GetFramebufferSize() const;

	void Run();

	glm::dvec2 GetCursorPos() const;

private:
	static BaseWindow* GetBaseWindow(GLFWwindow* window);

	virtual void OnResize(
		[[maybe_unused]] int width, [[maybe_unused]] int height) {}
	virtual void OnMouseButton(
		[[maybe_unused]] int button, [[maybe_unused]] int action,
		[[maybe_unused]] int mods) {}
	virtual void OnMouseMove(
		[[maybe_unused]] double x, [[maybe_unused]] double y) {}
	virtual void Draw(int width, int height) = 0;
	virtual void OnRunStart() {}
	virtual void OnRunEnd() {}
	virtual void OnKey(int /*key*/, int /*scanCode*/, int /*action*/, int /*mods*/){}

	static GLFWwindow* MakeWindow(int w, int h, const char* title);

	GLFWwindow* m_window;
};
