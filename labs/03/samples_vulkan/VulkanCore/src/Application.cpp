#include <VulkanCore/Application.h>
#include <VulkanCore/FpsCounter.h>
#include <VulkanCore/Rendering.h>
#include <VulkanCore/Window.h>

#include "internal/FrameLoop.h"
#include "internal/Gui.h"
#include "internal/Swapchain.h"

#include <optional>

namespace vkc
{

namespace
{

/// Переводит изображение цепочки показа из одной раскладки в другую.
///
/// Раскладка — это способ, которым видеокарта хранит изображение в памяти.
/// Для рисования и для показа на экране они разные.
void TransitionImage(vk::CommandBuffer cmd, vk::Image image,
	vk::ImageLayout from, vk::ImageLayout to,
	vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccess,
	vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess)
{
	const vk::ImageMemoryBarrier2 barrier{
		.srcStageMask = srcStage,
		.srcAccessMask = srcAccess,
		.dstStageMask = dstStage,
		.dstAccessMask = dstAccess,
		.oldLayout = from,
		.newLayout = to,
		.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
		.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
		.image = image,
		.subresourceRange = { .aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1 }
	};

	cmd.pipelineBarrier2(
		vk::DependencyInfo{ .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier });
}

} // namespace

struct Application::Impl
{
	explicit Impl(Config configuration)
		: config(std::move(configuration))
		, window(config.title, config.size)
		, device(window)
		, shaders(device)
		, swapchain(device, window.FramebufferSize())
		, frameLoop(device, config.framesInFlight, swapchain.ImageCount())
	{
		if (config.gui)
		{
			gui.emplace(device, window, swapchain.Format(), swapchain.ImageCount(),
				config.fontPath);
		}
	}

	Config config;
	Window window;
	RenderDevice device;
	ShaderCache shaders;
	Swapchain swapchain;
	FrameLoop frameLoop;

	std::optional<Gui> gui;

	FpsCounter clock;
	bool resizeRequested = false;
};

Application::Application(Config config)
	: m_impl(std::make_unique<Impl>(std::move(config)))
{
	Window& window = m_impl->window;

	window.OnResize = [this](vk::Extent2D) { m_impl->resizeRequested = true; };

	window.OnKey = [this](int key, int scancode, int action, int mods) {
		if (m_impl->gui && m_impl->gui->WantsKeyboard())
			return;
		OnKey(key, scancode, action, mods);
	};

	window.OnMouseButton = [this](glm::vec2 pos, int button, int action, int mods) {
		if (m_impl->gui && m_impl->gui->WantsMouse())
			return;
		OnMouseButton(pos, button, action, mods);
	};

	window.OnMouseMove = [this](glm::vec2 pos, glm::vec2 delta) {
		if (m_impl->gui && m_impl->gui->WantsMouse())
			return;
		OnMouseMove(pos, delta);
	};

	window.OnScroll = [this](glm::vec2 offset) {
		if (m_impl->gui && m_impl->gui->WantsMouse())
			return;
		OnScroll(offset);
	};
}

Application::~Application() = default;

void Application::OnKey(int, int, int, int) {}
void Application::OnMouseButton(glm::vec2, int, int, int) {}
void Application::OnMouseMove(glm::vec2, glm::vec2) {}
void Application::OnScroll(glm::vec2) {}

const RenderDevice& Application::Device() const
{
	return m_impl->device;
}

ShaderCache& Application::Shaders()
{
	return m_impl->shaders;
}

vk::Extent2D Application::Extent() const
{
	return m_impl->swapchain.Extent();
}

uint32_t Application::FramesInFlight() const
{
	return m_impl->config.framesInFlight;
}

TargetFormats Application::Formats() const
{
	return TargetFormats{ .color = m_impl->swapchain.Format() };
}

void Application::Run()
{
	Impl& impl = *m_impl;

	// Цепочка показа создана под конкретный размер окна, поэтому при его
	// изменении её нужно создать заново. У свёрнутого окна размер нулевой,
	// и цепочку для него не построить — сначала ждём, пока его развернут.
	const auto recreate = [&] {
		while (impl.window.Minimized() && !impl.window.ShouldClose())
			impl.window.WaitEvents();

		if (impl.window.ShouldClose())
			return;

		impl.swapchain.Recreate(impl.window.FramebufferSize());
		impl.frameLoop.ResizeImageCount(impl.swapchain.ImageCount());
		impl.resizeRequested = false;

		OnResize(impl.swapchain.Extent());
	};

	while (!impl.window.ShouldClose())
	{
		impl.window.PollEvents();

		if (impl.window.Minimized())
		{
			impl.window.WaitEvents();
			continue;
		}

		if (impl.resizeRequested)
		{
			recreate();
			continue;
		}

		impl.clock.Tick();
		OnUpdate(impl.clock.DeltaTime());

		const std::optional<FrameLoop::Acquired> acquired = impl.frameLoop.Begin(impl.swapchain.Handle());

		if (!acquired.has_value())
		{
			recreate();
			continue;
		}

		const vk::Image image = impl.swapchain.Image(acquired->imageIndex);

		const Frame frame{ .cmd = acquired->cmd,
			.slot = acquired->slot,
			.color = impl.swapchain.View(acquired->imageIndex),
			.extent = impl.swapchain.Extent() };

		frame.cmd.begin(
			vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		// Номер слота известен только теперь, поэтому писать в копию
		// текущего кадра пример может отсюда, но не из OnUpdate.
		OnPrepare(frame);

		if (impl.gui.has_value())
		{
			impl.gui->NewFrame();
			Gui::DrawFpsWidget(impl.clock);
			OnGui();
		}

		// Старое содержимое изображения не нужно: рисование начинается
		// с очистки. Раскладка eUndefined разрешает видеокарте его выбросить.
		TransitionImage(frame.cmd, image, vk::ImageLayout::eUndefined,
			vk::ImageLayout::eColorAttachmentOptimal,
			vk::PipelineStageFlagBits2::eTopOfPipe, vk::AccessFlagBits2::eNone,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::AccessFlagBits2::eColorAttachmentWrite);

		OnRender(frame);

		// Интерфейс рисуется поверх отдельным проходом: так примеру не нужно
		// знать, какой из его проходов последний.
		if (impl.gui.has_value())
		{
			Rendering guiPass{ frame, RenderingInfo::Continue() };
			impl.gui->Render(frame.cmd);
		}

		TransitionImage(frame.cmd, image, vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eNone);

		frame.cmd.end();

		if (!impl.frameLoop.End(impl.swapchain.Handle(), acquired->imageIndex))
			recreate();
	}

	OnShutdown();

	// Поля наследника разрушатся раньше базы, поэтому дожидаться простоя
	// устройства надо здесь, а не в деструкторе.
	impl.device.WaitIdle();
}

} // namespace vkc
