#pragma once

#include <VulkanCore/RenderDevice.h>

#include <optional>
#include <vector>

namespace vkc
{

/// Синхронизация кадров: ожидание, получение изображения, отправка, показ.
///
/// Здесь встречаются два разных номера. Слот кадра — это номер набора
/// ресурсов процессора, он идёт по кругу 0, 1, 0, 1... и по нему
/// индексируются командный буфер, барьер и семафор imageAvailable.
/// Индекс изображения выдаёт цепочка показа, порядок в нём произвольный,
/// и по нему индексируется только семафор renderFinished: показ завершается
/// асинхронно, поэтому семафор обязан быть привязан к изображению, а не
/// к слоту. Наружу, в vkc::Frame, уходит только слот.
class FrameLoop
{
public:
	FrameLoop(const RenderDevice& device, uint32_t framesInFlight, uint32_t imageCount);

	struct Acquired
	{
		uint32_t slot = 0;
		uint32_t imageIndex = 0;
		vk::CommandBuffer cmd;
	};

	/// nullopt означает, что цепочку показа нужно пересоздать.
	std::optional<Acquired> Begin(vk::SwapchainKHR swapchain);

	/// false означает то же самое.
	bool End(vk::SwapchainKHR swapchain, uint32_t imageIndex);

	void ResizeImageCount(uint32_t imageCount);

private:
	const RenderDevice* m_device = nullptr;

	vk::raii::CommandPool m_pool{ nullptr };
	std::vector<vk::raii::CommandBuffer> m_commandBuffers; // по слоту
	std::vector<vk::raii::Semaphore> m_imageAvailable; // по слоту
	std::vector<vk::raii::Fence> m_inFlight; // по слоту
	std::vector<vk::raii::Semaphore> m_renderFinished; // по индексу изображения

	uint32_t m_framesInFlight = 2;
	uint32_t m_slot = 0;
};

} // namespace vkc
