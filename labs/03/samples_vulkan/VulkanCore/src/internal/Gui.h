#pragma once

#include <VulkanCore/FpsCounter.h>
#include <VulkanCore/RenderDevice.h>

#include <imgui.h>

#include <string>

namespace vkc
{

class Window;

/// Обёртка над ImGui и его бэкендами
class Gui
{
public:
	Gui(const RenderDevice& device, const Window& window, vk::Format colorFormat,
		uint32_t imageCount, const std::string& fontPath);

	~Gui();

	Gui(const Gui&) = delete;
	Gui& operator=(const Gui&) = delete;

	void NewFrame();
	void Render(vk::CommandBuffer cmd);

	bool WantsMouse() const;
	bool WantsKeyboard() const;

	/// Счётчик кадров одинаков во всех примерах, поэтому рисуется здесь.
	static void DrawFpsWidget(const FpsCounter& clock);

private:
	void LoadFont(const std::string& fontPath);

	vk::raii::DescriptorPool m_pool{ nullptr };

	/// ImGui запоминает указатель на набор символов и читает его при сборке
	/// атласа, поэтому набор обязан пережить загрузку шрифта.
	ImVector<ImWchar> m_glyphs;
};

} // namespace vkc
