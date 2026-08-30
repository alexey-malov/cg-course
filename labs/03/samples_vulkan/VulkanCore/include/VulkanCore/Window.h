#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <functional>
#include <string>

struct GLFWwindow;

namespace vkc
{

/// Окно и ввод поверх GLFW
class Window
{
public:
	Window(const std::string& title, glm::ivec2 size);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	vk::Extent2D FramebufferSize() const;
	bool ShouldClose() const;
	bool Minimized() const;

	void PollEvents() const;

	/// Блокируется до следующего события. Нужно, пока окно свёрнуто.
	void WaitEvents() const;

	/// Создаёт поверхность показа. Владельцем становится RenderDevice.
	vk::SurfaceKHR CreateSurface(vk::Instance instance) const;

	/// Требуется бэкенду ImGui. Больше ни для чего.
	GLFWwindow* Handle() const { return m_window; }

	/// Обработчики событий.
	std::function<void(vk::Extent2D)> OnResize;
	std::function<void(int, int, int, int)> OnKey;
	std::function<void(glm::vec2, int, int, int)> OnMouseButton;
	std::function<void(glm::vec2, glm::vec2)> OnMouseMove;
	std::function<void(glm::vec2)> OnScroll;

private:
	static Window& From(GLFWwindow* window);

	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow* window, double x, double y);
	static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

	GLFWwindow* m_window = nullptr;
	glm::vec2 m_lastCursor{ 0.0f };
	bool m_hasCursor = false;
};

} // namespace vkc
