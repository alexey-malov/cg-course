#include <VulkanCore/Window.h>

#include <GLFW/glfw3.h>

#include <stdexcept>

namespace vkc
{

namespace
{
int g_windowCount = 0;
}

Window::Window(const std::string& title, glm::ivec2 size)
{
	if (g_windowCount == 0 && glfwInit() != GLFW_TRUE)
		throw std::runtime_error("не удалось инициализировать GLFW");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
	if (m_window == nullptr)
	{
		if (g_windowCount == 0)
			glfwTerminate();
		throw std::runtime_error("не удалось создать окно");
	}
	++g_windowCount;

	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
	glfwSetKeyCallback(m_window, KeyCallback);
	glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
	glfwSetCursorPosCallback(m_window, CursorPosCallback);
	glfwSetScrollCallback(m_window, ScrollCallback);
}

Window::~Window()
{
	if (m_window != nullptr)
	{
		glfwDestroyWindow(m_window);
		--g_windowCount;
	}
	if (g_windowCount == 0)
		glfwTerminate();
}

vk::Extent2D Window::FramebufferSize() const
{
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(m_window, &width, &height);
	return vk::Extent2D{ .width = static_cast<uint32_t>(width),
		.height = static_cast<uint32_t>(height) };
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_window) == GLFW_TRUE;
}

bool Window::Minimized() const
{
	const vk::Extent2D size = FramebufferSize();
	return size.width == 0 || size.height == 0;
}

void Window::PollEvents() const
{
	glfwPollEvents();
}

void Window::WaitEvents() const
{
	glfwWaitEvents();
}

vk::SurfaceKHR Window::CreateSurface(vk::Instance instance) const
{
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	const VkResult result = glfwCreateWindowSurface(static_cast<VkInstance>(instance), m_window, nullptr, &surface);

	if (result != VK_SUCCESS)
		throw std::runtime_error("не удалось создать поверхность для окна");

	return vk::SurfaceKHR{ surface };
}

Window& Window::From(GLFWwindow* window)
{
	return *static_cast<Window*>(glfwGetWindowUserPointer(window));
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	Window& self = From(window);
	if (self.OnResize)
	{
		self.OnResize(vk::Extent2D{ .width = static_cast<uint32_t>(width),
			.height = static_cast<uint32_t>(height) });
	}
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window& self = From(window);
	if (self.OnKey)
		self.OnKey(key, scancode, action, mods);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Window& self = From(window);
	if (!self.OnMouseButton)
		return;

	double x = 0.0;
	double y = 0.0;
	glfwGetCursorPos(window, &x, &y);
	self.OnMouseButton(glm::vec2{ static_cast<float>(x), static_cast<float>(y) }, button, action,
		mods);
}

void Window::CursorPosCallback(GLFWwindow* window, double x, double y)
{
	Window& self = From(window);

	const glm::vec2 position{ static_cast<float>(x), static_cast<float>(y) };
	const glm::vec2 delta = self.m_hasCursor ? position - self.m_lastCursor : glm::vec2{ 0.0f };

	self.m_lastCursor = position;
	self.m_hasCursor = true;

	if (self.OnMouseMove)
		self.OnMouseMove(position, delta);
}

void Window::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	Window& self = From(window);
	if (self.OnScroll)
		self.OnScroll(glm::vec2{ static_cast<float>(xOffset), static_cast<float>(yOffset) });
}

} // namespace vkc
