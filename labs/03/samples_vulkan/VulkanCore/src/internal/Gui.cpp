#include "Gui.h"

#include <VulkanCore/Window.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <array>
#include <filesystem>
#include <stdexcept>

namespace vkc
{

namespace
{

void CheckVulkanResult(VkResult result)
{
	if (result != VK_SUCCESS)
		throw std::runtime_error("ошибка Vulkan внутри бэкенда ImGui");
}

/// Знаки, которых нет в кириллическом наборе ImGui, но которые встречаются
/// в русском тексте. Без них на их месте рисуется пустой квадрат.
constexpr const char* kExtraGlyphs = "—–…№";

} // namespace

void Gui::LoadFont(const std::string& fontPath)
{
	ImGuiIO& io = ImGui::GetIO();

	// Без кириллического диапазона вместо русских букв будут пустые квадраты.
	if (!std::filesystem::exists(fontPath))
		return;

	ImFontGlyphRangesBuilder builder;
	builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
	builder.AddText(kExtraGlyphs);
	builder.BuildRanges(&m_glyphs);

	io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, nullptr, m_glyphs.Data);
}

Gui::Gui(const RenderDevice& device, const Window& window, vk::Format colorFormat,
	uint32_t imageCount, const std::string& fontPath)
{
	constexpr uint32_t kPoolSize = 64;

	const std::array poolSizes{
		vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, kPoolSize },
	};

	const vk::DescriptorPoolCreateInfo poolInfo{
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = kPoolSize,
		.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
		.pPoolSizes = poolSizes.data()
	};

	m_pool = vk::raii::DescriptorPool{ device.Raii(), poolInfo };

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	LoadFont(fontPath);

	ImGui_ImplGlfw_InitForVulkan(window.Handle(), true);

	VkFormat format = static_cast<VkFormat>(colorFormat);

	VkPipelineRenderingCreateInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachmentFormats = &format;

	ImGui_ImplVulkan_InitInfo initInfo{};
	initInfo.Instance = static_cast<VkInstance>(device.Instance());
	initInfo.PhysicalDevice = static_cast<VkPhysicalDevice>(device.PhysicalDevice());
	initInfo.Device = static_cast<VkDevice>(device.Handle());
	initInfo.QueueFamily = device.GraphicsFamily();
	initInfo.Queue = static_cast<VkQueue>(device.GraphicsQueue());
	initInfo.DescriptorPool = static_cast<VkDescriptorPool>(*m_pool);
	initInfo.MinImageCount = imageCount;
	initInfo.ImageCount = imageCount;
	initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	initInfo.UseDynamicRendering = true;
	initInfo.PipelineRenderingCreateInfo = renderingInfo;
	initInfo.CheckVkResultFn = CheckVulkanResult;

	ImGui_ImplVulkan_Init(&initInfo);
}

Gui::~Gui()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::NewFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Gui::Render(vk::CommandBuffer cmd)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<VkCommandBuffer>(cmd));
}

bool Gui::WantsMouse() const
{
	return ImGui::GetIO().WantCaptureMouse;
}

bool Gui::WantsKeyboard() const
{
	return ImGui::GetIO().WantCaptureKeyboard;
}

void Gui::DrawFpsWidget(const FpsCounter& clock)
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(
		{ viewport->WorkPos.x + viewport->WorkSize.x - 10.0f, viewport->WorkPos.y + 10.0f },
		ImGuiCond_Always, { 1.0f, 0.0f });
	ImGui::SetNextWindowBgAlpha(0.4f);

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoNav;

	if (ImGui::Begin("Счётчик кадров", nullptr, flags))
	{
		ImGui::Text("%.1f кадр/с", static_cast<double>(clock.Fps()));
		ImGui::Text("%.2f мс", static_cast<double>(clock.DeltaTime() * 1000.0f));
	}
	ImGui::End();
}

} // namespace vkc
