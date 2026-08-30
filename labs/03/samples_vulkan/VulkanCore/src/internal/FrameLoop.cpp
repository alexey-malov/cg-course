#include "FrameLoop.h"

#include <limits>
#include <stdexcept>

namespace vkc
{

FrameLoop::FrameLoop(const RenderDevice& device, uint32_t framesInFlight, uint32_t imageCount)
	: m_device(&device)
	, m_framesInFlight(framesInFlight)
{
	const vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = device.GraphicsFamily()
	};
	m_pool = vk::raii::CommandPool{ device.Raii(), poolInfo };

	const vk::CommandBufferAllocateInfo allocateInfo{ .commandPool = *m_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = framesInFlight };
	m_commandBuffers = vk::raii::CommandBuffers{ device.Raii(), allocateInfo };

	m_imageAvailable.reserve(framesInFlight);
	m_inFlight.reserve(framesInFlight);

	for (uint32_t i = 0; i < framesInFlight; ++i)
	{
		m_imageAvailable.emplace_back(device.Raii(), vk::SemaphoreCreateInfo{});
		m_inFlight.emplace_back(device.Raii(),
			vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
	}

	ResizeImageCount(imageCount);
}

void FrameLoop::ResizeImageCount(uint32_t imageCount)
{
	m_renderFinished.clear();
	m_renderFinished.reserve(imageCount);

	for (uint32_t i = 0; i < imageCount; ++i)
		m_renderFinished.emplace_back(m_device->Raii(), vk::SemaphoreCreateInfo{});
}

std::optional<FrameLoop::Acquired> FrameLoop::Begin(vk::SwapchainKHR swapchain)
{
	const vk::Device device = m_device->Handle();

	const vk::Result waitResult = m_device->Raii().waitForFences(
		*m_inFlight[m_slot], vk::True, std::numeric_limits<uint64_t>::max());
	if (waitResult != vk::Result::eSuccess)
		throw std::runtime_error("ожидание кадра завершилось ошибкой");

	uint32_t imageIndex = 0;
	const vk::Result acquireResult = static_cast<vk::Result>(
		device.acquireNextImageKHR(swapchain, std::numeric_limits<uint64_t>::max(),
			*m_imageAvailable[m_slot], nullptr, &imageIndex));

	if (acquireResult == vk::Result::eErrorOutOfDateKHR)
		return std::nullopt;

	if (acquireResult != vk::Result::eSuccess && acquireResult != vk::Result::eSuboptimalKHR)
		throw std::runtime_error("не удалось получить изображение цепочки показа");

	// Барьер сбрасывается только после того, как стало ясно, что кадр
	// действительно будет отправлен: иначе следующий Begin повиснет.
	m_device->Raii().resetFences(*m_inFlight[m_slot]);

	m_commandBuffers[m_slot].reset();

	return Acquired{ .slot = m_slot, .imageIndex = imageIndex, .cmd = *m_commandBuffers[m_slot] };
}

bool FrameLoop::End(vk::SwapchainKHR swapchain, uint32_t imageIndex)
{
	const vk::CommandBuffer cmd = *m_commandBuffers[m_slot];

	const vk::Semaphore waitSemaphore = *m_imageAvailable[m_slot];
	const vk::Semaphore signalSemaphore = *m_renderFinished[imageIndex];

	constexpr vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1,
		.pWaitSemaphores = &waitSemaphore,
		.pWaitDstStageMask = &waitStage,
		.commandBufferCount = 1,
		.pCommandBuffers = &cmd,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &signalSemaphore };

	m_device->GraphicsQueue().submit(submitInfo, *m_inFlight[m_slot]);

	const vk::PresentInfoKHR presentInfo{ .waitSemaphoreCount = 1,
		.pWaitSemaphores = &signalSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &imageIndex };

	const vk::Result presentResult = static_cast<vk::Result>(m_device->PresentQueue().presentKHR(&presentInfo));

	m_slot = (m_slot + 1) % m_framesInFlight;

	if (presentResult == vk::Result::eErrorOutOfDateKHR
		|| presentResult == vk::Result::eSuboptimalKHR)
		return false;

	if (presentResult != vk::Result::eSuccess)
		throw std::runtime_error("показ кадра завершился ошибкой");

	return true;
}

} // namespace vkc
